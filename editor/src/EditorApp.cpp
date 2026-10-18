#include "editor/EditorApp.hpp"
#include "editor/commands/TransformCommand.hpp"
#include "editor/commands/CreateSpriteCommand.hpp"
#include "editor/commands/ReparentEntityCommand.hpp"
#include "engine/ecs/Components.hpp"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_wgpu.h>
#include <spdlog/spdlog.h>
#include <cstdio>
#include <algorithm>

namespace editor {

bool EditorApp::initialize(SDL_Window* window) {
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
#ifdef ImGuiConfigFlags_DockingEnable
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif
#ifdef ImGuiConfigFlags_ViewportsEnable
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif

    if (!ImGui_ImplSDL3_InitForOther(window)) {
        spdlog::error("Failed to initialize ImGui SDL3 backend.");
        return false;
    }

    if (!gpu_.initialize(window)) {
        return false;
    }

    ImGui_ImplWGPU_InitInfo initInfo{};
    initInfo.Device = gpu_.device().Get();
    initInfo.NumFramesInFlight = 3;
    initInfo.RenderTargetFormat = static_cast<WGPUTextureFormat>(gpu_.surfaceFormat());

    if (!ImGui_ImplWGPU_Init(&initInfo)) {
        spdlog::error("Failed to initialize ImGui WebGPU backend.");
        return false;
    }

    input_.loadBindings("config/input.json");

    audio_.initialize();

    if (!world_.loadScene("samples/platformer/scenes/level_01.scene.json")) {
        auto player = world_.createEntity("Player");
        auto& pTransform = world_.add<engine::TransformComponent>(player);
        pTransform.position = {0.0f, 3.0f, 0.0f};
        world_.add<engine::SpriteComponent>(player, engine::core::Vec2{48.0f, 64.0f});
        auto& pRb = world_.add<engine::RigidBody2DComponent>(player);
        pRb.type = engine::RigidBody2DType::Dynamic;
        pRb.fixedRotation = true;
        auto& pCol = world_.add<engine::Collider2DComponent>(player);
        pCol.type = engine::Collider2DType::Capsule;
        pCol.size = {0.8f, 1.6f};
        pCol.radius = 0.4f;
        auto& pCc = world_.add<engine::CharacterController2DComponent>(player);
        pCc.maxSpeed = 350.0f;
        pCc.jumpVelocity = 750.0f;

        auto ground = world_.createEntity("Ground");
        auto& gTransform = world_.add<engine::TransformComponent>(ground);
        gTransform.position = {0.0f, -2.0f, 0.0f};
        world_.add<engine::SpriteComponent>(ground, engine::core::Vec2{2000.0f, 100.0f});
        auto& gRb = world_.add<engine::RigidBody2DComponent>(ground);
        gRb.type = engine::RigidBody2DType::Static;
        auto& gCol = world_.add<engine::Collider2DComponent>(ground);
        gCol.type = engine::Collider2DType::Box;
        gCol.size = {2000.0f, 100.0f};
    }

    selectedEntity_ = world_.getEntityByName("Player");

    lua_.initialize(world_, input_, audio_);
    lua_.loadScript("samples/platformer/scripts/player.lua");

    assetDb_.initialize(".");
    if (std::filesystem::exists("assets/textures/player.png")) {
        assetDb_.importAsset("assets/textures/player.png");
    }
    if (std::filesystem::exists("samples/platformer/scripts/player.lua")) {
        assetDb_.importAsset("samples/platformer/scripts/player.lua");
    }
    assetWatcher_.start(assetDb_, [this](const std::filesystem::path& path) {
        if (path.extension() == ".lua") {
            lua_.reload();
        }
    });

    camera_.setZoom(50.0f); // 50 pixels per meter/unit
    camera_.setDeadZone(16.0f);

    initialized_ = true;

    return true;
}

void EditorApp::shutdown() {
    if (!initialized_) {
        return;
    }

    assetWatcher_.stop();
    assetDb_.shutdown();

    physics_.shutdown();
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

        if (event.key.key == SDLK_F1) {
            showColliders_ = !showColliders_;
        } else if (ctrl && event.key.key == SDLK_Z) {
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

    physics_.initialize(glm::vec2(0.0f, -9.81f));
    physics_.syncFromEcs(*playWorld_);

    lua_.initialize(*playWorld_, input_, audio_);
    lua_.reload();

    selectedEntity_ = playWorld_->getEntityByName("Player");

    spdlog::info("Play Mode started with Box2D physics simulation.");
}

void EditorApp::exitPlayMode() {
    if (mode_ != EditorMode::Play) {
        return;
    }

    physics_.shutdown();

    world_.restoreFrom(editWorld_);
    playWorld_.reset();
    mode_ = EditorMode::Edit;

    lua_.initialize(world_, input_, audio_);
    lua_.reload();

    selectedEntity_ = world_.getEntityByName("Player");

    spdlog::info("Play Mode stopped. Physics shutdown, Edit scene restored.");
}

void EditorApp::newFrame(double delta) {
    assetWatcher_.processPendingChanges();

    if (mode_ == EditorMode::Play && playWorld_.has_value()) {
        characterController_.update(*playWorld_, input_, physics_, static_cast<float>(delta));
        physics_.step(static_cast<float>(delta), 4);
        physics_.syncToEcs(*playWorld_);

        auto player = playWorld_->getEntityByName("Player");
        if (player.isValid()) {
            if (const auto* transform = playWorld_->tryGet<engine::TransformComponent>(player)) {
                camera_.follow(glm::vec2(transform->position.x, transform->position.y), static_cast<float>(delta));
            }
        }

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
    drawDebugColliders();
    assetBrowserPanel_.draw(assetDb_, activeWorld(), selectedEntity_);
    drawLuaConsole();

    ImGui::Render();
}

void EditorApp::drawDockspace() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
#if defined(IMGUI_HAS_VIEWPORT) || defined(ImGuiConfigFlags_ViewportsEnable)
    ImGui::SetNextWindowViewport(viewport->ID);
#endif

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;

#ifdef ImGuiWindowFlags_NoDocking
    windowFlags |= ImGuiWindowFlags_NoDocking;
#endif

    ImGui::Begin("DockSpace", nullptr, windowFlags);

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

#if defined(IMGUI_HAS_DOCK) || defined(ImGuiDockNodeFlags_None)
    ImGui::DockSpace(
        ImGui::GetID("MainDockSpace"),
        ImVec2(0.0f, 0.0f),
        ImGuiDockNodeFlags_None
    );
#endif

    ImGui::End();
}

void EditorApp::drawHierarchy() {
    ImGui::Begin("Hierarchy");

    if (mode_ == EditorMode::Play) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "● PLAY MODE ACTIVE");
        ImGui::Separator();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENGINE_ASSET_GUID")) {
            const std::string guid(
                static_cast<const char*>(payload->Data),
                payload->DataSize - 1
            );
            const engine::AssetRecord* record = assetDb_.find(guid);
            if (record != nullptr && record->type == engine::AssetType::Texture) {
                undo_.execute(std::make_unique<CreateSpriteCommand>(
                    activeWorld(), record->guid, glm::vec2(0.0f, 0.0f)
                ));
            }
        }
        ImGui::EndDragDropTarget();
    }

    const auto rootIds = activeWorld().getRootEntities();
    for (engine::ecs::EntityId rootId : rootIds) {
        engine::Entity root = activeWorld().getEntity(rootId);
        drawHierarchyNode(root);
    }

    ImGui::End();
}

