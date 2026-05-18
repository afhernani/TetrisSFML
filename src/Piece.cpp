#include "Piece.h"
#include "Board.h" // ⬅️ Incluye la definición completa de Board para usar isOccupied()
#include <random>

// Formas base de los 7 tetrominós
const std::array<std::array<int, 4>, 4> BASE_SHAPES[7] = {
    {{{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}}, // I
    {{{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}}}, // O
    {{{0,0,0,0}, {1,1,1,0}, {0,1,0,0}, {0,0,0,0}}}, // T
    {{{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}}}, // S
    {{{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}}}, // Z
    {{{0,0,0,0}, {1,0,0,0}, {1,1,1,0}, {0,0,0,0}}}, // J
    {{{0,0,0,0}, {0,0,1,0}, {1,1,1,0}, {0,0,0,0}}}  // L
};

Piece::Piece() : x(COLS/2 - 2), y(-1), type(0), color(PIECE_COLORS[0]) {
    shape = BASE_SHAPES[0];
}

Piece::Piece(int pieceType) : x(COLS/2 - 2), y(-1), type(pieceType) {
    color = PIECE_COLORS[pieceType];
    shape = BASE_SHAPES[pieceType];
}

std::array<std::array<int, 4>, 4> Piece::rotateMatrix(
    const std::array<std::array<int, 4>, 4>& s) {
    
    std::array<std::array<int, 4>, 4> rotated{};
    for (int r = 0; r < 4; ++r)
        for (int c = 0; c < 4; ++c)
            rotated[r][c] = s[3 - c][r];
    return rotated;
}

void Piece::rotate() {
    shape = rotateMatrix(shape);
}

bool Piece::canRotate(const Board& board) const {
    auto rotated = rotateMatrix(shape);
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (rotated[r][c]) {
                int nx = x + c, ny = y + r;
                if (nx < 0 || nx >= COLS || ny >= ROWS) return false;
                if (ny >= 0 && board.isOccupied(nx, ny)) return false;
            }
        }
    }
    return true;
}

bool Piece::canMove(int dx, int dy, const Board& board) const {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (shape[r][c]) {
                int nx = x + c + dx, ny = y + r + dy;
                if (nx < 0 || nx >= COLS || ny >= ROWS) return false;
                if (ny >= 0 && board.isOccupied(nx, ny)) return false;
            }
        }
    }
    return true;
}

std::array<std::array<int, 4>, 4> Piece::getShape(int type) {
    return BASE_SHAPES[type];
}

sf::Color Piece::getColor(int type) {
    return PIECE_COLORS[type];
}

// ✅ Función libre accesible desde Game.cpp
Piece createRandomPiece() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 6);
    return Piece(dist(rng));
}