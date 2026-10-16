#pragma once

#include <SDL3/SDL.h>
#include <webgpu/webgpu_cpp.h>
#include <optional>

#include "editor/UndoManager.hpp"
#include "engine/audio/AudioEngine.hpp"
#include "engine/ecs/World.hpp"
#include "engine/input/InputManager.hpp"
#include "engine/renderer/GPUContext.hpp"
#include "engine/renderer/SpriteBatch.hpp"
#include "engine/scripting/LuaRuntime.hpp"

namespace editor {

enum class EditorMode {
    Edit,
    Play
};

class EditorApp {
public:
    bool initialize(SDL_Window* window);
    void shutdown();

    void handleEvent(const SDL_Event& event);
    void newFrame(double delta = 0.016);
    void render();

    void enterPlayMode();
    void exitPlayMode();

    [[nodiscard]] bool wantCaptureInput() const;

private:
    void drawDockspace();
    void drawHierarchy();
    void drawInspector();

    [[nodiscard]] engine::ecs::World& activeWorld() noexcept {
        return (mode_ == EditorMode::Play && playWorld_.has_value()) ? *playWorld_ : world_;
    }

    EditorMode mode_ = EditorMode::Edit;

    engine::ecs::World world_;
    engine::ecs::World editWorld_;
    std::optional<engine::ecs::World> playWorld_;

    engine::InputManager input_;
    engine::AudioEngine audio_;
    engine::LuaRuntime lua_;
    UndoManager undo_;

    engine::GPUContext gpu_;
    engine::SpriteBatch batch_;

    engine::ecs::Entity selectedEntity_;
    bool initialized_ = false;
};

} // namespace editor
