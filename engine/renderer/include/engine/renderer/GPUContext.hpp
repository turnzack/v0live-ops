#pragma once

#include <webgpu/webgpu_cpp.h>
#include <SDL3/SDL.h>
#include <cstdint>

namespace engine {

class GPUContext {
public:
    bool initialize(SDL_Window* window);
    void shutdown();

    void resize(std::uint32_t width, std::uint32_t height);

    [[nodiscard]] wgpu::Device device() const noexcept { return device_; }
    [[nodiscard]] wgpu::Surface surface() const noexcept { return surface_; }
    [[nodiscard]] wgpu::TextureFormat surfaceFormat() const noexcept { return format_; }

    [[nodiscard]] bool beginFrame(wgpu::SurfaceTexture& surfaceTexture);
    void present();

private:
    bool createInstance();
    bool createSurface(SDL_Window* window);
    bool createAdapter();
    bool createDevice();
    void configureSurface();

    wgpu::Instance instance_;
    wgpu::Adapter adapter_;
    wgpu::Device device_;
    wgpu::Surface surface_;
    wgpu::TextureFormat format_ = wgpu::TextureFormat::Undefined;

    std::uint32_t width_ = 0;
    std::uint32_t height_ = 0;
};

} // namespace engine
