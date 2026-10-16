#include <SDL3/SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_wgpu.h>

#include <spdlog/spdlog.h>

#include "editor/EditorApp.hpp"
#include "engine/core/Log.hpp"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    engine::core::Log::initialize();

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        spdlog::error("SDL_Init failed: {}", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Engine Suite Editor",
        1600,
        900,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY
    );

    if (window == nullptr) {
        spdlog::error("SDL_CreateWindow failed: {}", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    editor::EditorApp editor;

    if (!editor.initialize(window)) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;

    while (running) {
        SDL_Event event{};

        while (SDL_PollEvent(&event)) {
            editor.handleEvent(event);

            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        editor.newFrame();

        editor.render();
    }

    editor.shutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
