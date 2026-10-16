#include "engine/ecs/SceneSerializer.hpp"
#include "engine/core/Log.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace engine::ecs {

namespace {

std::string extractJsonString(std::string_view json, std::string_view key, std::string_view defaultVal = "") {
    auto pos = json.find("\"" + std::string(key) + "\"");
    if (pos == std::string_view::npos) return std::string(defaultVal);
    pos = json.find(':', pos);
    if (pos == std::string_view::npos) return std::string(defaultVal);
    auto startQuote = json.find('\"', pos);
    if (startQuote == std::string_view::npos) return std::string(defaultVal);
    auto endQuote = json.find('\"', startQuote + 1);
    if (endQuote == std::string_view::npos) return std::string(defaultVal);
    return std::string(json.substr(startQuote + 1, endQuote - startQuote - 1));
}

float extractJsonFloat(std::string_view json, std::string_view key, float defaultVal = 0.0f) {
    auto pos = json.find("\"" + std::string(key) + "\"");
    if (pos == std::string_view::npos) return defaultVal;
    pos = json.find(':', pos);
    if (pos == std::string_view::npos) return defaultVal;
    pos = json.find_first_of("-0123456789.", pos);
    if (pos == std::string_view::npos) return defaultVal;
    auto endPos = json.find_first_not_of("-0123456789.eE", pos);
    std::string numStr = std::string(json.substr(pos, (endPos == std::string_view::npos) ? std::string_view::npos : (endPos - pos)));
    try {
        return std::stof(numStr);
    } catch (...) {
        return defaultVal;
    }
}

int extractJsonInt(std::string_view json, std::string_view key, int defaultVal = 0) {
    return static_cast<int>(extractJsonFloat(json, key, static_cast<float>(defaultVal)));
}

bool extractJsonBool(std::string_view json, std::string_view key, bool defaultVal = false) {
    auto pos = json.find("\"" + std::string(key) + "\"");
    if (pos == std::string_view::npos) return defaultVal;
    pos = json.find(':', pos);
    if (pos == std::string_view::npos) return defaultVal;
    auto valPart = json.substr(pos, 15);
    if (valPart.find("true") != std::string_view::npos) return true;
    if (valPart.find("false") != std::string_view::npos) return false;
    return defaultVal;
}

core::Vec3 extractVec3(std::string_view block, std::string_view key) {
    auto pos = block.find("\"" + std::string(key) + "\"");
    if (pos == std::string_view::npos) return core::Vec3::Zero;
    auto endBlock = block.find('}', pos);
    std::string_view sub = block.substr(pos, endBlock - pos + 1);
    return {extractJsonFloat(sub, "x"), extractJsonFloat(sub, "y"), extractJsonFloat(sub, "z")};
}

core::Vec2 extractVec2(std::string_view block, std::string_view key) {
    auto pos = block.find("\"" + std::string(key) + "\"");
    if (pos == std::string_view::npos) return core::Vec2::Zero;
    auto endBlock = block.find('}', pos);
    std::string_view sub = block.substr(pos, endBlock - pos + 1);
    return {extractJsonFloat(sub, "x"), extractJsonFloat(sub, "y")};
}

core::Vec4 extractVec4(std::string_view block, std::string_view key) {
    auto pos = block.find("\"" + std::string(key) + "\"");
    if (pos == std::string_view::npos) return core::Vec4(1.0f);
    auto endBlock = block.find('}', pos);
    std::string_view sub = block.substr(pos, endBlock - pos + 1);
    return {extractJsonFloat(sub, "x"), extractJsonFloat(sub, "y"), extractJsonFloat(sub, "z"), extractJsonFloat(sub, "w", 1.0f)};
}

} // namespace

SceneSerializer::SceneSerializer(Scene& scene) : m_scene(scene) {}

std::string SceneSerializer::serializeToString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4);

    oss << "{\n"
        << "  \"version\": 1,\n"
        << "  \"sceneName\": \"" << m_scene.getName() << "\",\n"
        << "  \"sceneGuid\": \"" << m_scene.getGUID().asString() << "\",\n"
        << "  \"entities\": [\n";

    const auto& entities = m_scene.getWorld().getAllEntities();
    for (size_t i = 0; i < entities.size(); ++i) {
        EntityId id = entities[i];
        if (!m_scene.getWorld().isAlive(id)) continue;

        Entity ent = const_cast<Scene&>(m_scene).getWorld().getEntity(id);

        oss << "    {\n"
            << "      \"id\": \"" << ent.getGUID().asString() << "\",\n"
            << "      \"tag\": \"" << ent.getName() << "\"";

        if (const auto* t = ent.getComponent<TransformComponent>()) {
            oss << ",\n      \"transform\": {\n"
                << "        \"position\": {\"x\": " << t->position.x << ", \"y\": " << t->position.y << ", \"z\": " << t->position.z << "},\n"
                << "        \"rotation\": {\"x\": " << t->rotation.x << ", \"y\": " << t->rotation.y << ", \"z\": " << t->rotation.z << "},\n"
                << "        \"scale\": {\"x\": " << t->scale.x << ", \"y\": " << t->scale.y << ", \"z\": " << t->scale.z << "}\n"
                << "      }";
        }

        if (const auto* s = ent.getComponent<SpriteComponent>()) {
            oss << ",\n      \"sprite\": {\n"
                << "        \"textureGuid\": \"" << s->textureGuid.asString() << "\",\n"
                << "        \"size\": {\"x\": " << s->size.x << ", \"y\": " << s->size.y << "},\n"
                << "        \"colorTint\": {\"x\": " << s->colorTint.x << ", \"y\": " << s->colorTint.y << ", \"z\": " << s->colorTint.z << ", \"w\": " << s->colorTint.w << "},\n"
                << "        \"sortingLayer\": " << s->sortingLayer << ",\n"
                << "        \"flipX\": " << (s->flipX ? "true" : "false") << ",\n"
                << "        \"flipY\": " << (s->flipY ? "true" : "false") << "\n"
                << "      }";
        }

        if (const auto* m = ent.getComponent<MovementComponent>()) {
            oss << ",\n      \"movement\": {\n"
                << "        \"speed\": " << m->speed << ",\n"
                << "        \"velocity\": {\"x\": " << m->velocity.x << ", \"y\": " << m->velocity.y << "}\n"
                << "      }";
        }

        oss << "\n    }" << (i + 1 < entities.size() ? "," : "") << "\n";
    }

    oss << "  ]\n"
        << "}\n";

    return oss.str();
}

