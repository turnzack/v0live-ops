#include "engine/assets/AssetDatabase.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "engine/core/Assert.hpp"
#include "engine/core/GUID.hpp"
#include "engine/core/SHA256.hpp"

#if __has_include(<openssl/evp.h>)
#include <openssl/evp.h>
#define ENGINE_HAS_OPENSSL 1
#else
#define ENGINE_HAS_OPENSSL 0
#endif

namespace engine {

namespace {

std::string toLower(std::string value) {
    std::transform(
        value.begin(), value.end(), value.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); }
    );
    return value;
}

std::string generateGuidV4() {
    return core::GUID::generate().asString();
}

} // namespace

bool AssetDatabase::initialize(const std::filesystem::path& projectRoot) {
    projectRoot_ = std::filesystem::absolute(projectRoot);
    metaRoot_ = projectRoot_ / ".engine" / "assets";
    std::filesystem::create_directories(metaRoot_);
    spdlog::info("Asset database initialized: {}", projectRoot_.string());
    return true;
}

void AssetDatabase::shutdown() {
    assets_.clear();
    pathToGuid_.clear();
}

AssetType AssetDatabase::detectType(const std::filesystem::path& path) {
    const std::string extension = toLower(path.extension().string());
    if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".webp") {
        return AssetType::Texture;
    }
    if (extension == ".wav" || extension == ".ogg" || extension == ".mp3") {
        return AssetType::Audio;
    }
    if (extension == ".lua") {
        return AssetType::Script;
    }
    if (path.string().ends_with(".scene.json") || extension == ".scene") {
        return AssetType::Scene;
    }
    if (path.string().ends_with(".prefab.json") || extension == ".prefab") {
        return AssetType::Prefab;
    }
    if (extension == ".glb" || extension == ".gltf") {
        return AssetType::Mesh;
    }
    return AssetType::Unknown;
}

