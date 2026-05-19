#include "SoundManager.h"
#include <iostream>

SoundManager& SoundManager::instance() {
    static SoundManager manager;
    return manager;
}

bool SoundManager::load(const std::string& name, const std::string& filepath) {
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filepath)) {
        std::cerr << "⚠️ SoundManager: No se pudo cargar '" << filepath << "'\n";
        return false;
    }
    buffers[name] = std::move(buffer);
    sounds[name].setBuffer(buffers[name]);
    sounds[name].setVolume(volume);
    return true;
}

void SoundManager::play(const std::string& name) {
    auto it = sounds.find(name);
    if (it != sounds.end()) {
        it->second.stop(); // Reinicia si ya está sonando
        it->second.play();
    }
}

void SoundManager::setVolume(float vol) {
    volume = std::max(0.f, std::min(100.f, vol));
    for (auto& [_, s] : sounds) s.setVolume(volume);
}