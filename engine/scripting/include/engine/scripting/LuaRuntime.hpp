#pragma once

#include <sol/sol.hpp>
#include <string>
#include "engine/audio/AudioEngine.hpp"
#include "engine/ecs/World.hpp"
#include "engine/input/InputManager.hpp"

namespace engine {

class LuaRuntime {
public:
    bool initialize(World& world, InputManager& input, AudioEngine& audio);
    void shutdown();

    bool loadScript(const std::string& path);
    bool reload();

    void callOnStart();
    void callOnUpdate(double delta);

    [[nodiscard]] const std::string& lastError() const noexcept { return lastError_; }
    [[nodiscard]] bool isValid() const noexcept { return valid_; }
    [[nodiscard]] sol::state& lua() noexcept { return lua_; }

private:
    void registerApi();

    sol::state lua_;
    World* world_ = nullptr;
    InputManager* input_ = nullptr;
    AudioEngine* audio_ = nullptr;

    sol::protected_function onStart_;
    sol::protected_function onUpdate_;
    std::string scriptPath_;
    std::string lastError_;
    bool valid_ = false;
};

} // namespace engine
