#pragma once

#include "engine/ecs/Scene.hpp"
#include <string>
#include <string_view>

namespace engine::ecs {

class SceneSerializer {
public:
    explicit SceneSerializer(Scene& scene);

    [[nodiscard]] std::string serializeToString() const;
    bool serializeToFile(std::string_view filePath) const;

    bool deserializeFromString(std::string_view jsonContent);
    bool deserializeFromFile(std::string_view filePath);

private:
    Scene& m_scene;
};

} // namespace engine::ecs
