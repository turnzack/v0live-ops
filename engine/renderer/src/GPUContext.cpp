#include "engine/renderer/GPUContext.hpp"

#include <spdlog/spdlog.h>
#include <string_view>

#include "engine/core/assert.hpp"
#include "sdl3webgpu.h"

namespace engine {

bool GPUContext::initialize(SDL_Window* window) {
    ENGINE_VERIFY(window != nullptr, "GPUContext requires a valid SDL window");

    if (!createInstance()) return false;
    if (!createSurface(window)) return false;
    if (!createAdapter()) return false;
    if (!createDevice()) return false;

    int width = 0;
    int height = 0;

    SDL_GetWindowSizeInPixels(window, &width, &height);

    width_ = static_cast<std::uint32_t>(width);
    height_ = static_cast<std::uint32_t>(height);

    configureSurface();

    spdlog::info("WebGPU context initialized: {}x{}", width_, height_);

    return true;
}

void GPUContext::shutdown() {
    surface_ = nullptr;
    device_ = nullptr;
    adapter_ = nullptr;
    instance_ = nullptr;
}

void GPUContext::resize(std::uint32_t width, std::uint32_t height) {
    if (!device_ || !surface_ || width == 0 || height == 0) {
        return;
    }

    width_ = width;
    height_ = height;

    configureSurface();
}

bool GPUContext::createInstance() {
    instance_ = wgpu::CreateInstance();

    if (!instance_) {
        spdlog::error("Failed to create WebGPU instance.");
        return false;
    }

    return true;
}

bool GPUContext::createSurface(SDL_Window* window) {
    surface_ = SDL_GetWGPUSurface(instance_, window);

    if (!surface_) {
        spdlog::error("Failed to create WebGPU surface from SDL3 window.");
        return false;
    }

    return true;
}

bool GPUContext::createAdapter() {
    wgpu::RequestAdapterOptions options{};
    options.compatibleSurface = surface_;

    wgpu::Future future = instance_.RequestAdapter(
        &options,
        wgpu::CallbackMode::WaitAnyOnly,
        [&](wgpu::RequestAdapterStatus status,
            wgpu::Adapter result,
            wgpu::StringView message) {
            if (status != wgpu::RequestAdapterStatus::Success) {
                spdlog::error(
                    "Adapter request failed: {}",
                    std::string_view{message.data, message.length}
                );

                return;
            }

            adapter_ = std::move(result);
        }
    );

    instance_.WaitAny(future, UINT64_MAX);

    return static_cast<bool>(adapter_);
}

bool GPUContext::createDevice() {
    wgpu::DeviceDescriptor descriptor{};

    descriptor.SetUncapturedErrorCallback(
        [](const wgpu::Device&,
           wgpu::ErrorType type,
           wgpu::StringView message) {
            spdlog::error(
                "WebGPU error [{}]: {}",
                static_cast<int>(type),
                std::string_view{message.data, message.length}
            );
        }
    );

    wgpu::Future future = adapter_.RequestDevice(
        &descriptor,
        wgpu::CallbackMode::WaitAnyOnly,
        [&](wgpu::RequestDeviceStatus status,
            wgpu::Device result,
            wgpu::StringView message) {
            if (status != wgpu::RequestDeviceStatus::Success) {
                spdlog::error(
                    "Device request failed: {}",
                    std::string_view{message.data, message.length}
                );

                return;
            }

            device_ = std::move(result);
        }
    );

    instance_.WaitAny(future, UINT64_MAX);

    return static_cast<bool>(device_);
}

void GPUContext::configureSurface() {
    wgpu::SurfaceCapabilities capabilities{};
    surface_.GetCapabilities(adapter_, &capabilities);

    if (capabilities.formatCount == 0) {
        spdlog::error("No supported surface format.");
        return;
    }

    format_ = capabilities.formats[0];

    wgpu::SurfaceConfiguration config{};
    config.device = device_;
    config.format = format_;
    config.width = width_;
    config.height = height_;
    config.usage = wgpu::TextureUsage::RenderAttachment;
    config.alphaMode = wgpu::CompositeAlphaMode::Auto;

    surface_.Configure(&config);
}

bool GPUContext::beginFrame(wgpu::SurfaceTexture& surfaceTexture) {
    surface_.GetCurrentTexture(&surfaceTexture);

    if (surfaceTexture.status != wgpu::SurfaceGetCurrentTextureStatus::Success) {
        return false;
    }

    return true;
}

void GPUContext::present() {
    surface_.Present();
}

} // namespace engine
