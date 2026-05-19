#include "Game.h"
#include "Constants.h"
#include "SoundManager.h"
#include <iostream>

Game::Game() 
    : window(sf::VideoMode(COLS * BLOCK_SIZE, ROWS * BLOCK_SIZE), "TetrisSFML"),
      fallSpeed(FALL_INTERVAL),
      score(0), level(1), linesCleared(0), gameOver(false) {
    
    auto& sm = SoundManager::instance();
    sm.load("rotate", "sounds/rotate.wav");
    sm.load("drop", "sounds/drop.wav");
    sm.load("line_clear", "sounds/line_clear.wav");
    sm.load("game_over", "sounds/game_over.wav");

    window.setFramerateLimit(60);
    currentPiece = createRandomPiece();
    nextPiece = createRandomPiece();
    loadFont();
}

bool Game::loadFont() {
    if (font.loadFromFile(FONT_PATH_1) || font.loadFromFile(FONT_PATH_2)) {
        m_fontLoaded = true; //Fuente cargada correctamente
        return true;
    }
    std::cerr << "Warning: Could not load font. Text will not be drawn.\n";
    return false;
}

void Game::spawnNewPiece() {
    currentPiece = nextPiece;
    nextPiece = createRandomPiece();
    
    if (board.isGameOver(currentPiece)) {
        gameOver = true;
        SoundManager::instance().play("game_over");
    }
}

void Game::tryRotatePiece() {
    if (currentPiece.canRotate(board)) {
        currentPiece.rotate();
        SoundManager::instance().play("rotate"); 
    } else {
        // Wall kick básico
        int kicks[] = {-1, 1, 0, 0};
        int kickY[] = {0, 0, -1, 1};
        for (int i = 0; i < 4; ++i) {
            currentPiece.x += kicks[i];
            currentPiece.y += kickY[i];
            if (currentPiece.canRotate(board)) {
                currentPiece.rotate();
                return;
            }
            currentPiece.x -= kicks[i];
            currentPiece.y -= kickY[i];
        }
    }
}

void Game::dropPiece() {
    if (currentPiece.canMove(0, 1, board)) {
        currentPiece.y++;
    } else {
        board.lockPiece(currentPiece);
        int cleared = board.clearFullRows();
        if (cleared > 0) {
            updateScore(cleared);
            SoundManager::instance().play("line_clear");
        }
        spawnNewPiece();
    }
}

void Game::hardDrop() {
    while (currentPiece.canMove(0, 1, board)) {
        currentPiece.y++;
        score += 2; // Bonus por hard drop
    }
    SoundManager::instance().play("drop");
    dropPiece(); // Fijar inmediatamente
}

void Game::updateScore(int lines) {
    // Sistema de puntuación clásico
    const int points[] = {0, 40, 100, 300, 1200};
    score += points[lines] * level;
    linesCleared += lines;
    
    // Subir nivel cada 10 líneas
    level = linesCleared / 10 + 1;
    fallSpeed = std::max(0.05f, FALL_INTERVAL - (level - 1) * 0.05f);
}

void Game::handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();

        if (event.type == sf::Event::KeyPressed) {
            // R funciona SIEMPRE (para reiniciar en cualquier estado)
            if (event.key.code == sf::Keyboard::R) {
                board.clear();
                score = 0; 
                level = 1; 
                linesCleared = 0;
                fallSpeed = FALL_INTERVAL;
                currentPiece = createRandomPiece();
                nextPiece = createRandomPiece();
                gameOver = false;
                fallClock.restart(); // Crucial: evita caída inmediata al reiniciar
                return; // Salir para no procesar otras teclas en el mismo frame
            }

            // El resto de controles solo responden si el juego está activo
            if (!gameOver) {
                switch (event.key.code) {
                    case sf::Keyboard::Left:
                        if (currentPiece.canMove(-1, 0, board)) currentPiece.x--;
                        break;
                    case sf::Keyboard::Right:
                        if (currentPiece.canMove(1, 0, board)) currentPiece.x++;
                        break;
                    case sf::Keyboard::Down:
                        if (currentPiece.canMove(0, 1, board)) {
                            currentPiece.y++;
                            score += 1;
                        }
                        SoundManager::instance().play("drop");
                        break;
                    case sf::Keyboard::Up:
                    case sf::Keyboard::Z:
                    case sf::Keyboard::X:
                        tryRotatePiece();
                        break;
                    case sf::Keyboard::Space:
                        hardDrop();
                        break;
                    default: break;
                }
            }
        }
    }
}

void Game::update(float dt) {
    if (gameOver) return;
    
    if (fallClock.getElapsedTime().asSeconds() >= fallSpeed) {
        fallClock.restart();
        dropPiece();
    }
}

