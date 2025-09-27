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

This project is designed to meet all Udacity C++ Nanodegree rubric criteria through strategic architectural choices. Each decision below maps to specific rubric requirements:

### 1. **Loops, Functions, and I/O Requirements** (Target: 2+ criteria)

#### **Control Structures and Function Organization**
- **Multiple Control Structures**: Implement various loops (for, while), conditionals (if/else, switch), and range-based loops throughout the codebase
- **Function Decomposition**: Break down complex operations into smaller, well-named functions with single responsibilities
- **Function Overloading**: Use overloaded functions for similar operations with different parameter types

#### **File I/O Integration**
- **High Score Persistence**: Read/write high scores to external file (`scores.txt`)
- **Configuration Loading**: Load game settings from configuration file (`config.json`)
- **Game State Saving**: Implement save/load functionality for game progress

#### **Enhanced User Input**
- **Extended Controls**: Support additional inputs beyond basic movement (pause, restart, speed adjustment)
- **Configuration Options**: Allow runtime adjustment of game parameters via user input

### 2. **Object-Oriented Programming Requirements** (Target: 3+ criteria)

#### **Class Design with Access Specifiers**
- **Explicit Visibility**: All data members marked as `public`, `private`, or `protected`
- **Encapsulation**: Private data members with public getter/setter methods where appropriate
- **Interface Abstraction**: Public methods expose only necessary functionality

#### **Constructor Best Practices**
- **Member Initialization Lists**: All constructors use member initialization lists for all member variables
- **RAII Compliance**: Constructors acquire resources, destructors release them

#### **Inheritance Hierarchy**
- **Base Entity Class**: Create abstract `Entity` base class for `Snake` and potential `Food` classes
- **Virtual Functions**: Use virtual methods for polymorphic behavior (`Update()`, `Render()`)
- **Override Specification**: Use `override` keyword for derived class virtual function implementations

#### **Template Implementation**
- **Generic Containers**: Use templated classes for game components (e.g., `GameBoard<T>`)
- **Function Templates**: Create template functions for common operations (collision detection, coordinate conversion)

### 3. **Memory Management Requirements** (Target: 3+ criteria)

#### **Reference Usage**
- **Pass-by-Reference**: Use const references for large objects in function parameters
- **Reference Returns**: Return references from getter methods where appropriate
- **Reference Variables**: Use reference variables to avoid unnecessary copying

#### **RAII and Scope Management**
- **Automatic Resource Management**: Use stack allocation and RAII for automatic cleanup
- **Scope-Based Lifetime**: Design object lifetimes to match scope requirements
- **Exception Safety**: Ensure resources are properly released even during exceptions

#### **Smart Pointer Implementation**
- **Unique Ownership**: Use `std::unique_ptr` for exclusive resource ownership (e.g., game entities)
- **Shared Resources**: Use `std::shared_ptr` for resources with multiple owners (e.g., textures, sounds)
- **Weak References**: Use `std::weak_ptr` to break circular dependencies where needed

#### **Move Semantics**
- **Move Constructors**: Implement move constructors for performance-critical classes
- **Move Assignment**: Provide move assignment operators where beneficial
- **Perfect Forwarding**: Use `std::move` and `std::forward` to avoid unnecessary copies

#### **Rule of Five Compliance**
- **Complete Implementation**: For classes managing resources, implement all five special member functions
- **Consistent Behavior**: Ensure copy/move operations maintain class invariants

### 4. **Concurrency Requirements** (Target: 2+ criteria)

#### **Multithreading Architecture**
- **Separate Render Thread**: Run rendering operations on dedicated thread for smooth graphics
- **Input Processing Thread**: Handle user input on separate thread to prevent blocking
- **Game Logic Thread**: Keep game state updates on main thread with proper synchronization

#### **Thread Synchronization**
- **Mutex Protection**: Use `std::mutex` and `std::lock_guard` to protect shared game state
- **Atomic Operations**: Use `std::atomic` for simple shared variables (scores, flags)
- **Condition Variables**: Implement `std::condition_variable` for thread coordination (pause/resume functionality)

#### **Asynchronous Operations**
- **Promise/Future Pattern**: Use `std::promise`/`std::future` for communication between threads
- **Async File Operations**: Perform file I/O operations asynchronously using `std::async`

### 5. **Implementation Strategy**

#### **Development Phases**
1. **Phase 1**: Establish core OOP structure with proper access specifiers and inheritance
2. **Phase 2**: Implement file I/O and enhanced user input features
3. **Phase 3**: Add smart pointers and move semantics for memory management
4. **Phase 4**: Introduce multithreading and synchronization mechanisms

#### **Code Quality Standards**
- **Const Correctness**: Use `const` wherever possible for immutable data
- **Exception Safety**: Implement proper exception handling with RAII
- **Documentation**: Clear function and class documentation explaining behavior
- **Naming Conventions**: Consistent, descriptive naming for all identifiers

#### **Testing and Validation**
- **Rubric Compliance**: Each implemented feature must map to specific rubric criteria
- **Performance Testing**: Verify that concurrency improvements actually enhance performance
- **Memory Leak Detection**: Use tools to ensure proper memory management

This architectural foundation ensures that the final implementation will systematically address all rubric requirements while maintaining clean, maintainable, and efficient code.

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