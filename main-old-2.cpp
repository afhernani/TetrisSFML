#include <SFML/Graphics.hpp>
#include <array>
#include <random>
#include <iostream>

// Configuración
constexpr int COLS = 10;
constexpr int ROWS = 20;
constexpr int BLOCK_SIZE = 30;
constexpr float FALL_INTERVAL = 0.5f; // Velocidad base

int board[ROWS][COLS] = {0};

struct Piece {
    std::array<std::array<int, 4>, 4> shape;
    int x, y;
    sf::Color color;
};

// Definición de las 7 piezas estándar (CORREGIDO)
const std::array<std::array<int, 4>, 4> SHAPES[7] = {
    // I
    {{ {0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0} }},
    // O
    {{ {0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0} }},
    // T
    {{ {0,0,0,0}, {1,1,1,0}, {0,1,0,0}, {0,0,0,0} }},
    // S
    {{ {0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0} }},
    // Z
    {{ {0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0} }},
    // J
    {{ {0,0,0,0}, {1,0,0,0}, {1,1,1,0}, {0,0,0,0} }},
    // L
    {{ {0,0,0,0}, {0,0,1,0}, {1,1,1,0}, {0,0,0,0} }}
};

const sf::Color COLORS[7] = {
    sf::Color::Cyan,      // I
    sf::Color::Yellow,    // O
    sf::Color::Magenta,   // T
    sf::Color::Green,     // S
    sf::Color::Red,       // Z
    sf::Color::Blue,      // J
    sf::Color(255, 165, 0) // L (Naranja)
};

// Generador aleatorio (se inicializa una vez en main)
std::mt19937 rng(std::random_device{}());
std::uniform_int_distribution<int> pieceDist(0, 6);

Piece createRandomPiece() {
    int idx = pieceDist(rng);
    Piece p;
    p.shape = SHAPES[idx];
    p.color = COLORS[idx];
    p.x = COLS / 2 - 2; // Centrado aproximado
    p.y = -1;           // Empieza medio fuera para dar tiempo
    return p;
}

bool checkCollision(const Piece& p, int dx, int dy) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (p.shape[r][c]) {
                int nx = p.x + c + dx;
                int ny = p.y + r + dy;
                // Límites laterales e inferiores
                if (nx < 0 || nx >= COLS || ny >= ROWS) return true;
                // Colisión con bloques fijos (ignora filas negativas)
                if (ny >= 0 && board[ny][nx]) return true;
            }
        }
    }
    return false;
}

void lockPiece(const Piece& p) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (p.shape[r][c]) {
                int bx = p.x + c;
                int by = p.y + r;
                if (by >= 0 && by < ROWS && bx >= 0 && bx < COLS) {
                    board[by][bx] = 1; // Guardamos solo 1 (color se pierde, se puede mejorar con struct board)
                }
            }
        }
    }
}

void clearLines() {
    for (int y = ROWS - 1; y >= 0; --y) {
        bool full = true;
        for (int x = 0; x < COLS; ++x) {
            if (!board[y][x]) { full = false; break; }
        }
        if (full) {
            for (int yy = y; yy > 0; --yy)
                for (int x = 0; x < COLS; ++x)
                    board[yy][x] = board[yy-1][x];
            for (int x = 0; x < COLS; ++x) board[0][x] = 0;
            ++y; // Revisar la misma posición de nuevo
        }
    }
}

// Rotar pieza 90° en sentido horario
std::array<std::array<int, 4>, 4> rotateShape(const std::array<std::array<int, 4>, 4>& shape) {
    std::array<std::array<int, 4>, 4> rotated{};
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            rotated[r][c] = shape[3 - c][r];  // Fórmula de rotación 90° CW
    return rotated;
}

