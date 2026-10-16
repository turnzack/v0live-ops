#pragma once

#include "engine/core/Config.hpp"
#include "engine/core/Window.hpp"
#include "engine/core/Time.hpp"
#include <string_view>

namespace engine::core {

class Application {
public:
    explicit Application(std::string_view configPath = "config.json");
    virtual ~Application();

    int run();
    void stop() noexcept;

protected:
    virtual bool onInit() { return true; }
    virtual void onUpdate(float deltaTime) { (void)deltaTime; }
    virtual void onFixedUpdate(float fixedDeltaTime) { (void)fixedDeltaTime; }
    virtual void onRender(float alpha) { (void)alpha; }
    virtual void onShutdown() {}

    [[nodiscard]] Window& getWindow() noexcept { return m_window; }
    [[nodiscard]] const Window& getWindow() const noexcept { return m_window; }
    [[nodiscard]] const Time& getTime() const noexcept { return m_time; }
    [[nodiscard]] const EngineConfig& getConfig() const noexcept { return m_config; }

private:
    EngineConfig m_config;
    Window m_window;
    Time m_time;
    bool m_running{false};
    float m_fpsLogTimer{0.0f};
};

} // namespace engine::core

namespace engine {
    using core::Application;
} // namespace engine
