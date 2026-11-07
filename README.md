# ModEx: moderación del extremismo de opiniones (ADA II)

Proyecto 1 de Análisis y Diseño de Algoritmos II en C++17, Qt 6 Widgets y CMake. Resuelve el problema ModEx con fuerza bruta, un algoritmo voraz y programación dinámica.

## Archivos

| Archivo | Descripción |
|---|---|
| `CMakeLists.txt` | Construcción: librería `modex_core` y ejecutables `ModEx` y `pruebas` |
| `.gitignore` | Excluye `build*/`, `compile_commands.json`, `.cache/` y `*.user` |
| `.clang-tidy` | Configuración de clang-tidy |
| `include/agente.hpp` | Clase `Agente` (opinión, receptividad, influencia) |
| `include/red.hpp` | Clase `Red` (extremismo), `leerRed` y `escribirSalida` |
| `include/resultado.hpp` | Struct `Resultado`: estrategia E, esfuerzo y extremismo |
| `include/mainwindow.hpp` | Ventana principal (Qt) |
| `include/estrategias/fuerzabruta.hpp` | `techo`, `esfuerzo`, `generarEstrategias`, `FuerzaBruta` y `modexFB` |
| `include/estrategias/voraz.hpp` | `modexV` |
| `include/estrategias/dinamica.hpp` | `modexPD` |
| `src/main.cpp` | Punto de entrada de la aplicación gráfica |
| `src/mainwindow.cpp` | Interfaz: abrir, ejecutar algoritmos, verificar y guardar |
| `src/agente.cpp` | Implementación de `Agente` |
| `src/red.cpp` | Implementación de `Red`, lectura y escritura de archivos |
| `src/estrategias/fuerzabruta.cpp` | Algoritmo de fuerza bruta |
| `src/estrategias/voraz.cpp` | Algoritmo voraz |
| `src/estrategias/dinamica.cpp` | Algoritmo de programación dinámica |
| `pruebas/pruebas.cpp` | Ejecuta los 9 casos de prueba con fuerza bruta e imprime Ext, Esf y E |

## Requisitos

CMake ≥ 3.21, un compilador con C++17 y Qt ≥ 6.3 (Widgets y Concurrent).

| Sistema | Instalación |
|---|---|
| Arch Linux | `sudo pacman -S qt6-base cmake` |
| Ubuntu | `sudo apt install qt6-base-dev cmake g++` |
| Windows | Instalador online de Qt (componente MinGW o MSVC) y CMake |
| macOS | `brew install qt cmake` |

## Compilar

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

En Windows hay que indicar dónde está Qt:

```bat
cmake -B build -DCMAKE_PREFIX_PATH=<ruta Qt>
cmake --build build --config Release
```

## Ejecutar

| Sistema | Comando |
|---|---|
| Linux | `./build/ModEx` (interfaz) y `./build/pruebas` (consola) |
| Windows | `build\Release\ModEx.exe` (MSVC) o `build\ModEx.exe` (MinGW) |
| macOS | `open build/ModEx.app` |

## Empaquetar

| Sistema | Comando |
|---|---|
| Windows | `windeployqt` sobre la ruta del binario (`build\Release\ModEx.exe` o `build\ModEx.exe`) |
| macOS | `macdeployqt build/ModEx.app -dmg` |
| Linux | Se usa el binario directamente con el Qt del sistema |

## Formato de entrada

```
n
op0,recep0,inf0
...
op(n-1),recep(n-1),inf(n-1)
R_max
```

Acepta como separadores `,`, `;`, espacios y `. ` (punto seguido de espacio). También acepta finales de línea CRLF y líneas vacías. Si el formato es inválido, el programa lo informa con un error.

## Formato de salida

```
Ext
Esf
mod0
...
mod(n-1)
```

`modi` vale 1 si el agente i fue moderado y 0 si no.
