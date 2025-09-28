# Snake Game in C++

A classic Snake game implementation built with C++ and SDL2. This project showcases modern C++ programming techniques.

<img src="snake_game.gif"/>

This Snake game demonstrates object-oriented programming principles, memory management, multi-threading, and real-time graphics rendering. The implementation features smooth gameplay, collision detection, and score tracking using C++ and the SDL2 graphics library.

## Features

This enhanced Snake game includes several advanced features beyond the classic gameplay:

### High Score System
The game features a complete high score tracking system where players can enter their names and save their scores permanently. When you start the game, you'll be prompted to enter your name using the keyboard. After each game, your score is automatically saved to a file, and you can view the top 10 high scores with player names and timestamps. The system validates player names and handles file persistence across game sessions.

### Obstacle System
Dynamic obstacles add challenge and complexity to the gameplay. The game includes two types of obstacles:
- **Fixed Obstacles**: Brown squares that appear at random locations and disappear after 12 seconds
- **Moving Obstacles**: Orange squares that move in various patterns (linear, circular, zigzag, random walk) and disappear after 7 seconds

### Controls
- **Arrow Keys**: Control snake movement (up, down, left, right)
- **Text Input**: Enter player name at game start (alphanumeric characters, spaces, underscores, dashes)
- **Enter**: Confirm name input or restart game
- **Backspace**: Edit name during input
- **Escape**: Cancel name input or exit game
- **R Key**: Restart game from game over screen
- **Window Close (X)**: Exit game at any time

The game automatically transitions between states: name entry → playing → game over → high scores display.

## Dependencies for Running Locally
* cmake >= 3.7
  * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1 (Linux, Mac), 3.81 (Windows)
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* SDL2 >= 2.0
  * All installation instructions can be found [here](https://wiki.libsdl.org/Installation)
  >Note that for Linux, an `apt` or `apt-get` installation is preferred to building from source.
* SDL2_ttf
  * Linux: `sudo apt-get install libsdl2-ttf-dev`
  * Mac: `brew install sdl2_ttf`
  * Windows: Download development libraries from [SDL2_ttf releases](https://github.com/libsdl-org/SDL_ttf/releases) 
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools](https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* pkg-config
  * Linux: `sudo apt-get install pkg-config` (usually pre-installed)
  * Mac: `brew install pkg-config` (usually installed with other dev tools)
  * Windows: Install via MSYS2/MinGW or use vcpkg for dependency management

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory in the top level directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./SnakeGame`.

## Addressed Rubric Points

This project addresses all required criteria for the Udacity C++ Nanodegree capstone project, with many criteria addressed multiple times across different components.

### Loops, Functions, and I/O

**The project demonstrates an understanding of C++ functions and control structures**
- Various control structures used throughout: `for` loops in collision detection, `while` loops in game state management, `switch` statements in movement patterns and event handling
- Function decomposition in obstacle management system and collision detection utilities

**The project reads data from a file and process the data, or the program writes data to a file**
- High score system reads from and writes to `scores.txt` using `std::ifstream` and `std::ofstream`
- CSV format parsing and generation for persistent score storage

**The project accepts user input and processes the input**
- Enhanced input system beyond basic arrow keys: text input for player names with real-time validation
- Multiple input modes: gameplay controls, text entry, and game state navigation

**The project uses data structures and immutable variables**
- STL containers: `std::vector` for obstacle storage, score entries
- `const` and `constexpr` used throughout for immutable data (colors, default values)

### Object-Oriented Programming

**One or more classes are added to the project with appropriate access specifiers for class members**
- `HighScoreManager`, `ScoreEntry` classes with explicit `public`/`private` sections
- `Obstacle` base class with `protected` members for derived class access
- `FixedObstacle`, `MovingObstacle` derived classes with proper encapsulation

**Class constructors utilize member initialization lists**
- All constructors use initialization lists: `HighScoreManager`, `Obstacle`, `AsyncObstacleGenerator`
- Performance optimization and proper const member initialization

**Classes abstract implementation details from their interfaces**
- `ObstacleManager` hides internal container details, provides clean public interface
- `ThreadedObstacleManager` abstracts threading complexity from game logic

**Overloaded functions allow the same function to operate on different parameters**
- `Controller::HandleTextInput()` overloaded for different parameter sets
- `CollisionDetector` static methods overloaded for various collision scenarios

**Classes follow an appropriate inheritance hierarchy with virtual and override functions**
- `Obstacle` abstract base class with pure virtual methods (`Update()`, `Render()`, `GetType()`)
- `FixedObstacle` and `MovingObstacle` inherit and override virtual functions
- `ObstacleManager` and `ThreadedObstacleManager` inheritance with virtual method overrides

### Memory Management

**The project makes use of references in function declarations**
- `const` references used throughout: `const Snake&`, `const std::string&`, `const ObstacleManager&`
- Avoids unnecessary copying while maintaining const correctness

**The project uses destructors appropriately**
- RAII pattern implemented in all resource-managing classes
- `ThreadedObstacleManager` destructor properly shuts down background threads
- TTF font resources automatically cleaned up

**The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate**
- Smart pointers manage dynamic memory: `std::unique_ptr<Obstacle>` for polymorphic storage
- File handles automatically closed through RAII
- Thread synchronization objects properly managed

**The project follows the Rule of 5**
- `ScoreEntry` and `HighScoreManager` implement complete Rule of Five
- Move semantics implemented for performance optimization
- Copy constructors and assignment operators properly defined or deleted

**The project uses move semantics to move data instead of copying it, where possible**
- Move constructors in obstacle management classes
- `std::unique_ptr` transfers ownership efficiently
- Perfect forwarding in template constructors

### Concurrency

**The project uses multithreading**
- Dedicated background thread for obstacle lifetime management in `ThreadedObstacleManager`
- Thread pool implementation in `AsyncObstacleGenerator` for parallel processing

**A promise and future is used in the project**
- `std::async` with `std::future` for non-blocking obstacle generation
- `std::promise`/`std::future` pattern in async obstacle validation

**A mutex or lock is used in the project**
- `std::shared_mutex` for reader-writer access patterns in obstacle container
- `std::mutex` for task queue protection in thread pool
- Thread-safe collision detection and rendering operations

**A condition variable is used in the project**
- `std::condition_variable` for efficient thread coordination
- Worker thread sleep/wake coordination in obstacle lifetime management
- Task queue notification system in async generation

### Additional Advanced Features

**Template Functions and Generic Programming**
- Template methods for type-safe obstacle counting and collision detection
- Generic movement validation and path optimization algorithms

**Exception Safety and Error Handling**
- Comprehensive exception handling in file I/O operations
- Exception-safe resource management throughout

**Performance Optimization**
- Atomic operations for lock-free lifetime management
- Early exit optimization in collision detection
- Background processing maintains 60 FPS target

**Modern C++17 Features**
- Smart pointers, auto keyword, range-based for loops
- Template metaprogramming and perfect forwarding
- Structured bindings and constexpr where applicable

## CC Attribution-ShareAlike 4.0 International


Shield: [![CC BY-SA 4.0][cc-by-sa-shield]][cc-by-sa]

This work is licensed under a
[Creative Commons Attribution-ShareAlike 4.0 International License][cc-by-sa].

[![CC BY-SA 4.0][cc-by-sa-image]][cc-by-sa]

[cc-by-sa]: http://creativecommons.org/licenses/by-sa/4.0/
[cc-by-sa-image]: https://licensebuttons.net/l/by-sa/4.0/88x31.png
[cc-by-sa-shield]: https://img.shields.io/badge/License-CC%20BY--SA%204.0-lightgrey.svg
