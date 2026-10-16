#pragma once

#include "engine/input/InputManager.hpp"
#include <SDL3/SDL.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace engine::input {

/// Maps logical action names ("jump", "moveLeft") to physical scancodes.
/// Serialisable en JSON pour permettre le remapping par l'utilisateur.
class ActionMap {
public:
    ActionMap() = default;

    // ── Configuration ─────────────────────────────────────────

    /// Bind a scancode to a named action. Multiple keys per action are allowed.
    void bind(std::string_view action, SDL_Scancode key);

    /// Remove a specific key binding from an action.
    void unbind(std::string_view action, SDL_Scancode key);

    /// Remove all bindings for an action.
    void clearAction(std::string_view action);

    /// Remove all bindings.
    void clearAll();

    // ── Query (requires an InputManager reference) ────────────

    /// True while any key bound to this action is held down.
    [[nodiscard]] bool isActionDown(std::string_view action, const InputManager& input) const;

    /// True only on the frame any key bound to this action was first pressed.
    [[nodiscard]] bool isActionPressed(std::string_view action, const InputManager& input) const;

    /// True only on the frame all keys bound to this action were released.
    [[nodiscard]] bool isActionReleased(std::string_view action, const InputManager& input) const;

    // ── Serialisation JSON ────────────────────────────────────

    /// Load action map from a JSON file. Returns false on failure.
    bool loadFromFile(std::string_view filePath);

    /// Save current action map to a JSON file.
    bool saveToFile(std::string_view filePath) const;

    /// Get all registered action names.
    [[nodiscard]] std::vector<std::string> getActionNames() const;

    /// Get all scancodes bound to an action.
    [[nodiscard]] std::vector<SDL_Scancode> getBindings(std::string_view action) const;

    // ── Scancode ↔ String helpers ─────────────────────────────

    /// Convert a scancode to its human-readable name.
    [[nodiscard]] static std::string scancodeToString(SDL_Scancode code);

    /// Convert a string name back to a scancode (case-insensitive).
    [[nodiscard]] static SDL_Scancode stringToScancode(std::string_view name);

private:
    std::unordered_map<std::string, std::vector<SDL_Scancode>> bindings_;
};

} // namespace engine::input
