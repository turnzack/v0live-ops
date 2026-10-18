#include "engine/core/Application.hpp"
#include "engine/core/Log.hpp"

namespace engine::core {

namespace {
engine::WindowConfig toEngineWindowConfig(const engine::core::WindowConfig& cfg) {
    engine::WindowConfig out;
    out.title = cfg.title;
    out.width = static_cast<int>(cfg.width);
    out.height = static_cast<int>(cfg.height);
    out.resizable = cfg.resizable;
    out.vsync = cfg.vsync;
    return out;
}
}

Application::Application(std::string_view configPath)
    : m_config(EngineConfig::loadFromFile(configPath))
    , m_window(toEngineWindowConfig(m_config.window))
    , m_time(m_config.simulation.fixedTimestepHz, m_config.simulation.maxAccumulatedSeconds) {
    Logger::init("engine.log");
    LOG_INFO(Engine, "Demarrage de l'Application: ", m_config.window.title);
}

Application::~Application() {
    LOG_INFO(Engine, "Fermeture de l'Application");
    Logger::shutdown();
}

void Application::stop() noexcept {
    m_running = false;
    m_window.requestClose();
}

int Application::run() {
    if (!m_window.handle()) {
        LOG_FATAL(Engine, "Echec d'initialisation de la fenetre");
        return 1;
    }

    if (!onInit()) {
        LOG_FATAL(Engine, "Echec de onInit()");
        return 1;
    }

    m_running = true;
    m_time.reset();
    m_fpsLogTimer = 0.0f;

    LOG_INFO(Engine, "Boucle principale demarree avec fixed timestep = ", m_config.getFixedDeltaTime() * 1000.0f, " ms (", m_config.simulation.fixedTimestepHz, " Hz)");

    while (m_running && !m_window.shouldClose()) {
        m_window.pollEvents();

        m_time.tick();
        const float dt = m_time.getDeltaTime();

        // 1. Simulation physique & gameplay a pas fixe (déterministe)
        while (m_time.shouldDoFixedStep()) {
            onFixedUpdate(m_time.getFixedDeltaTime());
        }

        // 2. Mise a jour logique variable
        onUpdate(dt);

        // 3. Rendu interpole (avec alpha pour lissage visuel)
        if (!m_window.isMinimized()) {
            onRender(m_time.getAlpha());
        }

        // 4. Logging regulier des FPS et de la sante de la boucle
        m_fpsLogTimer += dt;
        if (m_fpsLogTimer >= 2.0f) {
            LOG_INFO(Engine, "Performance: ", m_time.getFPS(), " FPS | FrameTime: ", dt * 1000.0f, " ms | Total Frames: ", m_time.getFrameCount());
            m_fpsLogTimer = 0.0f;
        }
    }

    onShutdown();
    LOG_INFO(Engine, "Boucle principale terminee proprement");
    return 0;
}

} // namespace engine::core
