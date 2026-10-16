#include <engine/core/Application.hpp>

int main(int argc, char* argv[]) {
    const std::string configPath = argc > 1 ? argv[1] : "config/engine.json";
    engine::Application app(configPath);
    return app.run();
}
