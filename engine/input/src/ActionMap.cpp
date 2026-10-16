#include "engine/input/ActionMap.hpp"
#include "engine/core/Log.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <cctype>

namespace engine::input {

// ── Configuration ─────────────────────────────────────────────

void ActionMap::bind(std::string_view action, SDL_Scancode key) {
    auto& keys = bindings_[std::string(action)];
    // Avoid duplicates
    if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
        keys.push_back(key);
    }
}

void ActionMap::unbind(std::string_view action, SDL_Scancode key) {
    auto it = bindings_.find(std::string(action));
    if (it != bindings_.end()) {
        auto& keys = it->second;
        keys.erase(std::remove(keys.begin(), keys.end(), key), keys.end());
        if (keys.empty()) {
            bindings_.erase(it);
        }
    }
}

void ActionMap::clearAction(std::string_view action) {
    bindings_.erase(std::string(action));
}

void ActionMap::clearAll() {
    bindings_.clear();
}

// ── Query ─────────────────────────────────────────────────────

bool ActionMap::isActionDown(std::string_view action, const InputManager& input) const {
    auto it = bindings_.find(std::string(action));
    if (it == bindings_.end()) return false;
    for (auto key : it->second) {
        if (input.isKeyDown(key)) return true;
    }
    return false;
}

bool ActionMap::isActionPressed(std::string_view action, const InputManager& input) const {
    auto it = bindings_.find(std::string(action));
    if (it == bindings_.end()) return false;
    for (auto key : it->second) {
        if (input.isKeyPressed(key)) return true;
    }
    return false;
}

bool ActionMap::isActionReleased(std::string_view action, const InputManager& input) const {
    auto it = bindings_.find(std::string(action));
    if (it == bindings_.end()) return false;
    // Released = none currently down AND at least one was down last frame
    bool anyReleased = false;
    for (auto key : it->second) {
        if (input.isKeyDown(key)) return false; // Still held
        if (input.isKeyReleased(key)) anyReleased = true;
    }
    return anyReleased;
}

// ── Scancode ↔ String helpers ─────────────────────────────────

std::string ActionMap::scancodeToString(SDL_Scancode code) {
    const char* name = SDL_GetScancodeName(code);
    return name ? std::string(name) : "Unknown";
}

SDL_Scancode ActionMap::stringToScancode(std::string_view name) {
    // SDL3 provides SDL_GetScancodeFromName
    SDL_Scancode code = SDL_GetScancodeFromName(std::string(name).c_str());
    return code;
}

// ── Serialisation JSON ────────────────────────────────────────

bool ActionMap::loadFromFile(std::string_view filePath) {
    std::ifstream file(std::string(filePath));
    if (!file.is_open()) {
        LOG_WARNING(Input, "Cannot open action map file: ", filePath);
        return false;
    }

    try {
        nlohmann::json root;
        file >> root;

        clearAll();

        if (root.contains("actions") && root["actions"].is_object()) {
            for (auto& [actionName, actionData] : root["actions"].items()) {
                if (actionData.contains("keys") && actionData["keys"].is_array()) {
                    for (auto& keyEntry : actionData["keys"]) {
                        if (keyEntry.is_string()) {
                            SDL_Scancode code = stringToScancode(keyEntry.get<std::string>());
                            if (code != SDL_SCANCODE_UNKNOWN) {
                                bind(actionName, code);
                            } else {
                                LOG_WARNING(Input, "Unknown key name: '", keyEntry.get<std::string>(), "' for action '", actionName, "'");
                            }
                        }
                    }
                }
            }
        }

        LOG_INFO(Input, "Action map loaded from '", filePath, "' with ", bindings_.size(), " actions");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(Input, "Failed to parse action map JSON: ", e.what());
        return false;
    }
}

bool ActionMap::saveToFile(std::string_view filePath) const {
    nlohmann::json root;
    nlohmann::json actionsObj = nlohmann::json::object();

    for (const auto& [actionName, keys] : bindings_) {
        nlohmann::json keysArray = nlohmann::json::array();
        for (auto key : keys) {
            keysArray.push_back(scancodeToString(key));
        }
        actionsObj[actionName] = { {"keys", keysArray} };
    }

    root["actions"] = actionsObj;

    std::ofstream file(std::string(filePath));
    if (!file.is_open()) {
        LOG_ERROR(Input, "Cannot write action map to: ", filePath);
        return false;
    }

    file << root.dump(2);
    LOG_INFO(Input, "Action map saved to '", filePath, "'");
    return true;
}

std::vector<std::string> ActionMap::getActionNames() const {
    std::vector<std::string> names;
    names.reserve(bindings_.size());
    for (const auto& [name, _] : bindings_) {
        names.push_back(name);
    }
    return names;
}

std::vector<SDL_Scancode> ActionMap::getBindings(std::string_view action) const {
    auto it = bindings_.find(std::string(action));
    if (it != bindings_.end()) {
        return it->second;
    }
    return {};
}

} // namespace engine::input
