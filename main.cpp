#include "Game.h"
#include <cstdlib>

int main() {
       // Solo necesario en Linux/WSL para evitar el crash de OpenGL Zink
#if defined(__linux__) || defined(__APPLE__)
    setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
#endif

    Game game;
    game.run();
    return 0;
}