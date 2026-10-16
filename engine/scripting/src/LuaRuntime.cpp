#include "engine/scripting/LuaRuntime.hpp"
#include <spdlog/spdlog.h>

namespace engine {

bool LuaRuntime::initialize(
    World& world,
    InputManager& input,
    AudioEngine& audio
) {
    world_ = &world;
    input_ = &input;
    audio_ = &audio;

    lua_.open_libraries(
        sol::lib::base,
        sol::lib::math,
        sol::lib::string,
        sol::lib::table
    );

    registerApi();
    valid_ = true;

    return true;
}

void LuaRuntime::shutdown() {
    valid_ = false;
    lua_ = sol::state{};
}

bool LuaRuntime::loadScript(const std::string& path) {
    scriptPath_ = path;

    const auto result = lua_.safe_script_file(path, sol::script_pass_on_error);

    if (!result.valid()) {
        const sol::error error = result;
        spdlog::error("Lua script error in {}: {}", path, error.what());
        valid_ = false;
        return false;
    }

    onStart_ = lua_["onStart"];
    onUpdate_ = lua_["onUpdate"];

    if (onStart_.valid()) {
        callOnStart();
    }

    valid_ = true;
    return true;
}

void LuaRuntime::reload() {
    if (scriptPath_.empty()) {
        return;
    }

    spdlog::info("Reloading Lua script: {}", scriptPath_);
    loadScript(scriptPath_);
}

void LuaRuntime::callOnStart() {
    if (!onStart_.valid()) {
        return;
    }

    const auto result = onStart_();

    if (!result.valid()) {
        const sol::error error = result;
        spdlog::error("Lua onStart error: {}", error.what());
    }
}

void LuaRuntime::callOnUpdate(double delta) {
    if (!onUpdate_.valid()) {
        return;
    }

    const auto result = onUpdate_(delta);

    if (!result.valid()) {
        const sol::error error = result;
        spdlog::error("Lua onUpdate error: {}", error.what());
    }
}

void LuaRuntime::registerApi() {
    lua_.new_usertype<core::Vec3>(
        "Vec3",
        "x", &core::Vec3::x,
        "y", &core::Vec3::y,
        "z", &core::Vec3::z
    );

    lua_.new_usertype<core::Vec2>(
        "Vec2",
        "x", &core::Vec2::x,
        "y", &core::Vec2::y
    );

    lua_.new_usertype<glm::vec2>(
        "GlmVec2",
        "x", &glm::vec2::x,
        "y", &glm::vec2::y
    );

    lua_.new_usertype<TransformComponent>(
        "Transform",
        "position", &TransformComponent::position,
        "rotation", &TransformComponent::rotation,
        "scale", &TransformComponent::scale
    );

    lua_.new_usertype<MovementComponent>(
        "Movement",
        "speed", &MovementComponent::speed,
        "velocity", &MovementComponent::velocity
    );

    lua_.new_usertype<Entity>(
        "Entity",
        "valid", &Entity::isValid
    );

    sol::table engineTable = lua_["engine"].get_or_create<sol::table>();

    engineTable.set_function(
        "get_entity",
        [&](const std::string& name) {
            return world_->getEntityByName(name);
        }
    );

    engineTable.set_function(
        "get_transform",
        [&](Entity entity) -> TransformComponent* {
            return world_->tryGet<TransformComponent>(entity);
        }
    );

    engineTable.set_function(
        "get_movement",
        [&](Entity entity) -> MovementComponent* {
            return world_->tryGet<MovementComponent>(entity);
        }
    );

    engineTable.set_function(
        "play_sound",
        [&](const std::string& path) {
            audio_->playOneShot(path, AudioBus::Sfx);
        }
    );

    engineTable.set_function(
        "move_axis",
        [&]() {
            return input_->movementAxis();
        }
    );

    engineTable.set_function(
        "log",
        [](const std::string& message) {
            spdlog::info("[Lua] {}", message);
        }
    );
}

} // namespace engine
