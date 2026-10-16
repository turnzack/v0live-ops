#include "engine/input/InputManager.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace engine {

void InputManager::beginFrame() {
    previous_ = current_;
}

bool InputManager::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        const bool down = event.type == SDL_EVENT_KEY_DOWN;
        const auto iterator = bindings_.find(event.key.key);

        if (iterator != bindings_.end()) {
            setAction(iterator->second, down);
            return true;
        }
    }

    return false;
}

void InputManager::endFrame() {
}

bool InputManager::isDown(InputAction action) const noexcept {
    return (current_ & (1u << static_cast<std::uint32_t>(action))) != 0;
}

bool InputManager::wasPressed(InputAction action) const noexcept {
    const std::uint32_t mask = 1u << static_cast<std::uint32_t>(action);

    return (current_ & mask) != 0 && (previous_ & mask) == 0;
}

bool InputManager::wasReleased(InputAction action) const noexcept {
    const std::uint32_t mask = 1u << static_cast<std::uint32_t>(action);

    return (current_ & mask) == 0 && (previous_ & mask) != 0;
}

glm::vec2 InputManager::movementAxis() const noexcept {
    glm::vec2 axis{0.0f, 0.0f};

    if (isDown(InputAction::MoveLeft)) axis.x -= 1.0f;
    if (isDown(InputAction::MoveRight)) axis.x += 1.0f;
    if (isDown(InputAction::MoveUp)) axis.y -= 1.0f;
    if (isDown(InputAction::MoveDown)) axis.y += 1.0f;

    if (axis.x != 0.0f || axis.y != 0.0f) {
        axis = glm::normalize(axis);
    }

    return axis;
}

void InputManager::bindKey(SDL_Keycode key, InputAction action) {
    bindings_[key] = action;
}

void InputManager::loadBindings(const std::string& path) {
    bindings_.clear();

    bindKey(SDLK_LEFT, InputAction::MoveLeft);
    bindKey(SDLK_A, InputAction::MoveLeft);
    bindKey(SDLK_RIGHT, InputAction::MoveRight);
    bindKey(SDLK_D, InputAction::MoveRight);
    bindKey(SDLK_UP, InputAction::MoveUp);
    bindKey(SDLK_W, InputAction::MoveUp);
    bindKey(SDLK_DOWN, InputAction::MoveDown);
    bindKey(SDLK_S, InputAction::MoveDown);
    bindKey(SDLK_SPACE, InputAction::Jump);
    bindKey(SDLK_RETURN, InputAction::Confirm);
    bindKey(SDLK_ESCAPE, InputAction::Pause);

    std::ifstream file(path);

    if (!file) {
        spdlog::warn("Input bindings not found: {}. Using defaults.", path);
        return;
    }

    try {
        nlohmann::json json;
        file >> json;

        for (const auto& binding : json["bindings"]) {
            const std::string keyName = binding["key"];
            const std::string actionName = binding["action"];

            SDL_Keycode key = SDL_GetKeyFromName(keyName.c_str());

            if (key == SDLK_UNKNOWN) {
                spdlog::warn("Unknown key: {}", keyName);
                continue;
            }

            if (actionName == "MoveLeft") bindKey(key, InputAction::MoveLeft);
            else if (actionName == "MoveRight") bindKey(key, InputAction::MoveRight);
            else if (actionName == "MoveUp") bindKey(key, InputAction::MoveUp);
            else if (actionName == "MoveDown") bindKey(key, InputAction::MoveDown);
            else if (actionName == "Jump") bindKey(key, InputAction::Jump);
            else if (actionName == "Confirm") bindKey(key, InputAction::Confirm);
            else if (actionName == "Pause") bindKey(key, InputAction::Pause);
        }
    } catch (const std::exception& error) {
        spdlog::error("Failed to load input bindings: {}", error.what());
    }
}

void InputManager::saveBindings(const std::string& path) const {
    nlohmann::json bindings = nlohmann::json::array();

    for (const auto& [key, action] : bindings_) {
        std::string actionName;

        switch (action) {
            case InputAction::MoveLeft: actionName = "MoveLeft"; break;
            case InputAction::MoveRight: actionName = "MoveRight"; break;
            case InputAction::MoveUp: actionName = "MoveUp"; break;
            case InputAction::MoveDown: actionName = "MoveDown"; break;
            case InputAction::Jump: actionName = "Jump"; break;
            case InputAction::Confirm: actionName = "Confirm"; break;
            case InputAction::Pause: actionName = "Pause"; break;
            default: continue;
        }

        bindings.push_back({
            {"key", SDL_GetKeyName(key)},
            {"action", actionName}
        });
    }

    nlohmann::json json;
    json["bindings"] = bindings;

    std::ofstream file(path);

    if (file) {
        file << json.dump(4) << '\n';
    }
}

void InputManager::setAction(InputAction action, bool down) {
    const std::uint32_t mask = 1u << static_cast<std::uint32_t>(action);

    if (down) {
        current_ |= mask;
    } else {
        current_ &= ~mask;
    }
}

} // namespace engine
