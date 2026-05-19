#pragma once
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>

class SoundManager {
public:
    static SoundManager& instance();
    
    bool load(const std::string& name, const std::string& filepath);
    void play(const std::string& name);
    void setVolume(float volume); // 0.0 - 100.0

private:
    SoundManager() = default;
    ~SoundManager() = default;
    
    std::unordered_map<std::string, sf::SoundBuffer> buffers;
    std::unordered_map<std::string, sf::Sound> sounds;
    float volume = 80.f;
};