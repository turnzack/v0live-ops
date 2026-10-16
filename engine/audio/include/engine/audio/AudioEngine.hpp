#pragma once

#include <miniaudio.h>
#include <string>
#include <unordered_map>
#include <cstdint>

namespace engine {

enum class AudioBus : std::uint8_t {
    Master = 0,
    Music,
    Sfx,
    Ui
};

class AudioEngine {
public:
    bool initialize();
    void shutdown();

    bool playOneShot(const std::string& path, AudioBus bus = AudioBus::Sfx);
    bool playMusic(const std::string& path, bool loop = true);
    void stopMusic();

    void setBusVolume(AudioBus bus, float volume);
    [[nodiscard]] float busVolume(AudioBus bus) const;

private:
    [[nodiscard]] ma_sound_group* group(AudioBus bus);

    ma_engine engine_{};
    ma_sound_group* masterGroup_ = nullptr;
    ma_sound_group* musicGroup_ = nullptr;
    ma_sound_group* sfxGroup_ = nullptr;
    ma_sound_group* uiGroup_ = nullptr;

    ma_sound_group musicGroupStorage_{};
    ma_sound_group sfxGroupStorage_{};
    ma_sound_group uiGroupStorage_{};

    ma_sound music_{};
    bool musicLoaded_ = false;
    bool initialized_ = false;
};

} // namespace engine
