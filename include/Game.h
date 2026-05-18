#pragma once
#include <SFML/Graphics.hpp>
#include "Board.h"
#include "Piece.h"

class Game {
public:
    Game();
    void run();
    
private:
    sf::RenderWindow window;
    sf::Font font;
    bool m_fontLoaded = false;
    
    Board board;
    Piece currentPiece;
    Piece nextPiece;
    
    sf::Clock fallClock;
    float fallSpeed;
    
    int score;
    int level;
    int linesCleared;
    bool gameOver;
    
    void handleInput();
    void update(float dt);
    void render();
    
    void spawnNewPiece();
    void tryRotatePiece();
    void dropPiece();
    void hardDrop();
    
    void updateScore(int lines);
    void drawBoard();
    void drawPiece(const Piece& p, sf::Color overrideColor = sf::Color::Transparent);
    void drawUI();
    void drawGhostPiece();
    
    bool loadFont();
};