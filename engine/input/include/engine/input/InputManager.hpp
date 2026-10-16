#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace engine {

enum class InputAction : std::uint8_t {
    None = 0,
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    Jump,
    Confirm,
    Pause
};

class InputManager {
public:
    void beginFrame();
    bool handleEvent(const SDL_Event& event);
    void endFrame();

    [[nodiscard]] bool isDown(InputAction action) const noexcept;
    [[nodiscard]] bool wasPressed(InputAction action) const noexcept;
    [[nodiscard]] bool wasReleased(InputAction action) const noexcept;

    [[nodiscard]] glm::vec2 movementAxis() const noexcept;

    void bindKey(SDL_Keycode key, InputAction action);
    void loadBindings(const std::string& path);
    void saveBindings(const std::string& path) const;

private:
    std::uint32_t current_ = 0;
    std::uint32_t previous_ = 0;

    std::unordered_map<SDL_Keycode, InputAction> bindings_;

    void setAction(InputAction action, bool down);
};

} // namespace engine
