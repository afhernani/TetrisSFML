#pragma once
#include <array>
#include <SFML/Graphics/Color.hpp>
#include "Constants.h"

class Board; // ⬅️ Forward declaration para evitar dependencia circular

class Piece {
public:
    std::array<std::array<int, 4>, 4> shape;
    int x, y;
    sf::Color color;
    int type; // 0-6

    Piece();
    Piece(int pieceType);
    
    void rotate();
    
    // ✅ Ahora reciben const Board& en lugar de raw array
    bool canRotate(const Board& board) const;
    bool canMove(int dx, int dy, const Board& board) const;
    
    static std::array<std::array<int, 4>, 4> getShape(int type);
    static sf::Color getColor(int type);
    
private:
    static std::array<std::array<int, 4>, 4> rotateMatrix(
        const std::array<std::array<int, 4>, 4>& shape);
};

// ✅ Declaración explícita para que Game.cpp pueda llamarla
Piece createRandomPiece();