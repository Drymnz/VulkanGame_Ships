#include "VulkanApp.h"      // Incluye la clase VulkanApp, que contiene toda la lógica de la aplicación
#include <iostream>         // Para imprimir mensajes en la consola
#include <stdexcept>        // Para manejar excepciones estándar (std::exception, std::runtime_error, etc.)

// -------------------------------------------------------
// Función principal (punto de entrada del programa)
// -------------------------------------------------------
int main()
{
    try
    {
        // Crear una instancia de la aplicación Vulkan
        // Aquí se ejecuta el constructor de VulkanApp, que inicializa los miembros principales
        VulkanApp app;

        // Ejecutar el flujo completo de la aplicación:
        // - Crea la ventana
        // - Inicializa Vulkan
        // - Ejecuta el bucle del juego (render + entrada)
        // - Limpia recursos al salir
        app.run();
    }
    // -------------------------------------------------------
    // Captura cualquier excepción lanzada dentro del bloque try
    // -------------------------------------------------------
    catch (const std::exception& e)
    {
        // Si ocurre un error, lo muestra en stderr y devuelve código de error
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;  // Retorna 1 (fallo)
    }

    // Si todo sale bien, retorna 0 (éxito)
    return EXIT_SUCCESS;
}
