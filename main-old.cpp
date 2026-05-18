#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <iostream>

// Configuración
const int COLS = 10;
const int ROWS = 20;
const int BLOCK_SIZE = 30;
const float FALL_INTERVAL = 0.6f; // segundos

// Representación del tablero: 0 = vacío, 1 = bloque fijo
int board[ROWS][COLS] = {0};

// Pieza actual
struct Piece {
    std::array<std::array<int, 4>, 4> shape;
    int x, y;
    sf::Color color;
};

// Generar una pieza simple (cuadrado 2x2 como ejemplo base)
Piece createPiece() {
    Piece p;
    p.shape = {{
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }};
    p.x = COLS / 2 - 1;
    p.y = 0;
    p.color = sf::Color::Cyan;
    return p;
}

// Verificar colisión
bool checkCollision(const Piece& p, int dx, int dy) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (p.shape[r][c]) {
                int nx = p.x + c + dx;
                int ny = p.y + r + dy;
                if (nx < 0 || nx >= COLS || ny >= ROWS) return true;
                if (ny >= 0 && board[ny][nx]) return true;
            }
        }
    }
    return false;
}

// Fijar pieza al tablero
void lockPiece(const Piece& p) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (p.shape[r][c]) {
                int bx = p.x + c;
                int by = p.y + r;
                if (by >= 0 && by < ROWS && bx >= 0 && bx < COLS) {
                    board[by][bx] = 1;
                }
            }
        }
    }
}

// Limpiar líneas completas
void clearLines() {
    for (int y = ROWS - 1; y >= 0; --y) {
        bool full = true;
        for (int x = 0; x < COLS; ++x) {
            if (!board[y][x]) { full = false; break; }
        }
        if (full) {
            // Desplazar todo hacia abajo
            for (int yy = y; yy > 0; --yy) {
                for (int x = 0; x < COLS; ++x) board[yy][x] = board[yy-1][x];
            }
            for (int x = 0; x < COLS; ++x) board[0][x] = 0;
            ++y; // revisar la misma fila de nuevo
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(COLS * BLOCK_SIZE, ROWS * BLOCK_SIZE), "Tetris WSL - SFML");
    window.setFramerateLimit(60);

    sf::Clock fallClock;
    Piece current = createPiece();

    sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));
    block.setOutlineThickness(1.f);
    block.setOutlineColor(sf::Color::Black);

    bool gameOver = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::KeyPressed && !gameOver) {
                if (event.key.code == sf::Keyboard::Left && !checkCollision(current, -1, 0))
                    current.x--;
                if (event.key.code == sf::Keyboard::Right && !checkCollision(current, 1, 0))
                    current.x++;
                if (event.key.code == sf::Keyboard::Down) {
                    if (!checkCollision(current, 0, 1)) current.y++;
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
                current = createPiece();
                if (checkCollision(current, 0, 0)) gameOver = true;
            }
        }

        // Renderizado
        window.clear(sf::Color(30, 30, 30));

        // Dibujar bloques fijos
        block.setFillColor(sf::Color::Green);
        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                if (board[y][x]) {
                    block.setPosition(x * BLOCK_SIZE, y * BLOCK_SIZE);
                    window.draw(block);
                }
            }
        }

        // Dibujar pieza activa
        if (!gameOver) {
            block.setFillColor(current.color);
            for (int r = 0; r < 4; ++r) {
                for (int c = 0; c < 4; ++c) {
                    if (current.shape[r][c]) {
                        block.setPosition((current.x + c) * BLOCK_SIZE, (current.y + r) * BLOCK_SIZE);
                        window.draw(block);
                    }
                }
            }
        }

        // Texto Game Over
        if (gameOver) {
            sf::Font font;
            // En WSL/Linux usa una fuente disponible:
            if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
                // Fallback si no existe (puedes instalar fonts-ubuntu)
                font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf");
            }
            sf::Text text("GAME OVER", font, 32);
            text.setFillColor(sf::Color::Red);
            text.setPosition(COLS * BLOCK_SIZE / 2 - text.getGlobalBounds().width / 2,
                             ROWS * BLOCK_SIZE / 2 - 16);
            window.draw(text);
        }

        window.display();
    }

    return 0;
}