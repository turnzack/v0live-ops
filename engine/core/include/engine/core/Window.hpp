#pragma once

#include "engine/core/Config.hpp"
#include <string_view>
#include <functional>
#include <memory>

namespace engine::core {

struct WindowNativeHandles {
    void* nativeWindowHandle{nullptr}; // HWND sous Windows
    void* nativeDisplayHandle{nullptr}; // HINSTANCE ou Display*
};

class Window {
public:
    using ResizeCallback = std::function<void(uint32_t width, uint32_t height)>;
    using CloseCallback = std::function<void()>;
    using KeyCallback = std::function<void(int key, bool pressed)>;

    Window();
    ~Window();

    bool init(const WindowConfig& config);
    void shutdown();

    void pollEvents();
    [[nodiscard]] bool shouldClose() const noexcept;
    void requestClose() noexcept;

    [[nodiscard]] uint32_t getWidth() const noexcept { return m_config.width; }
    [[nodiscard]] uint32_t getHeight() const noexcept { return m_config.height; }
    [[nodiscard]] std::string_view getTitle() const noexcept { return m_config.title; }
    [[nodiscard]] bool isMinimized() const noexcept { return m_isMinimized; }

    [[nodiscard]] WindowNativeHandles getNativeHandles() const noexcept;

    void setResizeCallback(ResizeCallback callback) { m_onResize = std::move(callback); }
    void setCloseCallback(CloseCallback callback) { m_onClose = std::move(callback); }
    void setKeyCallback(KeyCallback callback) { m_onKey = std::move(callback); }

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;

    WindowConfig m_config;
    bool m_shouldClose{false};
    bool m_isMinimized{false};

    ResizeCallback m_onResize;
    CloseCallback m_onClose;
    KeyCallback m_onKey;
};

} // namespace engine::core
