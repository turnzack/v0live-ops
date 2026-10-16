#pragma once

#include <SDL3/SDL.h>
#include <webgpu/webgpu_cpp.h>

#include "engine/ecs/World.hpp"
#include "engine/input/InputManager.hpp"
#include "engine/renderer/GPUContext.hpp"
#include "engine/renderer/SpriteBatch.hpp"

namespace editor {

class EditorApp {
public:
    bool initialize(SDL_Window* window);
    void shutdown();

    void handleEvent(const SDL_Event& event);
    void newFrame();
    void render();

    [[nodiscard]] bool wantCaptureInput() const;

private:
    void drawDockspace();
    void drawHierarchy();
    void drawInspector();

    engine::ecs::World world_;
    engine::InputManager input_;
    engine::GPUContext gpu_;
    engine::SpriteBatch batch_;

    engine::ecs::Entity selectedEntity_;
    bool initialized_ = false;
};

} // namespace editor
