# Snake Game C++ Project Documentation

## Project Overview

This is a classic Snake game implementation built with C++ and SDL2, designed as a university capstone project for demonstrating modern C++ programming techniques. The game showcases object-oriented programming principles, memory management, multi-threading concepts, and real-time graphics rendering.

### Key Features
- Smooth gameplay with 60 FPS targett
- Real-time collision detection
- Dynamic score tracking and speed increase
- Object-oriented architecture with clear separation of concerns
- Cross-platform compatibility via SDL2

## Build Instructions

### Prerequisites
- **cmake** >= 3.7
- **make** >= 4.1 (Linux/Mac) or 3.81 (Windows)
- **SDL2** >= 2.0
- **gcc/g++** >= 5.
main.cpp
├── Game (Model/Controller4
- C++17 standard support

### Build Steps
```bash
# 1. Clone the repository
git clone <repository-url>
cd CppND-Capstone-Snake-Game-master

# 2. Create build directory
mkdir build && cd build

# 3. Configure and build
cmake .. && make

# 4. Run the game
./SnakeGame
```

### Build Configuration
The project uses CMake with the following configuration:
- **C++ Standard**: C++17
- **Compiler Flags**: `-Wall` for warnings
- **Dependencies**: SDL2 (dynamically linked)
- **Output**: `SnakeGame` executable

## Code Architecture

### High-Level Design
The project follows a **Model-View-Controller (MVC)** pattern with additional separation for rendering:

```
main.cpp
├── Game (Model/Controller
├── Renderer (View)
├── Controller (Input Handler)
└── Snake (Entity Model)
```

### Core Components

#### 1. **main.cpp** - Application Entry Point
- **Location**: `src/main.cpp:6-21`
- **Responsibilities**:
  - Initialize game constants (screen size: 640x640, grid: 32x32, 60 FPS)
  - Create and coordinate main objects (Game, Renderer, Controller)
  - Handle game termination and display final statistics

#### 2. **Game Class** - Core Game Logi timing control
- Random food placement with `std::mt19937` for consistent randomness

#### 3. **Snake Class** - Player Entity
- **Files**: `src/snake.h`, `src/snake.cpp`
- **Key Features**:
  - Direction enumeration for movement (`snake.h:9`)
  - Dynamic body growth using `std::vectorc
- **Files**: `src/game.h`, `src/game.cpp`
- **Key Responsibilities**:
  - Main game loop implementation (`game.cpp:12-50`)
  - Frame rate management and timing control
  - Food placement logic with collision avoidance (`game.cpp:52-65`)
  - Score tracking and snake growth mechanics (`game.cpp:67-84`)
  - Random number generation for food placement

**Key Design Decisions**:
- Uses composition pattern with Snake object
- Implements frame-independent movement with timing control
- Random food placement with `std::mt19937` for consistent randomness

#### 3. **Snake Class** - Player Entity
- **Files**: `src/snake.h`, `src/snake.cpp`
- **Key Features**:
  - Direction enumeration for movement (`snake.h:9`)
  - Dynamic body growth using `std::vector<SDL_Point>` (`snake.h:27`)
  - Collision detection methods
  - Smooth movement with floating-point positioning

**Design Patterns**:
- **State Pattern**: Direction enum for movement states
- **Entity Component**: Self-contained with position, size, and state

#### 4. **Controller Class** - Input Management
- **Files**: `src/controller.h`, `src/controller.cpp`
- **Responsibilities**:
  - SDL event handling and keyboard input processing
  - Direction change validation (prevents reverse direction)
  - Game exit control

#### 5. **Renderer Class** - Graphics and Display
- **Files**: `src/renderer.h`, `src/renderer.cpp`
- **Responsibilities**
  - SDL2 window and renderer management
  - Snake and food rendering with different colors
  - Window title updates with score and FPS display
  - Resource cleanup in destructor

## Architectural Decisions

### 1. **Separation of Concerns**
Each class has a single, well-defined responsibility:
- **Game**: Logic and state management
- **Snake**: Entity behavior and data
- **Controller**: Input handling only
- **Renderer**: Display and graphics only

### 2. **Resource Management**
- **RAII Pattern**: Renderer class manages SDL resources in constructor/destructor
- **Stack Allocation**: Most objects are stack-allocated for automatic cleanup
- **STL Containers**: Uses `std::vector` for dynamic snake body management

### 3. **Performance Considerations**
- **Frame Rate Control**: Fixed 60 FPS with `SDL_Delay` for consistent gameplay
- **Efficient Collision Detection**: Grid-based positioning for O(1) food collision checks
- **Memory Efficiency**: Minimal dynamic allocation, primarily for snake body growth

### 4. **Extensibility Design**
- **Modular Architecture**: Easy to extend with new features (power-ups, obstacles, etc.)
- **Clear Interfaces**: Public methods expose necessary functionality without implementation details
- **Event-Driven Input**: Controller pattern allows for easy input method changes

### 5. **Cross-Platform Compatibility**
- **SDL2 Abstraction**: Platform-independent graphics and input handling
- **CMake Build System**: Cross-platform build configuration
- **Standard C++17**: Uses only standard library features for portability

## Game Mechanics

### Core Loop (60 FPS)
1. **Input Processing**: Handle keyboard events and direction changes
2. **Game State Update**: Move snake, check collisions, handle food consumption
3. **Rendering**: Draw snake, food, and update display
4. **Timing Control**: Maintain consistent frame rate

### Snake Behavior
- **Movement**: Continuous movement based on current direction and speed
- **Growth**: Body extends when food is consumed (`snake.cpp:GrowBody()`)
- **Speed Increase**: Velocity increases by 0.02 per food item consumed (`game.cpp:82`)
- **Collision**: Game ends when snake collides with itself

### Scoring System
- **Score Calculation**: One point per food item consumed
- **Display**: Real-time score and FPS shown in window title
- **Final Statistics**: Score and snake size displayed on game termination

## File Structure
```
CppND-Capstone-Snake-Game-master/
├── src/
│   ├── main.cpp           # Application entry point
│   ├── game.h/cpp         # Core game logic and main loop
│   ├── snake.h/cpp        # Snake entity implementation
│   ├── controller.h/cpp   # Input handling
│   └── renderer.h/cpp     # SDL2 graphics rendering
├── build/                 # Build output directory
├── cmake/                 # CMake modules
├── CMakeLists.txt        # Build configuration
└── README.md             # Basic project information
```