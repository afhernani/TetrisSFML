cmake_minimum_required(VERSION 3.10)
project(TetrisSFML VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Estructura de directorios
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Fuentes del proyecto
set(SOURCES
    ${SRC_DIR}/SoundManager.cpp
    ${SRC_DIR}/Piece.cpp
    ${SRC_DIR}/Board.cpp
    ${SRC_DIR}/Game.cpp
    main.cpp
)

# Crear ejecutable
add_executable(tetris ${SOURCES})

# Incluir directorios de cabeceras
target_include_directories(tetris PRIVATE ${INCLUDE_DIR})

# Enlazar con SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
target_link_libraries(tetris PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# Copiar sonidos al directorio de compilación
file(COPY ${CMAKE_SOURCE_DIR}/sounds DESTINATION ${CMAKE_BINARY_DIR})
# Instalar (opcional)
install(TARGETS tetris DESTINATION bin)