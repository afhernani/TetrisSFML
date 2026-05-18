#include "Board.h"

Board::Board() {
    clear();
}

void Board::clear() {
    for (auto& row : cells) row.fill(0);
    for (auto& row : cellColors) row.fill(-1);
}

bool Board::isFull(int row) const {
    for (int x = 0; x < COLS; ++x)
        if (!cells[row][x]) return false;
    return true;
}

void Board::clearRow(int row) {
    for (int y = row; y > 0; --y) {
        cells[y] = cells[y-1];
        cellColors[y] = cellColors[y-1];
    }
    cells[0].fill(0);
    cellColors[0].fill(-1);
}

int Board::clearFullRows() {
    int cleared = 0;
    for (int y = ROWS - 1; y >= 0; --y) {
        if (isFull(y)) {
            clearRow(y);
            ++cleared;
            ++y; // Revisar misma posición
        }
    }
    return cleared;
}

void Board::lockPiece(const Piece& piece) {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (piece.shape[r][c]) {
                int bx = piece.x + c, by = piece.y + r;
                if (by >= 0 && by < ROWS && bx >= 0 && bx < COLS) {
                    cells[by][bx] = 1;
                    cellColors[by][bx] = piece.type;
                }
            }
        }
    }
}

bool Board::isOccupied(int x, int y) const {
    return (y >= 0 && y < ROWS && x >= 0 && x < COLS) ? cells[y][x] : true;
}

bool Board::isGameOver(const Piece& nextPiece) const {
    // Si la nueva pieza no puede spawnear sin colisión
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            if (nextPiece.shape[r][c]) {
                int bx = nextPiece.x + c, by = nextPiece.y + r;
                if (by >= 0 && isOccupied(bx, by)) return true;
            }
        }
    }
    return false;
}