#include "editor/EditorApp.hpp"
#include "editor/commands/TransformCommand.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_wgpu.h>
#include <spdlog/spdlog.h>
#include <cstdio>

namespace editor {

bool EditorApp::initialize(SDL_Window* window) {
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    if (!ImGui_ImplSDL3_InitForOther(window)) {
        spdlog::error("Failed to initialize ImGui SDL3 backend.");
        return false;
    }

    if (!gpu_.initialize(window)) {
        return false;
    }

    ImGui_ImplWGPU_InitInfo initInfo{};
    initInfo.Device = gpu_.device();
    initInfo.NumFramesInFlight = 3;
    initInfo.RenderTargetFormat = gpu_.surfaceFormat();

    if (!ImGui_ImplWGPU_Init(&initInfo)) {
        spdlog::error("Failed to initialize ImGui WebGPU backend.");
        return false;
    }

    input_.loadBindings("config/input.json");

    audio_.initialize();

    world_.createEntity("Player");
    world_.createEntity("Ground");

    selectedEntity_ = world_.getEntityByName("Player");

    lua_.initialize(world_, input_, audio_);
    lua_.loadScript("samples/platformer/scripts/player.lua");

    initialized_ = true;

    return true;
}

void EditorApp::shutdown() {
    if (!initialized_) {
        return;
    }

    lua_.shutdown();
    audio_.shutdown();

    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    gpu_.shutdown();
    initialized_ = false;
}

void EditorApp::handleEvent(const SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
    input_.handleEvent(event);

    if (event.type == SDL_EVENT_KEY_DOWN) {
        const bool ctrl = (event.key.mod & SDL_KMOD_CTRL) != 0;
        const bool shift = (event.key.mod & SDL_KMOD_SHIFT) != 0;

        if (ctrl && event.key.key == SDLK_Z) {
            undo_.undo();
        } else if (ctrl && event.key.key == SDLK_Y) {
            undo_.redo();
        } else if (ctrl && event.key.key == SDLK_P) {
            if (shift) {
                exitPlayMode();
            } else {
                enterPlayMode();
            }
        }
    }
}

void EditorApp::enterPlayMode() {
    if (mode_ == EditorMode::Play) {
        return;
    }

    editWorld_ = world_.clone();
    playWorld_ = world_.clone();
    mode_ = EditorMode::Play;

    lua_.initialize(*playWorld_, input_, audio_);
    lua_.reload();

    selectedEntity_ = playWorld_->getEntityByName("Player");

    spdlog::info("Play Mode started.");
}

void EditorApp::exitPlayMode() {
    if (mode_ != EditorMode::Play) {
        return;
    }

    world_.restoreFrom(editWorld_);
    playWorld_.reset();
    mode_ = EditorMode::Edit;

    lua_.initialize(world_, input_, audio_);
    lua_.reload();

    selectedEntity_ = world_.getEntityByName("Player");

    spdlog::info("Play Mode stopped. Edit scene restored.");
}

void EditorApp::newFrame(double delta) {
    if (mode_ == EditorMode::Play) {
        lua_.callOnUpdate(delta);
    }

    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void EditorApp::render() {
    drawDockspace();
    drawHierarchy();
    drawInspector();

    ImGui::Render();
}

void EditorApp::drawDockspace() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("DockSpace", nullptr,
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus
    );

    ImGui::PopStyleVar(2);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
                activeWorld().saveScene("samples/platformer/scenes/level_01.scene.json");
            }

            if (ImGui::MenuItem("Load Scene", "Ctrl+O")) {
                activeWorld().loadScene("samples/platformer/scenes/level_01.scene.json");
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, undo_.canUndo())) {
                undo_.undo();
            }

            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, undo_.canRedo())) {
                undo_.redo();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Play Mode")) {
            if (ImGui::MenuItem("Enter Play Mode", "Ctrl+P", mode_ == EditorMode::Play, mode_ != EditorMode::Play)) {
                enterPlayMode();
            }

            if (ImGui::MenuItem("Stop Play Mode", "Ctrl+Shift+P", false, mode_ == EditorMode::Play)) {
                exitPlayMode();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::DockSpace(
        ImGui::GetID("MainDockSpace"),
        ImVec2(0.0f, 0.0f),
        ImGuiDockNodeFlags_None
    );

    ImGui::End();
}

void EditorApp::drawHierarchy() {
    ImGui::Begin("Hierarchy");

    if (mode_ == EditorMode::Play) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "● PLAY MODE ACTIVE");
        ImGui::Separator();
    }

    activeWorld().forEach<engine::TagComponent>(
        [&](engine::ecs::EntityId entityId, engine::TagComponent& tag) {
            engine::Entity entity = activeWorld().getEntity(entityId);
            const bool selected = (entity == selectedEntity_);

            if (ImGui::Selectable(tag.name.c_str(), selected)) {
                selectedEntity_ = entity;
            }
        }
    );

    ImGui::End();
}

void EditorApp::drawInspector() {
    ImGui::Begin("Inspector");

    if (!activeWorld().isValid(selectedEntity_)) {
        ImGui::TextDisabled("No entity selected.");
        ImGui::End();
        return;
    }

    if (auto* tag = activeWorld().tryGet<engine::TagComponent>(selectedEntity_)) {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "%s", tag->name.c_str());

        if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
            tag->name = buffer;
            tag->tag = buffer;
        }
    }

    if (auto* transform = activeWorld().tryGet<engine::TransformComponent>(selectedEntity_)) {
        glm::vec3 pos{transform->position.x, transform->position.y, transform->position.z};
        if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
            glm::vec3 oldPos{transform->position.x, transform->position.y, transform->position.z};
            transform->position = {pos.x, pos.y, pos.z};
            if (mode_ == EditorMode::Edit) {
                undo_.execute(std::make_unique<TransformCommand>(activeWorld(), selectedEntity_, oldPos, pos));
            }
        }

        ImGui::DragFloat3("Rotation", &transform->rotation.x, 0.1f);
        ImGui::DragFloat3("Scale", &transform->scale.x, 0.1f);
    }

    if (auto* movement = activeWorld().tryGet<engine::MovementComponent>(selectedEntity_)) {
        ImGui::DragFloat("Speed", &movement->speed, 1.0f, 0.0f, 2000.0f);
    }

    ImGui::Separator();
    ImGui::Text("Scripting & Audio");

    if (ImGui::Button("Reload Lua")) {
        lua_.reload();
    }

    ImGui::SameLine();

    if (ImGui::Button("Play Test Sound")) {
        audio_.playOneShot("assets/audio/pickup.wav", engine::AudioBus::Sfx);
    }

    ImGui::End();
}

bool EditorApp::wantCaptureInput() const {
    return ImGui::GetIO().WantCaptureMouse ||
           ImGui::GetIO().WantCaptureKeyboard;
}

} // namespace editor
