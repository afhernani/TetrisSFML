#pragma once
#include <array>
#include "Constants.h"
#include "Piece.h"

class Board {
public:
    Board();
    
    void clear();
    bool isFull(int row) const;
    void clearRow(int row);
    int clearFullRows();  // Retorna número de líneas borradas
    
    void lockPiece(const Piece& piece);
    bool isOccupied(int x, int y) const;
    bool isGameOver(const Piece& nextPiece) const;
    
    // Acceso para renderizado
    int getValue(int x, int y) const { return cells[y][x]; }
    int getColorType(int x, int y) const { return cellColors[y][x]; }
    void setColorType(int x, int y, int type) { cellColors[y][x] = type; }
    
private:
    std::array<std::array<int, COLS>, ROWS> cells;        // 0=vacío, 1=ocupado
    std::array<std::array<int, COLS>, ROWS> cellColors;   // Tipo de pieza (0-6) para color
};