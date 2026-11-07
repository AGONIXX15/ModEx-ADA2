ModEx - Problema de moderacion de extremismo (ADA II)
Port a C++17 + Qt 6 Widgets + CMake del proyecto en Python.

ARCHIVOS
CMakeLists.txt                     Construccion: libreria modex_core, ejecutables ModEx y pruebas.
.gitignore                         Excluye build*/, compile_commands.json, .cache/ y *.user.
.clang-tidy                        Configuracion de clang-tidy.
include/agente.hpp                 Clase Agente (opinion, receptividad, influencia).
include/red.hpp                    Clase Red (extremismo), leerRed y escribirSalida.
include/resultado.hpp              Struct Resultado: estrategia E, esfuerzo y extremismo.
include/mainwindow.hpp             Ventana principal (Qt).
include/estrategias/fuerzabruta.hpp  techo, esfuerzo, generarEstrategias, FuerzaBruta y modexFB.
include/estrategias/voraz.hpp      modexV.
include/estrategias/dinamica.hpp   modexPD.
src/main.cpp                       Punto de entrada de la aplicacion grafica.
src/mainwindow.cpp                 Interfaz: abrir, ejecutar algoritmos, verificar y guardar.
src/agente.cpp                     Implementacion de Agente.
src/red.cpp                        Implementacion de Red, lectura y escritura de archivos.
src/estrategias/fuerzabruta.cpp    Algoritmo de fuerza bruta.
src/estrategias/voraz.cpp          Algoritmo voraz.
src/estrategias/dinamica.cpp       Algoritmo de programacion dinamica.
pruebas/pruebas.cpp                Ejecuta los 9 casos de prueba con fuerza bruta e imprime Ext, Esf y E.

REQUISITOS
CMake >= 3.21, compilador C++17 y Qt >= 6.3 (Widgets y Concurrent).

INSTALACION
Linux (Arch):   sudo pacman -S qt6-base cmake
Linux (Ubuntu): sudo apt install qt6-base-dev cmake g++
Windows:        instalador Qt online (componente MinGW o MSVC) y CMake.
macOS:          brew install qt cmake

COMPILAR
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build
Windows: cmake -B build -DCMAKE_PREFIX_PATH=<ruta Qt> && cmake --build build --config Release

EJECUTAR
Linux: ./build/ModEx y ./build/pruebas
Windows: el binario queda en build\Release\ModEx.exe (MSVC) o en build\ModEx.exe (MinGW).
macOS: open build/ModEx.app

EMPAQUETAR
Windows: windeployqt sobre la ruta del binario (build\Release\ModEx.exe o build\ModEx.exe)
macOS:   macdeployqt build/ModEx.app -dmg
Linux:   se usa el binario directamente con el Qt del sistema.

FORMATO DE ENTRADA
Primera linea: n. Luego n lineas "opinion,receptividad,influencia".
Ultima linea: R_max. Se aceptan los separadores ',' ';' y espacios,
tambien ". " (punto seguido de espacio), finales de linea CRLF y lineas vacias.
Si el formato es invalido se informa un error.

FORMATO DE SALIDA
Primera linea: extremismo (Ext). Segunda linea: esfuerzo (Esf).
Luego n lineas con 0 o 1 (1 = agente moderado).
