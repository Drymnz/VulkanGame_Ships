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

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/VulkanApp.cpp
    src/core/VulkanInstance.cpp
    src/core/VulkanDevice.cpp
    src/core/FPSController.cpp
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
    src/input/InputManager.cpp
    # ImGui core
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    # ImGui backends
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

# Crear ejecutable
add_executable(juego ${SOURCES})

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