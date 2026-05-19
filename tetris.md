# Compilar y ejecutar
```bash
g++ main.cpp -o tetris -lsfml-graphics -lsfml-window -lsfml-system
./tetris
```

# Compilar y ejecutar
```bash
g++ main.cpp -o tetris -lsfml-graphics -lsfml-window -lsfml-system -std=c++17
./tetris
```
# Compilar y ejecutar version factorizada.

- Con CMake (recomendado):
```bash
rm -rf build # si existe
mkdir build && cd build
cmake ..
make
./tetris
```
- Con Makefile:
```bash
make
./tetris
# o simplemente:
make run
```
- Con g++ directo (para pruebas rápidas):
```bash
g++ -std=c++17 -Iinclude src/*.cpp main.cpp -o tetris -lsfml-graphics -lsfml-window -lsfml-system
./tetris
```

## 🎮 Controles finales

| Tecla | Acción |
|--------|------------|
| ← → | Mover pieza |
| ↓ | Caída suave (+1 punto) |
| ↑ / Z / X | Rotar pieza |
| Espacio | Hard drop (caída instantánea, +2 puntos por celda) |
| R | Reiniciar partida |


Con la ghost piece ya integrada, el juego tiene mecánicas casi completas. ¿Quieres añadir ahora:
- 📊 Panel lateral profesional (puntuación, nivel, líneas y preview en rectángulo dedicado)
- 🎵 Sonidos con SFML (sf::SoundBuffer para rotación, caída y línea borrada)
- 🧩 Sistema "7-bag randomizer" (evita que salga la misma pieza 3 veces seguidas, como en Tetris oficial)
- 📦 Exportar a ejecutable Windows (.exe) para jugar sin WSL

# Exportar a ejecutable Windows (.exe) para jugar sin WSL

Para exportar un .exe nativo de Windows que funcione fuera de WSL, necesitamos resolver un detalle crítico primero: las rutas de fuentes de Linux (/usr/share/fonts/...) no existen en Windows. Luego compilaremos el proyecto con herramientas nativas y empaquetaremos las DLLs necesarias.
Sigue estos 4 pasos. Te llevará ~10 minutos.

## Paso 0: Hacer el juego portable (cambiar ruta de fuente)
- Crea una carpeta fonts/ en la raíz de tu proyecto (tetris/fonts/)
- Descarga cualquier .ttf (ej. DejaVuSans-Bold.ttf) y guárdala como fonts/game_font.ttf
- Actualiza include/Constants.h:
```cpp
#pragma once
#include <SFML/Graphics/Color.hpp>

constexpr int COLS = 10;
constexpr int ROWS = 20;
constexpr int BLOCK_SIZE = 30;
constexpr float FALL_INTERVAL = 0.5f;

// ✅ Ruta relativa (funciona en Windows, Linux y macOS)
constexpr const char* FONT_PATH = "fonts/game_font.ttf";

inline const sf::Color PIECE_COLORS[7] = {
    sf::Color::Cyan, sf::Color::Yellow, sf::Color::Magenta, sf::Color::Green,
    sf::Color::Red, sf::Color::Blue, sf::Color(255, 165, 0)
};
```
En src/Game.cpp, simplifica loadFont():

```cpp
bool Game::loadFont() {
    if (font.loadFromFile(FONT_PATH)) {
        m_fontLoaded = true;
        return true;
    }
    std::cerr << "Error: No se pudo cargar '" << FONT_PATH << "'\n";
    return false;
}
```
## Paso 1: Instalar SFML para Windows
- Ve a [SFML Download](https://www.sfml-dev.org/download/sfml/2.6.0/?spm=a2ty_o01.29997173.0.0.32e155fbBiDMVH)
- Descarga: [SFML-2.6.0-windows-vc17-64-bit.zip](SFML-2.6.0-windows-vc17-64-bit.zip) (para Visual Studio 2022)
- Extrae en `C:\SFML` (o donde prefieras)
   - Verifica que existan: `C:\SFML\bin\`, `C:\SFML\lib\`, `C:\SFML\lib\cmake\SFML\`

## 🛠️ Paso 2: Configurar y compilar con CMake
- Copia toda la carpeta `tetris/` desde WSL a Windows (ej. `C:\Users\TuNombre\Documents\tetris`)
- Abre "x64 Native Tools Command Prompt for VS 2022" (busca en Inicio)
- Ejecuta:
```cmd
cd C:\Users\TuNombre\Documents\tetris
mkdir build && cd build
cmake .. -DSFML_DIR="C:/SFML/lib/cmake/SFML" -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```
- Se generará: build\Release\tetris.exe

(Si usas VS Code + CMake Tools, simplemente abre la carpeta, configura el kit `Visual Studio 2022 Release x64` y compila. CMake detectará SFML automáticamente si `SFML_DIR` está en variables de entorno o usas `-DSFML_DIR`)

##  Paso 3: Empaquetar para distribución
Un .exe de SFML no funciona solo. Necesitas las DLLs de tiempo de ejecución.
- Crea una carpeta Tetris_Windows\
- Copia dentro:
  - build\Release\tetris.exe
  - fonts\game_font.ttf
  - Desde C:\SFML\bin\ copia:
    - sfml-system-2.dll
    - sfml-window-2.dll
    - sfml-graphics-2.dll
    - freetype-6.dll (o libfreetype-6.dll, depende de la versión)
- Opcional pero recomendado: Si tu PC no tiene las redistribuibles de VS, añade vcruntime140.dll y msvcp140.dll (suelen estar en `C:\Windows\System32\`).

Estructura final:
```bash
Tetris_Windows/
├── tetris.exe
├── fonts/
│   └── game_font.ttf
├── sfml-system-2.dll
├── sfml-window-2.dll
├── sfml-graphics-2.dll
└── freetype-6.dll
```
Haz doble clic en tetris.exe. Deberá abrirse la ventana y funcionar perfectamente fuera de WSL/VS.

## Extra: Script de empaquetado automático (package.bat)
Guarda esto en la raíz del proyecto y ejecútalo tras compilar:
```bat
@echo off
set OUT=Tetris_Windows
if exist %OUT% rmdir /s /q %OUT%
mkdir %OUT%
copy build\Release\tetris.exe %OUT%\
mkdir %OUT%\fonts
copy fonts\game_font.ttf %OUT%\fonts\
copy C:\SFML\bin\sfml-system-2.dll %OUT%\
copy C:\SFML\bin\sfml-window-2.dll %OUT%\
copy C:\SFML\bin\sfml-graphics-2.dll %OUT%\
copy C:\SFML\bin\freetype-6.dll %OUT%\ 2>nul
echo ✅ Paquete listo en %OUT%\
pause
```
## ¿Listo para compartir?

- Comprime Tetris_Windows/ en .zip
- Funciona en cualquier Windows 10/11 de 64 bits sin instalar nada
- No requiere WSL, Linux, ni Python

¿Quieres que añada ahora un instalador .exe con Inno Setup para que los usuarios lo instalen como cualquier programa, o prefieres optimizar rendimiento/gráficos primero? 

