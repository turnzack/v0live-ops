#include "engine/assets/AssetWatcher.hpp"
#include <spdlog/spdlog.h>

namespace engine {

bool AssetWatcher::start(AssetDatabase& database, ChangeCallback callback) {
    database_ = &database;
    callback_ = std::move(callback);

    watchId_ = watcher_.addWatch(
        database_->projectRoot().string(),
        this,
        true
    );

    if (watchId_ < 0) {
        spdlog::error("Failed to watch asset directory.");
        return false;
    }

    watcher_.watch();
    spdlog::info("Asset watcher started for: {}", database_->projectRoot().string());
    return true;
}

void AssetWatcher::stop() {
    if (watchId_ >= 0) {
        watcher_.removeWatch(watchId_);
        watchId_ = -1;
    }
}

void AssetWatcher::handleFileAction(
    efsw::WatchID,
    const std::string& directory,
    const std::string& filename,
    efsw::Action action,
    const std::string&
) {
    if (action != efsw::Actions::Modified && action != efsw::Actions::Add && action != efsw::Actions::Moved) {
        return;
    }

    const std::filesystem::path path = std::filesystem::path(directory) / filename;
    const auto ext = path.extension();
    if (ext != ".png" && ext != ".jpg" && ext != ".jpeg" && ext != ".webp" && ext != ".lua" && ext != ".json") {
        return;
    }

    std::lock_guard lock(mutex_);
    pending_.push_back(path);
}

void AssetWatcher::triggerChange(const std::filesystem::path& path) {
    std::lock_guard lock(mutex_);
    pending_.push_back(path);
}

void AssetWatcher::processPendingChanges() {
    std::deque<std::filesystem::path> changes;
    {
        std::lock_guard lock(mutex_);
        changes.swap(pending_);
    }

    for (const auto& path : changes) {
        if (!database_) continue;

        if (const AssetRecord* record = database_->findByPath(path)) {
            database_->reimportAsset(record->guid);
            if (callback_) {
                callback_(path);
            }
        } else {
            database_->importAsset(path);
            if (callback_) {
                callback_(path);
            }
        }
    }
}

} // namespace engine
