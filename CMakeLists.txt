cmake_minimum_required(VERSION 3.21)
project(ModEx CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.3 REQUIRED COMPONENTS Widgets Concurrent)
qt_standard_project_setup()

if(MSVC)
    set(ADVERTENCIAS /W4 /permissive-)
else()
    set(ADVERTENCIAS -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wnull-dereference -Wold-style-cast)
endif()

add_library(modex_core STATIC
    src/agente.cpp
    src/red.cpp
    src/estrategias/fuerzabruta.cpp
    src/estrategias/voraz.cpp
    src/estrategias/dinamica.cpp
)
target_include_directories(modex_core PUBLIC include)
target_compile_options(modex_core PRIVATE ${ADVERTENCIAS})

qt_add_executable(ModEx WIN32 MACOSX_BUNDLE
    src/main.cpp
    src/mainwindow.cpp
    include/mainwindow.hpp
)
target_link_libraries(ModEx PRIVATE modex_core Qt6::Widgets Qt6::Concurrent)
target_compile_options(ModEx PRIVATE ${ADVERTENCIAS})

add_executable(pruebas pruebas/pruebas.cpp)
target_link_libraries(pruebas PRIVATE modex_core)
target_compile_options(pruebas PRIVATE ${ADVERTENCIAS})

if(NOT CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR AND NOT CMAKE_GENERATOR MATCHES "Visual Studio")
    file(CREATE_LINK ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json RESULT enlace_resultado SYMBOLIC COPY_ON_ERROR)
endif()
