# Underwater Dream

Underwater Dream is a basic match-three puzzle game developed in **C** using the **Raylib** library.

## Features
- Basic match-3 mechanics (vertical and horizontal matching).
- Multiple levels with specific target scores and move limits.
- Background music, sound effects, and simple UI animations.
- Cross-platform build support via CMake.

## Prerequisites
To build and run this project, you will need:
- A C Compiler (GCC, Clang, or MSVC)
- [CMake](https://cmake.org/) (version 3.15 or higher)
- Internet connection (CMake automatically downloads Raylib during the build process)

## Getting Started

### 1. Clone the Repository
```bash
git clone https://github.com/suleymankara/underwaterDream.git
cd underwaterDream
```

### 2. Build the Game
Configure and construct the executables using CMake:
```bash
# Create build directory and setup files
cmake -S . -B build

# Compile the game
cmake --build build
```

### 3. Play
Run the executable from the build folder. The game automatically copies the `assets` folder inside so everything runs smoothly!
- **macOS / Linux**: `./build/BlockBlast`
- **Windows**: `build\Debug\BlockBlast.exe` (or `build\Release\BlockBlast.exe`)

## Project Structure
```text
underwaterDream/
├── CMakeLists.txt      # Build configuration (Fetches Raylib, copies assets)
├── README.md           # Project documentation
├── assets/             # External game assets (Audio files, Textures)
│   ├── audio/
│   └── textures/
└── src/                # Source code files
    ├── main.c          # Application entry point
    ├── definitions.c   # Implementations for game logic, UI, and algorithms
    └── definitions.h   # Globals, data structures, and function declarations
```

## Resources & Acknowledgements
- [Raylib Output & Cheatsheet](https://www.raylib.com/cheatsheet/cheatsheet.html)
