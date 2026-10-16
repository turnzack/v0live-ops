#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>

namespace engine {

using AssetGuid = std::string;

enum class AssetType : std::uint8_t {
    Unknown,
    Texture,
    Audio,
    Script,
    Scene,
    Prefab,
    Mesh
};

struct AssetRecord {
    AssetGuid guid;
    AssetType type = AssetType::Unknown;
    std::filesystem::path sourcePath;
    std::filesystem::path importedPath;
    std::uint64_t size = 0;
    std::string sha256;
    std::uint64_t version = 1;
    std::uint64_t lastModified = 0;
};

class AssetDatabase {
public:
    bool initialize(const std::filesystem::path& projectRoot);
    void shutdown();

    AssetGuid importAsset(const std::filesystem::path& path);
    bool reimportAsset(const AssetGuid& guid);
    bool renameAsset(const AssetGuid& guid, const std::filesystem::path& newPath);
    bool moveAsset(const AssetGuid& guid, const std::filesystem::path& newPath);
    bool removeAsset(const AssetGuid& guid);

    [[nodiscard]] const AssetRecord* find(const AssetGuid& guid) const;
    [[nodiscard]] const AssetRecord* findByPath(const std::filesystem::path& path) const;

    [[nodiscard]] const std::unordered_map<AssetGuid, AssetRecord>& assets() const noexcept { return assets_; }
    [[nodiscard]] const std::filesystem::path& projectRoot() const noexcept { return projectRoot_; }

    static std::string computeSha256(const std::filesystem::path& path);

private:
    static AssetType detectType(const std::filesystem::path& path);

    bool writeMetaFile(const AssetRecord& record);
    bool loadMetaFile(const std::filesystem::path& metaPath, AssetRecord& record);

    std::filesystem::path projectRoot_;
    std::filesystem::path metaRoot_;
    std::unordered_map<AssetGuid, AssetRecord> assets_;
    std::unordered_map<std::string, AssetGuid> pathToGuid_;
};

} // namespace engine