void EditorApp::drawHierarchyNode(engine::Entity entity) {
    if (!entity.isValid()) {
        return;
    }

    auto* tag = activeWorld().tryGet<engine::TagComponent>(entity);
    std::string name = tag ? tag->name : ("Entity " + std::to_string(entity.id()));

    ImGui::PushID(static_cast<int>(entity.id()));
    const bool selected = (entity == selectedEntity_);
    const auto children = activeWorld().children(entity);
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
                               (selected ? ImGuiTreeNodeFlags_Selected : 0);
    if (children.empty()) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    const bool opened = ImGui::TreeNodeEx(name.c_str(), flags);

    if (ImGui::IsItemClicked()) {
        selectedEntity_ = entity;
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("ENGINE_ENTITY", &entity, sizeof(entity));
        ImGui::Text("%s", name.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENGINE_ENTITY")) {
            engine::Entity draggedEntity;
            std::memcpy(&draggedEntity, payload->Data, sizeof(draggedEntity));
            if (draggedEntity.isValid() && draggedEntity != entity) {
                undo_.execute(std::make_unique<ReparentEntityCommand>(
                    activeWorld(), draggedEntity, entity
                ));
            }
        }
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENGINE_ASSET_GUID")) {
            const std::string guid(
                static_cast<const char*>(payload->Data),
                payload->DataSize - 1
            );
            const engine::AssetRecord* record = assetDb_.find(guid);
            if (record != nullptr && record->type == engine::AssetType::Texture) {
                undo_.execute(std::make_unique<CreateSpriteCommand>(
                    activeWorld(), record->guid, glm::vec2(0.0f, 0.0f)
                ));
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (opened) {
        for (const engine::Entity child : children) {
            drawHierarchyNode(child);
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
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
    ImGui::Text("Physics 2D (Box2D v3)");
    ImGui::Checkbox("Show Colliders (F1)", &showColliders_);

    if (auto* rb = activeWorld().tryGet<engine::RigidBody2DComponent>(selectedEntity_)) {
        const char* types[] = { "Static", "Kinematic", "Dynamic" };
        int currentType = static_cast<int>(rb->type);
        if (ImGui::Combo("Body Type", &currentType, types, 3)) {
            rb->type = static_cast<engine::RigidBody2DType>(currentType);
        }
        ImGui::DragFloat("Gravity Scale", &rb->gravityScale, 0.1f, -10.0f, 10.0f);
        ImGui::DragFloat("Linear Damping", &rb->linearDamping, 0.05f, 0.0f, 50.0f);
        ImGui::Checkbox("Fixed Rotation", &rb->fixedRotation);
        ImGui::Checkbox("Is Sensor", &rb->isSensor);
    } else {
        if (ImGui::Button("Add RigidBody2D")) {
            activeWorld().add<engine::RigidBody2DComponent>(selectedEntity_);
        }
    }

    if (auto* col = activeWorld().tryGet<engine::Collider2DComponent>(selectedEntity_)) {
        const char* colTypes[] = { "Box", "Circle", "Capsule" };
        int currentCol = static_cast<int>(col->type);
        if (ImGui::Combo("Collider Type", &currentCol, colTypes, 3)) {
            col->type = static_cast<engine::Collider2DType>(currentCol);
        }
        ImGui::DragFloat2("Collider Size", &col->size.x, 0.1f, 0.01f, 10000.0f);
        ImGui::DragFloat("Radius", &col->radius, 0.05f, 0.01f, 500.0f);
        ImGui::DragFloat("Density", &col->density, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat("Friction", &col->friction, 0.05f, 0.0f, 1.0f);
        ImGui::DragFloat("Restitution", &col->restitution, 0.05f, 0.0f, 1.0f);
        ImGui::Checkbox("Is Trigger", &col->isTrigger);
    } else {
        if (ImGui::Button("Add Collider2D")) {
            activeWorld().add<engine::Collider2DComponent>(selectedEntity_);
        }
    }

    if (auto* cc = activeWorld().tryGet<engine::CharacterController2DComponent>(selectedEntity_)) {
        ImGui::Separator();
        ImGui::Text("Character Controller 2D");
        ImGui::DragFloat("Max Speed", &cc->maxSpeed, 5.0f, 0.0f, 2000.0f);
        ImGui::DragFloat("Acceleration", &cc->acceleration, 50.0f, 0.0f, 20000.0f);
        ImGui::DragFloat("Jump Velocity", &cc->jumpVelocity, 10.0f, 0.0f, 3000.0f);
        ImGui::DragFloat("Coyote Time", &cc->coyoteTime, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Jump Buffer", &cc->jumpBufferTime, 0.01f, 0.0f, 1.0f);
        ImGui::Text("Grounded: %s", cc->grounded ? "TRUE" : "FALSE");
    } else {
        if (ImGui::Button("Add Character Controller")) {
            activeWorld().add<engine::CharacterController2DComponent>(selectedEntity_);
        }
    }

    ImGui::Separator();
    ImGui::Text("Camera 2D");
    float zoom = camera_.zoom();
    if (ImGui::SliderFloat("Camera Zoom", &zoom, 5.0f, 200.0f)) {
        camera_.setZoom(zoom);
    }
    ImGui::Text("Camera Pos: (%.1f, %.1f)", camera_.position().x, camera_.position().y);

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

void EditorApp::drawDebugColliders() {
    if (!showColliders_) {
        return;
    }

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    const ImVec2 screenCenter(vp->WorkPos.x + vp->WorkSize.x * 0.5f, vp->WorkPos.y + vp->WorkSize.y * 0.5f);
    const float zoom = camera_.zoom();

    auto toScreen = [&](glm::vec2 worldPos) -> ImVec2 {
        glm::vec2 rel = (worldPos - camera_.position()) * zoom;
        return ImVec2(screenCenter.x + rel.x, screenCenter.y - rel.y);
    };

    activeWorld().forEach<engine::TransformComponent, engine::Collider2DComponent>(
        [&](engine::Entity entity, engine::TransformComponent& transform, engine::Collider2DComponent& collider) {
            const glm::vec2 center{transform.position.x, transform.position.y};

            ImU32 color = IM_COL32(0, 220, 255, 220);
            if (collider.isTrigger) {
                color = IM_COL32(255, 255, 0, 220);
            } else if (auto* cc = activeWorld().tryGet<engine::CharacterController2DComponent>(entity)) {
                color = cc->grounded ? IM_COL32(0, 255, 0, 240) : IM_COL32(255, 60, 60, 240);
            } else if (auto* rb = activeWorld().tryGet<engine::RigidBody2DComponent>(entity)) {
                if (rb->type == engine::RigidBody2DType::Dynamic) {
                    color = IM_COL32(255, 80, 80, 220);
                }
            }

            switch (collider.type) {
                case engine::Collider2DType::Circle: {
                    ImVec2 p = toScreen(center);
                    drawList->AddCircle(p, collider.radius * zoom, color, 32, 2.0f);
                    break;
                }
                case engine::Collider2DType::Capsule: {
                    ImVec2 p1 = toScreen(glm::vec2(center.x, center.y - collider.size.y * 0.5f));
                    ImVec2 p2 = toScreen(glm::vec2(center.x, center.y + collider.size.y * 0.5f));
                    float r = collider.radius * zoom;
                    drawList->AddLine(ImVec2(p1.x - r, p1.y), ImVec2(p2.x - r, p2.y), color, 2.0f);
                    drawList->AddLine(ImVec2(p1.x + r, p1.y), ImVec2(p2.x + r, p2.y), color, 2.0f);
                    drawList->AddCircle(p1, r, color, 16, 2.0f);
                    drawList->AddCircle(p2, r, color, 16, 2.0f);
                    break;
                }
                default: {
                    ImVec2 minP = toScreen(glm::vec2(center.x - collider.size.x * 0.5f, center.y + collider.size.y * 0.5f));
                    ImVec2 maxP = toScreen(glm::vec2(center.x + collider.size.x * 0.5f, center.y - collider.size.y * 0.5f));
                    drawList->AddRect(minP, maxP, color, 0.0f, 0, 2.0f);
                    break;
                }
            }
        }
    );
}

void EditorApp::drawLuaConsole() {
    ImGui::Begin("Lua Console");
    if (lua_.isValid()) {
        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Lua runtime: OK");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Lua runtime: ERROR");
    }
    if (!lua_.lastError().empty()) {
        ImGui::Separator();
        ImGui::TextWrapped("%s", lua_.lastError().c_str());
    }
    if (ImGui::Button("Reload Script")) {
        lua_.reload();
    }
    ImGui::End();
}

bool EditorApp::wantCaptureInput() const {
    return ImGui::GetIO().WantCaptureMouse ||
           ImGui::GetIO().WantCaptureKeyboard;
}

} // namespace editor
