```markdown
# 🌀 Vulkan Game Framework

Pequeño framework base en **C++17** para crear aplicaciones o juegos usando **Vulkan** y **GLFW**.  
El objetivo es ofrecer una estructura modular, clara y extensible para futuros proyectos.

---

## 📁 Estructura del proyecto

```

├── src/
│   ├── core/
│   │   ├── VulkanInstance.cpp
│   │   ├── VulkanDevice.cpp
│   │   ├── WindowManager.cpp
│   │   └── ...
│   ├── game/
│   │   └── GameObject.cpp
│   ├── VulkanApp.cpp
│   └── main.cpp
│
├── include/
│   ├── core/
│   │   ├── VulkanInstance.h
│   │   ├── VulkanDevice.h
│   │   ├── WindowManager.h
│   │   └── ...
│   ├── game/
│   │   └── GameObject.h
│   └── VulkanApp.h
│
├── CMakeLists.txt
└── README.md

````

---

## ⚙️ Dependencias

El proyecto usa las siguientes librerías externas:

| Librería | Uso | Instalación (Arch Linux) |
|-----------|------|--------------------------|
| **Vulkan SDK** | API principal de renderizado | `sudo pacman -S vulkan-devel` |
| **GLFW** | Creación de ventana y manejo de entrada | `sudo pacman -S glfw` |
| **glm** | Matemática 3D (opcional) | `sudo pacman -S glm` |
| **CMake** | Sistema de build multiplataforma | `sudo pacman -S cmake` |

---

## 🧩 Componentes principales

### 🪟 `WindowManager`
Encapsula toda la gestión de la ventana y la inicialización de **GLFW** con soporte para **Vulkan**.

### ⚡ `VulkanInstance`
Crea y destruye la instancia de Vulkan (`VkInstance`), configurando extensiones y capas requeridas.

### 💠 `VulkanDevice`
Selecciona la GPU física, crea el dispositivo lógico y obtiene las colas (`graphics` y `present`).

### 🎮 `GameObject`
Clase base para representar objetos en el juego, con su propio `Transform` y lógica de actualización.

### 🧱 `VulkanApp`
Clase principal que coordina todo: ventana, instancia, dispositivo, bucle principal, etc.

---

## 🛠️ Compilación

### 1. Clonar el repositorio
```bash
git clone https://github.com/tuusuario/vulkan-game-framework.git
cd vulkan-game-framework
````

### 2. Crear carpeta de build

```bash
mkdir build && cd build
```

### 3. Ejecutar CMake

```bash
cmake ..
```

### 4. Compilar

```bash
make -j$(nproc)
```

### 5. Ejecutar

```bash
./VulkanApp
```

---

## 🧱 Estructura modular

Cada módulo está diseñado para ser reemplazable o ampliable.
Por ejemplo, puedes cambiar `WindowManager` por una versión SDL2, o añadir soporte a ImGui.

```
core/
 ├── VulkanInstance.h/.cpp   → Inicializa Vulkan
 ├── VulkanDevice.h/.cpp     → GPU + Logical Device
 ├── WindowManager.h/.cpp    → Ventana GLFW
 └── ...
game/
 ├── GameObject.h/.cpp       → Entidades del juego
 └── ...
```

---

## 🧩 TODO / Roadmap

* [ ] Manejo de Swapchain (buffers de pantalla)
* [ ] Sincronización con semáforos y fences
* [ ] Sistema de render por componentes
* [ ] Integración con ImGui para debugging
* [ ] Soporte multiplataforma (Windows / Linux / macOS)

---

## 📜 Licencia

MIT License © 2025
Desarrollado por **Benjamin Aguilar**
