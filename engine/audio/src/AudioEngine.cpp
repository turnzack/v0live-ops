#define MINIAUDIO_IMPLEMENTATION
#include "engine/audio/AudioEngine.hpp"

#include <spdlog/spdlog.h>

namespace engine {

bool AudioEngine::initialize() {
    if (initialized_) {
        return true;
    }

    if (ma_engine_init(nullptr, &engine_) != MA_SUCCESS) {
        spdlog::error("Failed to initialize miniaudio engine.");
        return false;
    }

    masterGroup_ = ma_engine_get_master_sound_group(&engine_);

    if (ma_sound_group_init(&engine_, 0, masterGroup_, &musicGroupStorage_) != MA_SUCCESS) {
        spdlog::error("Failed to create Music group.");
        return false;
    }

    if (ma_sound_group_init(&engine_, 0, masterGroup_, &sfxGroupStorage_) != MA_SUCCESS) {
        spdlog::error("Failed to create SFX group.");
        return false;
    }

    if (ma_sound_group_init(&engine_, 0, masterGroup_, &uiGroupStorage_) != MA_SUCCESS) {
        spdlog::error("Failed to create UI group.");
        return false;
    }

    musicGroup_ = &musicGroupStorage_;
    sfxGroup_ = &sfxGroupStorage_;
    uiGroup_ = &uiGroupStorage_;

    initialized_ = true;
    spdlog::info("Audio engine initialized.");
    return true;
}

void AudioEngine::shutdown() {
    if (!initialized_) {
        return;
    }

    stopMusic();

    if (musicGroup_) ma_sound_group_uninit(musicGroup_);
    if (sfxGroup_) ma_sound_group_uninit(sfxGroup_);
    if (uiGroup_) ma_sound_group_uninit(uiGroup_);

    ma_engine_uninit(&engine_);

    musicGroup_ = nullptr;
    sfxGroup_ = nullptr;
    uiGroup_ = nullptr;
    masterGroup_ = nullptr;

    initialized_ = false;
}

bool AudioEngine::playOneShot(const std::string& path, AudioBus bus) {
    if (!initialized_) {
        return false;
    }
    return ma_engine_play_sound(&engine_, path.c_str(), group(bus)) == MA_SUCCESS;
}

bool AudioEngine::playMusic(const std::string& path, bool loop) {
    if (!initialized_) {
        return false;
    }

    stopMusic();

    if (ma_sound_init_from_file(
            &engine_,
            path.c_str(),
            MA_SOUND_FLAG_STREAM,
            musicGroup_,
            nullptr,
            &music_
        ) != MA_SUCCESS) {
        spdlog::error("Failed to load music: {}", path);
        return false;
    }

    ma_sound_set_looping(&music_, loop ? MA_TRUE : MA_FALSE);
    ma_sound_start(&music_);
    musicLoaded_ = true;

    return true;
}

void AudioEngine::stopMusic() {
    if (!musicLoaded_) {
        return;
    }

    ma_sound_stop(&music_);
    ma_sound_uninit(&music_);
    musicLoaded_ = false;
}

void AudioEngine::setBusVolume(AudioBus bus, float volume) {
    ma_sound_group* target = group(bus);
    if (target != nullptr) {
        ma_sound_group_set_volume(target, volume);
    }
}

float AudioEngine::busVolume(AudioBus bus) const {
    const ma_sound_group* target = nullptr;

    switch (bus) {
        case AudioBus::Music: target = musicGroup_; break;
        case AudioBus::Sfx:   target = sfxGroup_;   break;
        case AudioBus::Ui:    target = uiGroup_;    break;
        default:              target = masterGroup_; break;
    }

    if (target == nullptr) {
        return 0.0f;
    }

    float volume = 0.0f;
    ma_sound_group_get_volume(const_cast<ma_sound_group*>(target), &volume);
    return volume;
}

ma_sound_group* AudioEngine::group(AudioBus bus) {
    switch (bus) {
        case AudioBus::Music: return musicGroup_;
        case AudioBus::Sfx:   return sfxGroup_;
        case AudioBus::Ui:    return uiGroup_;
        default:              return masterGroup_;
    }
}

} // namespace engine
