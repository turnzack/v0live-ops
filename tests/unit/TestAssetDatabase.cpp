#include <doctest/doctest.h>
#include <filesystem>
#include <fstream>
#include "engine/assets/AssetDatabase.hpp"

using namespace engine;

TEST_CASE("Asset import creates a stable GUID") {
    std::filesystem::create_directories("test_project/assets");
    std::filesystem::path assetPath = "test_project/assets/player.png";

    {
        std::ofstream file(assetPath, std::ios::binary);
        file << "fake-png-data";
    }

    AssetDatabase database;
    CHECK(database.initialize("test_project"));

    const AssetGuid first = database.importAsset(assetPath);
    const AssetGuid second = database.importAsset(assetPath);

    CHECK_FALSE(first.empty());
    CHECK(first == second);

    const AssetRecord* record = database.find(first);
    REQUIRE(record != nullptr);
    CHECK(record->type == AssetType::Texture);
    CHECK_FALSE(record->sha256.empty());

    // Clean up
    database.shutdown();
    std::filesystem::remove_all("test_project");
}

TEST_CASE("Asset rename preserves GUID") {
    std::filesystem::create_directories("test_project/assets");
    std::filesystem::path assetPath = "test_project/assets/player.png";
    std::filesystem::path newPath = "test_project/assets/hero.png";

    {
        std::ofstream file(assetPath, std::ios::binary);
        file << "fake-png-data";
    }

    AssetDatabase database;
    CHECK(database.initialize("test_project"));

    const AssetGuid guid = database.importAsset(assetPath);
    CHECK_FALSE(guid.empty());

    CHECK(database.renameAsset(guid, newPath));

    const AssetRecord* record = database.find(guid);
    REQUIRE(record != nullptr);
    CHECK(record->sourcePath.filename() == "hero.png");
    CHECK(record->guid == guid);

    // Clean up
    database.shutdown();
    std::filesystem::remove_all("test_project");
}

TEST_CASE("Asset SHA-256 changes on content modification") {
    std::filesystem::create_directories("test_project/assets");
    std::filesystem::path assetPath = "test_project/assets/script.lua";

    {
        std::ofstream file(assetPath);
        file << "print('hello')";
    }

    AssetDatabase database;
    CHECK(database.initialize("test_project"));

    const AssetGuid guid = database.importAsset(assetPath);
    CHECK_FALSE(guid.empty());

    const AssetRecord* rec1 = database.find(guid);
    REQUIRE(rec1 != nullptr);
    const std::string originalHash = rec1->sha256;
    CHECK_FALSE(originalHash.empty());

    // Modify file
    {
        std::ofstream file(assetPath);
        file << "print('world modified version')";
    }

    CHECK(database.reimportAsset(guid));

    const AssetRecord* rec2 = database.find(guid);
    REQUIRE(rec2 != nullptr);
    CHECK(rec2->sha256 != originalHash);
    CHECK(rec2->version == 2);

    // Clean up
    database.shutdown();
    std::filesystem::remove_all("test_project");
}
