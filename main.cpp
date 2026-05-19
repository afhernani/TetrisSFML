#include "Game.h"

int main() {
    setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1); // ← Añade esta línea
    Game game;
    game.run();
    return 0;
}