bool SceneSerializer::serializeToFile(std::string_view filePath) const {
    std::ofstream file(std::string(filePath));
    if (!file.is_open()) {
        LOG_ERROR(Scene, "Impossible de sauvegarder la scene vers: ", filePath);
        return false;
    }
    file << serializeToString();
    LOG_INFO(Scene, "Scene sauvegardee avec succes: ", filePath);
    return true;
}

bool SceneSerializer::deserializeFromString(std::string_view jsonContent) {
    if (jsonContent.empty()) return false;

    int version = extractJsonInt(jsonContent, "version", 0);
    if (version != 1) {
        LOG_ERROR(Scene, "Version de schema de scene incompatible ou absente: ", version);
        return false;
    }

    std::string sceneName = extractJsonString(jsonContent, "sceneName", "Loaded Scene");
    std::string sceneGuidStr = extractJsonString(jsonContent, "sceneGuid", "");
    core::GUID sceneGuid(sceneGuidStr);

    m_scene.clear();
    m_scene.setName(sceneName);
    if (sceneGuid.isValid()) {
        m_scene.setGUID(sceneGuid);
    }

    // Extraction des entités
    size_t entPos = jsonContent.find("\"entities\": [");
    if (entPos == std::string_view::npos) return true;

    size_t cursor = entPos;
    while (true) {
        size_t objStart = jsonContent.find('{', cursor);
        if (objStart == std::string_view::npos) break;

        // Trouver la fermeture d'objet d'entité équilibrée
        int depth = 0;
        size_t objEnd = std::string_view::npos;
        for (size_t i = objStart; i < jsonContent.length(); ++i) {
            if (jsonContent[i] == '{') depth++;
            else if (jsonContent[i] == '}') {
                depth--;
                if (depth == 0) {
                    objEnd = i;
                    break;
                }
            }
        }
        if (objEnd == std::string_view::npos) break;

        std::string_view entBlock = jsonContent.substr(objStart, objEnd - objStart + 1);

        std::string idStr = extractJsonString(entBlock, "id");
        std::string tagStr = extractJsonString(entBlock, "tag", "Entity");

        core::GUID guid(idStr);
        Entity ent = m_scene.createEntity(tagStr, guid);

        // Transform
        if (entBlock.find("\"transform\"") != std::string_view::npos) {
            auto* t = ent.getComponent<TransformComponent>();
            if (t) {
                t->position = extractVec3(entBlock, "position");
                t->rotation = extractVec3(entBlock, "rotation");
                t->scale    = extractVec3(entBlock, "scale");
            }
        }

        // Sprite
        if (entBlock.find("\"sprite\"") != std::string_view::npos) {
            core::GUID texGuid(extractJsonString(entBlock, "textureGuid"));
            core::Vec2 sz = extractVec2(entBlock, "size");
            core::Vec4 col = extractVec4(entBlock, "colorTint");
            int layer = extractJsonInt(entBlock, "sortingLayer", 0);
            bool fx = extractJsonBool(entBlock, "flipX", false);
            bool fy = extractJsonBool(entBlock, "flipY", false);

            auto& s = ent.addComponent<SpriteComponent>(sz, col);
            s.textureGuid = texGuid;
            s.sortingLayer = layer;
            s.flipX = fx;
            s.flipY = fy;
        }

        // Movement
        if (entBlock.find("\"movement\"") != std::string_view::npos) {
            float speed = extractJsonFloat(entBlock, "speed", 200.0f);
            core::Vec2 vel = extractVec2(entBlock, "velocity");
            auto& m = ent.addComponent<MovementComponent>(speed);
            m.velocity = vel;
        }

        cursor = objEnd + 1;
        // Vérifier si fin de liste
        size_t nextBracket = jsonContent.find(']', cursor);
        size_t nextOpen = jsonContent.find('{', cursor);
        if (nextBracket < nextOpen || nextOpen == std::string_view::npos) {
            break;
        }
    }

    LOG_INFO(Scene, "Scene chargee avec succes: ", m_scene.getName(), " (", m_scene.getWorld().entityCount(), " entites)");
    return true;
}

bool SceneSerializer::deserializeFromFile(std::string_view filePath) {
    std::ifstream file(std::string(filePath));
    if (!file.is_open()) {
        LOG_ERROR(Scene, "Impossible d'ouvrir le fichier de scene: ", filePath);
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return deserializeFromString(buffer.str());
}

} // namespace engine::ecs
