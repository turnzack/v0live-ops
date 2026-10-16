#include "editor/AssetBrowserPanel.hpp"
#include "editor/commands/CreateSpriteCommand.hpp"

namespace editor {

void AssetBrowserPanel::draw(
    engine::AssetDatabase& database,
    engine::World& world,
    engine::Entity& selectedEntity
) {
    (void)world;
    (void)selectedEntity;

    ImGui::Begin("Asset Browser");

    if (ImGui::Button("Import Asset")) {
        const engine::AssetGuid guid = database.importAsset("assets/textures/player.png");
        if (!guid.empty()) {
            selectedAsset_ = guid;
        }
    }

    ImGui::Separator();

    for (const auto& [guid, record] : database.assets()) {
        const bool selected = (guid == selectedAsset_);
        ImGui::PushID(guid.c_str());

        if (ImGui::Selectable(record.sourcePath.filename().string().c_str(), selected)) {
            selectedAsset_ = guid;
        }

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload(
                "ENGINE_ASSET_GUID",
                guid.data(),
                guid.size() + 1
            );
            ImGui::Text("Asset: %s", record.sourcePath.filename().string().c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::PopID();
    }

    ImGui::End();
}

} // namespace editor
