#pragma once

#if __has_include(<efsw/efsw.hpp>)
#include <efsw/efsw.hpp>
#define ENGINE_HAS_EFSW 1
#else
#define ENGINE_HAS_EFSW 0
namespace efsw {
    using WatchID = long;
    enum Action { Add = 1, Delete = 2, Modified = 3, Moved = 4 };
    struct Actions {
        static constexpr Action Add = Action::Add;
        static constexpr Action Delete = Action::Delete;
        static constexpr Action Modified = Action::Modified;
        static constexpr Action Moved = Action::Moved;
    };
    class FileWatchListener {
    public:
        virtual ~FileWatchListener() = default;
        virtual void handleFileAction(WatchID, const std::string&, const std::string&, Action, std::string) = 0;
    };
    class FileWatcher {
    public:
        WatchID addWatch(const std::string&, FileWatchListener*, bool) { return 1; }
        void removeWatch(WatchID) {}
        void watch() {}
    };
}
#endif

#include <chrono>
#include <deque>
#include <filesystem>
#include <functional>
#include <mutex>
#include <string>
#include "engine/assets/AssetDatabase.hpp"

namespace engine {

class AssetWatcher final : public efsw::FileWatchListener {
public:
    using ChangeCallback = std::function<void(const std::filesystem::path& path)>;

    AssetWatcher() = default;
    ~AssetWatcher() override { stop(); }

    bool start(AssetDatabase& database, ChangeCallback callback);
    void stop();

    void handleFileAction(
        efsw::WatchID watchId,
        const std::string& directory,
        const std::string& filename,
        efsw::Action action,
        std::string oldFilename = ""
    ) override;

    void processPendingChanges();

    void triggerChange(const std::filesystem::path& path);

private:
    AssetDatabase* database_ = nullptr;
    ChangeCallback callback_;
    efsw::FileWatcher watcher_;
    efsw::WatchID watchId_ = -1;
    std::mutex mutex_;
    std::deque<std::filesystem::path> pending_;
};

} // namespace engine
