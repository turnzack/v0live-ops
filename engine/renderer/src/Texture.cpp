#include "engine/renderer/Texture.hpp"

#include <spdlog/spdlog.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace engine {

bool Texture::loadFromFile(const std::string& path, wgpu::Device device) {
    int channels = 0;

    unsigned char* pixels = stbi_load(
        path.c_str(),
        &width_,
        &height_,
        &channels,
        STBI_rgb_alpha
    );

    if (pixels == nullptr) {
        spdlog::error("Failed to load texture {}: {}", path, stbi_failure_reason());
        return false;
    }

    wgpu::TextureDescriptor descriptor{};
    descriptor.dimension = wgpu::TextureDimension::e2D;
    descriptor.format = wgpu::TextureFormat::RGBA8Unorm;
    descriptor.mipLevelCount = 1;
    descriptor.sampleCount = 1;
    descriptor.size = {
        static_cast<std::uint32_t>(width_),
        static_cast<std::uint32_t>(height_),
        1
    };
    descriptor.usage =
        wgpu::TextureUsage::TextureBinding |
        wgpu::TextureUsage::CopyDst;

    texture_ = device.CreateTexture(&descriptor);

    wgpu::TexelCopyTextureInfo destination{};
    destination.texture = texture_;
    destination.mipLevel = 0;
    destination.origin = {0, 0, 0};
    destination.aspect = wgpu::TextureAspect::All;

    wgpu::TexelCopyBufferLayout layout{};
    layout.offset = 0;
    layout.bytesPerRow = static_cast<std::uint32_t>(width_ * 4);
    layout.rowsPerImage = static_cast<std::uint32_t>(height_);

    wgpu::Extent3D size{
        static_cast<std::uint32_t>(width_),
        static_cast<std::uint32_t>(height_),
        1
    };

    device.GetQueue().WriteTexture(
        &destination,
        pixels,
        static_cast<std::size_t>(width_ * height_ * 4),
        &layout,
        &size
    );

    stbi_image_free(pixels);

    wgpu::TextureViewDescriptor viewDescriptor{};
    viewDescriptor.dimension = wgpu::TextureViewDimension::e2D;
    viewDescriptor.format = wgpu::TextureFormat::RGBA8Unorm;
    viewDescriptor.baseMipLevel = 0;
    viewDescriptor.mipLevelCount = 1;
    viewDescriptor.baseArrayLayer = 0;
    viewDescriptor.arrayLayerCount = 1;

    view_ = texture_.CreateView(&viewDescriptor);

    spdlog::info("Texture loaded: {} ({}x{})", path, width_, height_);

    return true;
}

void Texture::shutdown() {
    view_ = nullptr;
    texture_ = nullptr;
}

} // namespace engine