// Intentar rotar la pieza, con ajuste básico si choca (wall kick simple)
bool tryRotate(Piece& p) {
    Piece original = p;
    p.shape = rotateShape(p.shape);
    
    // Si rota sin colisión → éxito
    if (!checkCollision(p, 0, 0)) return true;
    
    // Wall kick: intentar desplazar 1 celda a izq/der si la rotación choca
    if (!checkCollision(p, -1, 0)) { p.x--; return true; }
    if (!checkCollision(p,  1, 0)) { p.x++; return true; }
    if (!checkCollision(p,  0,-1)) { p.y--; return true; } // útil para pieza I cerca del techo
    
    // Si nada funciona, revertir rotación
    p = original;
    return false;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(COLS * BLOCK_SIZE, ROWS * BLOCK_SIZE), "Tetris WSL - SFML");
    window.setFramerateLimit(60);

    sf::Clock fallClock;
    Piece current = createRandomPiece();

    sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE - 2, BLOCK_SIZE - 2));
    block.setOutlineThickness(1.f);
    block.setOutlineColor(sf::Color(50, 50, 50));

    bool gameOver = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::KeyPressed && !gameOver) {
                if (event.key.code == sf::Keyboard::Left && !checkCollision(current, -1, 0))
                    current.x--;
                if (event.key.code == sf::Keyboard::Right && !checkCollision(current, 1, 0))
                    current.x++;
                if (event.key.code == sf::Keyboard::Down && !checkCollision(current, 0, 1))
                    current.y++;
                
                // 🔄 ROTACIÓN: teclas Up, Z o X
                if (event.key.code == sf::Keyboard::Up || 
                    event.key.code == sf::Keyboard::Z || 
                    event.key.code == sf::Keyboard::X) {
                    tryRotate(current);
                }
                
                if (event.key.code == sf::Keyboard::R) { // Reiniciar
                    for(auto& row : board) for(auto& cell : row) cell = 0;
                    current = createRandomPiece();
                    gameOver = false;
                    fallClock.restart();
                }
            }
        }

        // Caída automática
        if (!gameOver && fallClock.getElapsedTime().asSeconds() >= FALL_INTERVAL) {
            fallClock.restart();
            if (!checkCollision(current, 0, 1)) {
                current.y++;
            } else {
                lockPiece(current);
                clearLines();
                current = createRandomPiece();
                if (checkCollision(current, 0, 0)) gameOver = true;
            }
        }

        // Renderizado
        window.clear(sf::Color(20, 20, 30));

        // Dibujar grid sutil
        sf::Vertex gridLine[2];
        for (int i = 0; i <= COLS; ++i) {
            gridLine[0].position = sf::Vector2f(i * BLOCK_SIZE, 0);
            gridLine[1].position = sf::Vector2f(i * BLOCK_SIZE, ROWS * BLOCK_SIZE);
            gridLine[0].color = gridLine[1].color = sf::Color(40, 40, 50);
            window.draw(gridLine, 2, sf::Lines);
        }
        for (int i = 0; i <= ROWS; ++i) {
            gridLine[0].position = sf::Vector2f(0, i * BLOCK_SIZE);
            gridLine[1].position = sf::Vector2f(COLS * BLOCK_SIZE, i * BLOCK_SIZE);
            window.draw(gridLine, 2, sf::Lines);
        }

        // Bloques fijos
        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                if (board[y][x]) {
                    block.setPosition(x * BLOCK_SIZE + 1, y * BLOCK_SIZE + 1);
                    block.setFillColor(sf::Color::White); // Simplificado, se puede guardar color por celda
                    window.draw(block);
                }
            }
        }

        // Pieza activa
        if (!gameOver) {
            block.setFillColor(current.color);
            block.setOutlineColor(sf::Color::White); // Borde blanco para mejor visibilidad
            block.setOutlineThickness(1.5f);
            for (int r = 0; r < 4; ++r) {
                for (int c = 0; c < 4; ++c) {
                    if (current.shape[r][c]) {
                        block.setPosition((current.x + c) * BLOCK_SIZE + 1, (current.y + r) * BLOCK_SIZE + 1);
                        window.draw(block);
                    }
                }
            }
        }

        // UI
        sf::Font font;
        if (font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf") ||
            font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf")) {
            
            if (gameOver) {
                sf::Text text("GAME OVER\nPresiona R", font, 24);
                text.setFillColor(sf::Color::Red);
                float tw = text.getGlobalBounds().width;
                text.setPosition((COLS * BLOCK_SIZE - tw) / 2, ROWS * BLOCK_SIZE / 2 - 30);
                window.draw(text);
            } else {
                sf::Text text("← → Mover | ↓ Acelerar | R Reiniciar", font, 14);
                text.setFillColor(sf::Color(200, 200, 200));
                float tw = text.getGlobalBounds().width;
                text.setPosition((COLS * BLOCK_SIZE - tw) / 2, ROWS * BLOCK_SIZE - 20);
                window.draw(text);
            }
        }

        window.display();
    }

    return 0;
}