std::string AssetDatabase::computeSha256(const std::filesystem::path& path) {
#if ENGINE_HAS_OPENSSL
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        return {};
    }

    EVP_MD_CTX* context = EVP_MD_CTX_new();
    ENGINE_VERIFY(context != nullptr, "Failed to create EVP context");
    EVP_DigestInit_ex(context, EVP_sha256(), nullptr);

    char buffer[8192];
    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0) {
        EVP_DigestUpdate(
            context, buffer, static_cast<std::size_t>(file.gcount())
        );
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLength = 0;
    EVP_DigestFinal_ex(context, hash, &hashLength);
    EVP_MD_CTX_free(context);

    std::ostringstream stream;
    for (unsigned int i = 0; i < hashLength; ++i) {
        stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return stream.str();
#else
    return core::SHA256::hashFile(path.string());
#endif
}

AssetGuid AssetDatabase::importAsset(const std::filesystem::path& path) {
    const std::filesystem::path absolutePath = std::filesystem::absolute(path);
    if (!std::filesystem::exists(absolutePath)) {
        spdlog::error("Cannot import missing asset: {}", absolutePath.string());
        return {};
    }

    if (const AssetRecord* existing = findByPath(absolutePath)) {
        return existing->guid;
    }

    AssetRecord record;
    record.guid = generateGuidV4();
    record.type = detectType(absolutePath);
    record.sourcePath = absolutePath;
    record.size = std::filesystem::file_size(absolutePath);
    record.sha256 = computeSha256(absolutePath);
    record.lastModified = static_cast<std::uint64_t>(
        std::filesystem::last_write_time(absolutePath).time_since_epoch().count()
    );

    if (!writeMetaFile(record)) {
        return {};
    }

    assets_[record.guid] = record;
    pathToGuid_[absolutePath.string()] = record.guid;

    spdlog::info(
        "Asset imported: {} ({})",
        absolutePath.filename().string(),
        record.guid
    );
    return record.guid;
}

bool AssetDatabase::reimportAsset(const AssetGuid& guid) {
    const auto iterator = assets_.find(guid);
    if (iterator == assets_.end()) {
        return false;
    }

    AssetRecord& record = iterator->second;
    if (!std::filesystem::exists(record.sourcePath)) {
        spdlog::warn("Cannot reimport missing asset: {}", record.sourcePath.string());
        return false;
    }

    const std::string newHash = computeSha256(record.sourcePath);
    if (newHash == record.sha256) {
        return true;
    }

    record.sha256 = newHash;
    record.size = std::filesystem::file_size(record.sourcePath);
    record.version++;
    record.lastModified = static_cast<std::uint64_t>(
        std::filesystem::last_write_time(record.sourcePath).time_since_epoch().count()
    );

    writeMetaFile(record);
    spdlog::info(
        "Asset reimported: {} (v{})",
        record.sourcePath.filename().string(),
        record.version
    );
    return true;
}

bool AssetDatabase::renameAsset(
    const AssetGuid& guid,
    const std::filesystem::path& newPath
) {
    return moveAsset(guid, newPath);
}

bool AssetDatabase::moveAsset(
    const AssetGuid& guid,
    const std::filesystem::path& newPath
) {
    const auto iterator = assets_.find(guid);
    if (iterator == assets_.end()) {
        return false;
    }

    AssetRecord& record = iterator->second;
    const std::filesystem::path oldPath = record.sourcePath;

    std::error_code ec;
    std::filesystem::rename(oldPath, newPath, ec);
    if (ec) {
        spdlog::error("Failed to move asset: {}", ec.message());
        return false;
    }

    record.sourcePath = std::filesystem::absolute(newPath);
    pathToGuid_.erase(oldPath.string());
    pathToGuid_[record.sourcePath.string()] = guid;

    writeMetaFile(record);
    return true;
}

bool AssetDatabase::removeAsset(const AssetGuid& guid) {
    const auto iterator = assets_.find(guid);
    if (iterator == assets_.end()) {
        return false;
    }

    pathToGuid_.erase(iterator->second.sourcePath.string());
    std::filesystem::remove(
        metaRoot_ / (iterator->second.guid + ".meta.json")
    );
    assets_.erase(iterator);
    return true;
}

const AssetRecord* AssetDatabase::find(const AssetGuid& guid) const {
    const auto iterator = assets_.find(guid);
    if (iterator == assets_.end()) {
        return nullptr;
    }
    return &iterator->second;
}

const AssetRecord* AssetDatabase::findByPath(
    const std::filesystem::path& path
) const {
    const auto iterator = pathToGuid_.find(
        std::filesystem::absolute(path).string()
    );
    if (iterator == pathToGuid_.end()) {
        return nullptr;
    }
    return find(iterator->second);
}

bool AssetDatabase::writeMetaFile(const AssetRecord& record) {
    nlohmann::json json = {
        {"guid", record.guid},
        {"type", static_cast<int>(record.type)},
        {"sourcePath", record.sourcePath.string()},
        {"sha256", record.sha256},
        {"size", record.size},
        {"version", record.version},
        {"lastModified", record.lastModified}
    };

    std::ofstream file(metaRoot_ / (record.guid + ".meta.json"));
    if (!file) {
        return false;
    }
    file << json.dump(4) << '\n';
    return true;
}

bool AssetDatabase::loadMetaFile(
    const std::filesystem::path& metaPath,
    AssetRecord& record
) {
    std::ifstream file(metaPath);
    if (!file) {
        return false;
    }

    nlohmann::json json;
    try {
        file >> json;
    } catch (...) {
        return false;
    }

    record.guid = json["guid"];
    record.type = static_cast<AssetType>(json["type"].get<int>());
    record.sourcePath = json["sourcePath"].get<std::string>();
    record.sha256 = json["sha256"];
    record.size = json["size"];
    record.version = json["version"];
    record.lastModified = json["lastModified"];
    return true;
}

} // namespace engine
