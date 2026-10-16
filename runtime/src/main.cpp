#include "engine/core/Application.hpp"
#include "engine/core/Log.hpp"

class SandboxApp : public engine::core::Application {
public:
    SandboxApp() : engine::core::Application("config.json") {}

protected:
    bool onInit() override {
        LOG_INFO(Game, "SandboxApp initialise avec succes.");
        return true;
    }

    void onFixedUpdate(float fixedDeltaTime) override {
        m_fixedTicks++;
        (void)fixedDeltaTime;
    }

    void onUpdate(float deltaTime) override {
        (void)deltaTime;
    }

    void onRender(float alpha) override {
        (void)alpha;
        // Point d'accroche pour le rendu WebGPU au lot suivant
    }

    void onShutdown() override {
        LOG_INFO(Game, "SandboxApp termine. Total fixed ticks: ", m_fixedTicks);
    }

private:
    uint64_t m_fixedTicks{0};
};

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;
    SandboxApp app;
    return app.run();
}