void Game::drawGhostPiece() {
    Piece ghost = currentPiece;
    while (ghost.canMove(0, 1, board)) ghost.y++;
    
    sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE - 2, BLOCK_SIZE - 2));
    block.setOutlineThickness(1.f);
    block.setOutlineColor(sf::Color(100, 100, 100));
    block.setFillColor(sf::Color(255, 255, 255, 50)); // Semi-transparente
    
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (ghost.shape[r][c]) {
                block.setPosition((ghost.x + c) * BLOCK_SIZE + 1, 
                                (ghost.y + r) * BLOCK_SIZE + 1);
                window.draw(block);
            }
        }
    }
}

void Game::drawBoard() {
    // Grid de fondo
    sf::Vertex line[2];
    for (int i = 0; i <= COLS; ++i) {
        line[0].position = sf::Vector2f(i * BLOCK_SIZE, 0);
        line[1].position = sf::Vector2f(i * BLOCK_SIZE, ROWS * BLOCK_SIZE);
        line[0].color = line[1].color = sf::Color(40, 40, 50);
        window.draw(line, 2, sf::Lines);
    }
    for (int i = 0; i <= ROWS; ++i) {
        line[0].position = sf::Vector2f(0, i * BLOCK_SIZE);
        line[1].position = sf::Vector2f(COLS * BLOCK_SIZE, i * BLOCK_SIZE);
        window.draw(line, 2, sf::Lines);
    }
    
    // Bloques fijos con sus colores originales
    sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE - 2, BLOCK_SIZE - 2));
    block.setOutlineThickness(1.f);
    block.setOutlineColor(sf::Color(30, 30, 40));
    
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            if (board.getValue(x, y)) {
                int type = board.getColorType(x, y);
                block.setPosition(x * BLOCK_SIZE + 1, y * BLOCK_SIZE + 1);
                block.setFillColor(PIECE_COLORS[type]);
                window.draw(block);
            }
        }
    }
}

void Game::drawPiece(const Piece& p, sf::Color overrideColor) {
    sf::RectangleShape block(sf::Vector2f(BLOCK_SIZE - 2, BLOCK_SIZE - 2));
    block.setOutlineThickness(1.5f);
    block.setOutlineColor(sf::Color::White);
    block.setFillColor(overrideColor != sf::Color::Transparent ? overrideColor : p.color);
    
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (p.shape[r][c]) {
                block.setPosition((p.x + c) * BLOCK_SIZE + 1, 
                                (p.y + r) * BLOCK_SIZE + 1);
                window.draw(block);
            }
        }
    }
}

void Game::drawUI() {
    if (!m_fontLoaded) return;
    
    // Puntuación y nivel
    sf::Text stats("", font, 14);
    stats.setFillColor(sf::Color(220, 220, 220));
    stats.setPosition(10, ROWS * BLOCK_SIZE - 40);
    stats.setString("Score: " + std::to_string(score) + 
                   "  Level: " + std::to_string(level) +
                   "  Lines: " + std::to_string(linesCleared));
    window.draw(stats);
    
    // Controles
    sf::Text controls("←→ Mover | ↑/Z/X Rotar | ↓ Soft | Espacio Hard | R Reiniciar", font, 12);
    controls.setFillColor(sf::Color(150, 150, 150));
    controls.setPosition(10, ROWS * BLOCK_SIZE - 20);
    window.draw(controls);
    
    // Game Over
    if (gameOver) {
        sf::Text go("GAME OVER\nPresiona R para reiniciar", font, 24);
        go.setFillColor(sf::Color::Red);
        go.setPosition(COLS * BLOCK_SIZE / 2 - go.getGlobalBounds().width / 2,
                      ROWS * BLOCK_SIZE / 2 - 30);
        window.draw(go);
    }
    
    // Preview de siguiente pieza (esquina superior derecha)
    sf::Text nextLabel("NEXT", font, 14);
    nextLabel.setFillColor(sf::Color(200, 200, 200));
    nextLabel.setPosition(COLS * BLOCK_SIZE - 80, 10);
    window.draw(nextLabel);
    
    // Dibujar preview en área de 4x4 bloques a la derecha
    Piece preview = nextPiece;
    preview.x = COLS + 1;  // Posición fuera del tablero principal
    preview.y = 1;
    drawPiece(preview);
}

void Game::render() {
    window.clear(sf::Color(20, 20, 35));
    
    drawBoard(); //tableros y bloques fijos
    drawGhostPiece();      // Sombra de caída
    drawPiece(currentPiece); // Pieza activa
    drawUI(); // Puntuación, controles, siguiente pieza
    
    window.display();
}

void Game::run() {
    while (window.isOpen()) {
        handleInput();
        update(0.016f); // ~60 FPS
        render();
    }
}