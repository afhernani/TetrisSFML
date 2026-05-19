#pragma once
#include <SFML/Graphics/Color.hpp>

// Configuración del juego
constexpr int COLS = 10;
constexpr int ROWS = 20;
constexpr int BLOCK_SIZE = 30;
constexpr float FALL_INTERVAL = 0.5f;

// Rutas de fuentes (WSL/Linux)
constexpr const char* FONT_PATH_1 = "fonts/DejaVuSans-Bold.ttf";
constexpr const char* FONT_PATH_2 = "fonts/LiberationSans-Bold.ttf";

// Colores de las 7 piezas (I, O, T, S, Z, J, L)
inline const sf::Color PIECE_COLORS[7] = {
    sf::Color::Cyan,
    sf::Color::Yellow,
    sf::Color::Magenta,
    sf::Color::Green,
    sf::Color::Red,
    sf::Color::Blue,
    sf::Color(255, 165, 0)
};