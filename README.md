# ModEx: moderación del extremismo de opiniones (ADA II)

Proyecto 1 de Análisis y Diseño de Algoritmos II en C++17, Qt 6 Widgets y CMake. Resuelve el problema ModEx con fuerza bruta, un algoritmo voraz y programación dinámica.

## Archivos

| Archivo | Descripción |
|---|---|
| `CMakeLists.txt` | Construcción: librería `modex_core` y ejecutables `ModEx` y `pruebas` |
| `.gitignore` | Excluye `build*/`, `compile_commands.json`, `.cache/` y `*.user` |
| `.clang-tidy` | Configuración de clang-tidy |
| `include/agente.hpp` | Clase `Agente` (opinión, receptividad, influencia) |
| `include/red.hpp` | Clase `Red` (extremismo), `techo`, `costoModerar`, `esfuerzo`, lectura/escritura de archivos y generador de casos |
| `include/resultado.hpp` | Struct `Resultado`: estrategia E, esfuerzo y extremismo |
| `include/mainwindow.hpp` | Ventana principal (Qt) |
| `include/estrategias/fuerzabruta.hpp` | Clase `FuerzaBruta` y `modexFB` |
| `include/estrategias/voraz.hpp` | `modexV` |
| `include/estrategias/dinamica.hpp` | `modexPD` |
| `src/main.cpp` | Punto de entrada de la aplicación gráfica |
| `src/mainwindow.cpp` | Interfaz: abrir, generar casos, ejecutar y comparar algoritmos, verificar y guardar |
| `src/agente.cpp` | Implementación de `Agente` |
| `src/red.cpp` | Implementación de `Red`, esfuerzo, lectura/escritura de archivos y generador |
| `src/estrategias/fuerzabruta.cpp` | Fuerza bruta: recorre las 2^n estrategias, O(n·2^n) |
| `src/estrategias/voraz.cpp` | Voraz por razón beneficio/costo, O(n log n) |
| `src/estrategias/dinamica.cpp` | Programación dinámica tipo mochila 0/1, O(n·A) con A = min(R_max, Σcostos) |
| `pruebas/pruebas.cpp` | Verifica los ejemplos del enunciado, casos fijos y 2000 casos aleatorios (PD = FB), mide la calidad del voraz y los tiempos |
| `casos/ejemplo1.txt`, `casos/ejemplo2.txt` | Ejemplos 2.3.1 y 2.3.2 del enunciado en el formato de entrada |

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

## Uso de la interfaz

- **Abrir**: carga una red desde un archivo de texto.
- **Generar caso**: crea una red aleatoria (n y R_max a elección), la guarda en un archivo y la carga.
- **Fuerza bruta / Voraz / Dinamica**: ejecuta el algoritmo; la columna correspondiente de la tabla muestra E.
- **Comparar**: ejecuta los tres (fuerza bruta solo si n ≤ 25).
- La tabla de resultados muestra Ext, Esf, si Esf ≤ R_max, si el recálculo de Esf y Ext a partir de E coincide, el tiempo y si el resultado es óptimo comparado con la programación dinámica.
- **Guardar salida**: guarda la salida del algoritmo seleccionado en la tabla de resultados.

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
