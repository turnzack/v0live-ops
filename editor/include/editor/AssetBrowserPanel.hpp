#pragma once

#include <imgui.h>
#include <filesystem>
#include "engine/assets/AssetDatabase.hpp"
#include "engine/ecs/World.hpp"

namespace editor {

class AssetBrowserPanel {
public:
    void draw(
        engine::AssetDatabase& database,
        engine::World& world,
        engine::Entity& selectedEntity
    );

private:
    engine::AssetGuid selectedAsset_;
};

} // namespace editor
