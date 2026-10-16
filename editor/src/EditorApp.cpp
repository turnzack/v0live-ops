#include "editor/EditorApp.hpp"

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

    world_.createEntity("Player");
    world_.createEntity("Ground");

    selectedEntity_ = world_.getEntityByName("Player");

    initialized_ = true;

    return true;
}

void EditorApp::shutdown() {
    if (!initialized_) {
        return;
    }

    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    gpu_.shutdown();
    initialized_ = false;
}

void EditorApp::handleEvent(const SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
    input_.handleEvent(event);
}

void EditorApp::newFrame() {
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
                world_.saveScene("samples/platformer/scenes/level_01.scene.json");
            }

            if (ImGui::MenuItem("Load Scene", "Ctrl+O")) {
                world_.loadScene("samples/platformer/scenes/level_01.scene.json");
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

    world_.forEach<engine::TagComponent>(
        [&](engine::ecs::EntityId entityId, engine::TagComponent& tag) {
            engine::Entity entity = world_.getEntity(entityId);
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

    if (!world_.isValid(selectedEntity_)) {
        ImGui::TextDisabled("No entity selected.");
        ImGui::End();
        return;
    }

    if (auto* tag = world_.tryGet<engine::TagComponent>(selectedEntity_)) {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "%s", tag->name.c_str());

        if (ImGui::InputText("Name", buffer, sizeof(buffer))) {
            tag->name = buffer;
            tag->tag = buffer;
        }
    }

    if (auto* transform = world_.tryGet<engine::TransformComponent>(selectedEntity_)) {
        ImGui::DragFloat3("Position", &transform->position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &transform->rotation.x, 0.1f);
        ImGui::DragFloat3("Scale", &transform->scale.x, 0.1f);
    }

    if (auto* movement = world_.tryGet<engine::MovementComponent>(selectedEntity_)) {
        ImGui::DragFloat("Speed", &movement->speed, 1.0f, 0.0f, 2000.0f);
    }

    ImGui::End();
}

bool EditorApp::wantCaptureInput() const {
    return ImGui::GetIO().WantCaptureMouse ||
           ImGui::GetIO().WantCaptureKeyboard;
}

} // namespace editor
