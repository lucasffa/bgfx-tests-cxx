cmake_minimum_required(VERSION 3.20)
project(BgfxRectangles)

# Configuração do padrão C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------------------------------
# 1) Configuração para SDL2 em Windows e Linux
# --------------------------------------------------------------------------
if(WIN32)
    # Caminhos específicos para Windows
    set(SDL2_INCLUDE_DIRS "C:/dev/sdl/include")
    set(SDL2_LIBRARIES
        "C:/dev/sdl/lib/x64/SDL2.lib"
        "C:/dev/sdl/lib/x64/SDL2main.lib"
    )
elseif(UNIX)
    # Usar o sistema para localizar SDL2 em Linux
    find_package(SDL2 REQUIRED)
    set(SDL2_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
    set(SDL2_LIBRARIES ${SDL2_LIBRARIES})
endif()

# --------------------------------------------------------------------------
# 2) Adicionar bgfx.cmake
# --------------------------------------------------------------------------
add_subdirectory(libs/bgfx.cmake)

# --------------------------------------------------------------------------
# 3) Criar o executável principal
# --------------------------------------------------------------------------
add_executable(${PROJECT_NAME} src/main.cpp)

# --------------------------------------------------------------------------
# 4) Inclui diretórios de SDL2, bx, bimg e bgfx
# --------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${SDL2_INCLUDE_DIRS}    # SDL2
    libs/bx/include         # bx
    libs/bimg/include       # bimg
    libs/bgfx/include       # bgfx
)

# --------------------------------------------------------------------------
# 5) Definir macros específicas de plataforma
# --------------------------------------------------------------------------
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DWIN32)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DLINUX)
endif()

# --------------------------------------------------------------------------
# 6) Linkar as bibliotecas (bgfx, bimg, bx, SDL2)
# --------------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
    bgfx
    bimg
    bx
    ${SDL2_LIBRARIES}
)

# --------------------------------------------------------------------------
# 7) Mensagem para o desenvolvedor
# --------------------------------------------------------------------------
message(STATUS "SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}")
message(STATUS "SDL2_LIBRARIES: ${SDL2_LIBRARIES}")
