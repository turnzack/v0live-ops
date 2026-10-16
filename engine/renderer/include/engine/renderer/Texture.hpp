#pragma once

#include <webgpu/webgpu_cpp.h>
#include <string>

namespace engine {

class Texture {
public:
    bool loadFromFile(const std::string& path, wgpu::Device device);
    void shutdown();

    [[nodiscard]] wgpu::TextureView view() const noexcept { return view_; }
    [[nodiscard]] int width() const noexcept { return width_; }
    [[nodiscard]] int height() const noexcept { return height_; }

private:
    wgpu::Texture texture_;
    wgpu::TextureView view_;
    int width_ = 0;
    int height_ = 0;
};

} // namespace engine
