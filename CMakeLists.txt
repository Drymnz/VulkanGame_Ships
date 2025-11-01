cmake_minimum_required(VERSION 3.20)
project(VulkanGame)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(juego src/main.cpp)

target_link_libraries(juego 
    glfw
    glm::glm
    Vulkan::Vulkan
)
