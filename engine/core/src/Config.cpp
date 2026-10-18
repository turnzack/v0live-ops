#include "engine/core/Config.hpp"
#include "engine/core/Log.hpp"
#include <fstream>
#include <sstream>

namespace engine::core {

namespace {

// Helper parser basique & résilient si nlohmann-json n'est pas lié statiquement
std::string extractJsonString(std::string_view json, std::string_view key, std::string_view defaultVal) {
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

double extractJsonNumber(std::string_view json, std::string_view key, double defaultVal) {
    auto pos = json.find("\"" + std::string(key) + "\"");
    if (pos == std::string_view::npos) return defaultVal;
    pos = json.find(':', pos);
    if (pos == std::string_view::npos) return defaultVal;
    pos = json.find_first_of("-0123456789.", pos);
    if (pos == std::string_view::npos) return defaultVal;
    auto endPos = json.find_first_not_of("-0123456789.eE", pos);
    std::string numStr = std::string(json.substr(pos, (endPos == std::string_view::npos) ? std::string_view::npos : (endPos - pos)));
    try {
        return std::stod(numStr);
    } catch (...) {
        return defaultVal;
    }
}

bool extractJsonBool(std::string_view json, std::string_view key, bool defaultVal) {
    auto pos = json.find("\"" + std::string(key) + "\"");
    if (pos == std::string_view::npos) return defaultVal;
    pos = json.find(':', pos);
    if (pos == std::string_view::npos) return defaultVal;
    auto valPart = json.substr(pos, 15);
    if (valPart.find("true") != std::string_view::npos) return true;
    if (valPart.find("false") != std::string_view::npos) return false;
    return defaultVal;
}

} // namespace

std::string EngineConfig::toJsonString() const {
    std::ostringstream oss;
    oss << "{\n"
        << "  \"window\": {\n"
        << "    \"title\": \"" << window.title << "\",\n"
        << "    \"width\": " << window.width << ",\n"
        << "    \"height\": " << window.height << ",\n"
        << "    \"vsync\": " << (window.vsync ? "true" : "false") << ",\n"
        << "    \"resizable\": " << (window.resizable ? "true" : "false") << ",\n"
        << "    \"fullscreen\": " << (window.fullscreen ? "true" : "false") << "\n"
        << "  },\n"
        << "  \"simulation\": {\n"
        << "    \"fixedTimestepHz\": " << simulation.fixedTimestepHz << ",\n"
        << "    \"maxAccumulatedSeconds\": " << simulation.maxAccumulatedSeconds << "\n"
        << "  }\n"
        << "}\n";
    return oss.str();
}

EngineConfig EngineConfig::fromJsonString(std::string_view jsonContent) {
    EngineConfig cfg{};
    cfg.window.title = extractJsonString(jsonContent, "title", cfg.window.title);
    cfg.window.width = static_cast<uint32_t>(extractJsonNumber(jsonContent, "width", cfg.window.width));
    cfg.window.height = static_cast<uint32_t>(extractJsonNumber(jsonContent, "height", cfg.window.height));
    cfg.window.vsync = extractJsonBool(jsonContent, "vsync", cfg.window.vsync);
    cfg.window.resizable = extractJsonBool(jsonContent, "resizable", cfg.window.resizable);
    cfg.window.fullscreen = extractJsonBool(jsonContent, "fullscreen", cfg.window.fullscreen);

    cfg.simulation.fixedTimestepHz = static_cast<float>(extractJsonNumber(jsonContent, "fixedTimestepHz", cfg.simulation.fixedTimestepHz));
    cfg.simulation.maxAccumulatedSeconds = static_cast<float>(extractJsonNumber(jsonContent, "maxAccumulatedSeconds", cfg.simulation.maxAccumulatedSeconds));

    return cfg;
}

EngineConfig EngineConfig::loadFromFile(std::string_view filePath) {
    std::string pathStr(filePath);
    std::ifstream file(pathStr);
    if (!file.is_open()) {
        LOG_WARNING(Config, "Fichier de configuration introuvable: '", filePath, "'. Utilisation des valeurs par defaut.");
        return EngineConfig{};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    LOG_INFO(Config, "Configuration chargee depuis '", filePath, "'");
    return fromJsonString(buffer.str());
}

bool EngineConfig::saveToFile(std::string_view filePath) const {
    std::string pathStr(filePath);
    std::ofstream file(pathStr);
    if (!file.is_open()) {
        LOG_ERROR(Config, "Impossible d'ecrire la configuration dans '", filePath, "'");
        return false;
    }
    file << toJsonString();
    return true;
}

} // namespace engine::core
