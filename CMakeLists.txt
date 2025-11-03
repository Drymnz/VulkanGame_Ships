cmake_minimum_required(VERSION 3.20)
project(VulkanGame)
set(CMAKE_CXX_STANDARD 17)

# Incluir directorios
include_directories(include)
include_directories(external/imgui)
include_directories(external/imgui/backends)
include_directories(external)
find_package(nlohmann_json REQUIRED)

# Definir directorio de ImGui
set(IMGUI_DIR external/imgui)

# Archivos fuente organizados por categoría
set(SOURCES_CORE
    src/core/VulkanInstance.cpp
    src/core/VulkanDevice.cpp
    src/core/FPSController.cpp
    src/core/GameLoop.cpp
    src/core/WindowManager.cpp
)

set(SOURCES_GRAPHICS
    src/graphics/VulkanSwapChain.cpp
    src/graphics/VulkanRenderPass.cpp
    src/graphics/VulkanPipeline.cpp
    src/graphics/VulkanRenderer.cpp
    src/graphics/BufferManager.cpp
    src/graphics/CommandManager.cpp
    src/graphics/FramebufferManager.cpp
    src/graphics/ShaderManager.cpp
    src/graphics/SyncManager.cpp
    src/graphics/ModelLoader.cpp
    src/graphics/Transform.cpp
)

set(SOURCES_GAME
    src/game/GameObject.cpp
)

set(SOURCES_INPUT
    src/input/InputManager.cpp
)

set(SOURCES_MAIN
    src/main.cpp
    src/VulkanApp.cpp
)

# ImGui sources
set(SOURCES_IMGUI
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

# Combinar todas las fuentes
set(ALL_SOURCES
    ${SOURCES_CORE}
    ${SOURCES_GRAPHICS}
    ${SOURCES_GAME}
    ${SOURCES_INPUT}
    ${SOURCES_MAIN}
    ${SOURCES_IMGUI}
)

# Crear ejecutable
add_executable(juego ${ALL_SOURCES})

# Enlazar bibliotecas
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(juego 
    Vulkan::Vulkan 
    glfw 
    nlohmann_json::nlohmann_json
)

# Copiar shaders al directorio de build
file(COPY shaders/vert.spv DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shaders/frag.spv DESTINATION ${CMAKE_BINARY_DIR})

# Opcional: Copiar assets si existen
if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
    file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
endif()

# Opcional: Mensaje de ayuda
message(STATUS "==============================================")
message(STATUS "VulkanGame - Build Configuration")
message(STATUS "==============================================")
message(STATUS "Core files:      ${SOURCES_CORE}")
message(STATUS "Graphics files:  ${SOURCES_GRAPHICS}")
message(STATUS "Game files:      ${SOURCES_GAME}")
message(STATUS "Input files:     ${SOURCES_INPUT}")
message(STATUS "==============================================")