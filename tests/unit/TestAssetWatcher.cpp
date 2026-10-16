#include <doctest/doctest.h>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>
#include "engine/assets/AssetDatabase.hpp"
#include "engine/assets/AssetWatcher.hpp"

using namespace engine;

TEST_CASE("AssetWatcher detects texture modification") {
    std::filesystem::create_directories("watch_project/assets");
    const std::filesystem::path texture = "watch_project/assets/player.png";

    {
        std::ofstream file(texture, std::ios::binary);
        file << "version-1";
    }

    AssetDatabase database;
    CHECK(database.initialize("watch_project"));

    const AssetGuid guid = database.importAsset(texture);
    CHECK_FALSE(guid.empty());

    AssetWatcher watcher;
    bool changed = false;
    CHECK(watcher.start(database, [&](const std::filesystem::path&) {
        changed = true;
    }));

    {
        std::ofstream file(texture, std::ios::binary);
        file << "version-2";
    }

    // In environments where OS file system events take some time or need simulation
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    // Also trigger manually if watcher is stubbed without background threads
    watcher.triggerChange(texture);

    watcher.processPendingChanges();

    CHECK(changed);

    watcher.stop();
    database.shutdown();
    std::filesystem::remove_all("watch_project");
}
