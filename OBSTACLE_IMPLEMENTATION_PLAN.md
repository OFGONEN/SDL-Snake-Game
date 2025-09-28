# Obstacle System Implementation Plan

## Epic Overview
**Feature**: Fixed and Moving Obstacles for Enhanced Gameplay
**Goal**: Add static and dynamic obstacles to increase game difficulty and demonstrate advanced C++ programming concepts
**Estimated Effort**: 4-5 development sessions
**Priority**: High (addresses multiple rubric criteria with advanced OOP patterns)

## User Stories

### User Story 1: Temporary Fixed Obstacles
**As a** player
**I want to** encounter static obstacles that appear and disappear over time
**So that** I need to adapt my strategy as the playing field changes dynamically

**Acceptance Criteria:**
- [ ] Fixed obstacles spawn randomly during gameplay at timed intervals
- [ ] Obstacles are visually distinct from snake and food (different color/texture)
- [ ] Snake dies when colliding with fixed obstacles
- [ ] Food cannot spawn on obstacle locations
- [ ] Obstacles have limited lifespan (5-15 seconds) before disappearing
- [ ] New obstacles spawn continuously to maintain challenge level

### User Story 2: Temporary Moving Obstacles
**As a** player
**I want to** face dynamic moving obstacles that have limited duration
**So that** the game becomes progressively more challenging with constantly changing patterns

**Acceptance Criteria:**
- [ ] Moving obstacles patrol in predictable patterns (linear, circular, zigzag)
- [ ] Moving obstacles respect grid boundaries and wrap around screen edges
- [ ] Multiple movement patterns available for variety
- [ ] Snake dies when colliding with moving obstacles
- [ ] Moving obstacles avoid colliding with each other
- [ ] Movement speed configurable and increases with game progression
- [ ] Moving obstacles have shorter lifespan (3-10 seconds) than fixed obstacles

### User Story 3: Dynamic Obstacle Spawning
**As a** player
**I want** obstacle difficulty to increase as my score improves through more frequent spawning
**So that** the game remains challenging with constantly evolving obstacles

**Acceptance Criteria:**
- [ ] Obstacle spawn frequency increases with score progression
- [ ] Moving obstacle speed increases with progression
- [ ] Obstacle lifetime decreases at higher difficulty levels (faster turnover)
- [ ] Multiple obstacles can exist simultaneously with overlapping lifetimes
- [ ] Spawn timing becomes more unpredictable at higher scores
- [ ] Maximum simultaneous obstacle limit prevents impossible scenarios

## Technical Implementation Plan

### Sprint 1: Foundation & Base Classes (Session 1)

#### Task 1.1: Create Obstacle Base Class Hierarchy
**Rubric Criteria**: Object-Oriented Programming, Inheritance, Polymorphism
**Files**: `src/obstacle.h`, `src/obstacle.cpp`

**Required Headers and Includes:**
```cpp
#include "SDL.h"
#include <atomic>
#include <memory>
#include <thread>
#include <shared_mutex>
#include <condition_variable>
#include <future>
#include <chrono>
#include <vector>
#include <algorithm>
```

**Base Obstacle Class Design:**
```cpp
enum class ObstacleType {
    FIXED,
    MOVING
};

class Obstacle {
public:
    explicit Obstacle(int x, int y, int grid_width, int grid_height, float lifetime_seconds = 10.0f);
    virtual ~Obstacle() = default;

    // Rule of Five implementation
    Obstacle(const Obstacle& other) = default;
    Obstacle& operator=(const Obstacle& other) = default;
    Obstacle(Obstacle&& other) noexcept = default;
    Obstacle& operator=(Obstacle&& other) noexcept = default;

    // Pure virtual methods for polymorphism
    virtual void Update() = 0;
    virtual void Render(SDL_Renderer* renderer, std::size_t screen_width,
                       std::size_t screen_height, std::size_t grid_width,
                       std::size_t grid_height) const = 0;
    virtual ObstacleType GetType() const = 0;

    // Const methods for position access
    int GetX() const { return position.x; }
    int GetY() const { return position.y; }
    const SDL_Point& GetPosition() const { return position; }

    // Thread-safe lifetime management (managed by background thread)
    bool IsExpired() const { return remaining_lifetime.load() <= 0.0f; }
    float GetRemainingLifetime() const { return remaining_lifetime.load(); }
    void DecrementLifetime(float delta_time) {
        float current = remaining_lifetime.load();
        remaining_lifetime.store(std::max(0.0f, current - delta_time));
    }

    // Collision detection
    bool CollidesWithPoint(int x, int y) const;
    virtual bool CollidesWithRect(const SDL_Rect& rect) const;

protected:
    SDL_Point position;
    const int grid_width;
    const int grid_height;
    bool active{true};
    std::atomic<float> remaining_lifetime;

    // Protected helper methods
    void WrapPosition();
    bool IsValidPosition(int x, int y) const;
};
```

**Implementation Checklist:**
- [ ] Create abstract base class with proper access specifiers
- [ ] Use member initialization lists in constructor
- [ ] Implement virtual destructor and Rule of Five
- [ ] Add const methods for data access
- [ ] Create pure virtual methods for polymorphic behavior
- [ ] Use protected members for derived class access
- [ ] Add grid boundary checking and wrapping logic
- [ ] Implement thread-safe lifetime management with atomic operations
- [ ] Add default lifetime parameters (fixed: 10-15s, moving: 5-8s)
- [ ] Use std::atomic for lock-free lifetime counting

#### Task 1.2: Create FixedObstacle Class
**Rubric Criteria**: Object-Oriented Programming, Inheritance
**Files**: `src/fixed_obstacle.h`, `src/fixed_obstacle.cpp`

**FixedObstacle Class Design:**
```cpp
class FixedObstacle : public Obstacle {
public:
    explicit FixedObstacle(int x, int y, int grid_width, int grid_height,
                          float lifetime_seconds = 12.0f);

    // Override virtual methods
    void Update() override final;  // Does nothing - lifetime managed by background thread
    void Render(SDL_Renderer* renderer, std::size_t screen_width,
               std::size_t screen_height, std::size_t grid_width,
               std::size_t grid_height) const override;
    ObstacleType GetType() const override { return ObstacleType::FIXED; }

private:
    static constexpr SDL_Color kFixedObstacleColor{128, 64, 0, 255}; // Brown
    static constexpr float kDefaultLifetime = 12.0f; // 12 seconds default
};
```

**Implementation Checklist:**
- [ ] Inherit from Obstacle base class
- [ ] Use `override` keyword for all virtual method implementations
- [ ] Use `final` for methods that shouldn't be overridden further
- [ ] Implement Update() as empty method (lifetime handled by background thread)
- [ ] Use constexpr for compile-time color constants
- [ ] Add proper constructor with member initialization list
- [ ] Set default lifetime to 12 seconds for fixed obstacles

#### Task 1.3: Create MovingObstacle Class with Movement Patterns
**Rubric Criteria**: Object-Oriented Programming, Template Functions
**Files**: `src/moving_obstacle.h`, `src/moving_obstacle.cpp`

**MovingObstacle Class Design:**
```cpp
enum class MovementPattern {
    LINEAR_HORIZONTAL,
    LINEAR_VERTICAL,
    CIRCULAR,
    ZIGZAG,
    RANDOM_WALK
};

class MovingObstacle : public Obstacle {
public:
    explicit MovingObstacle(int x, int y, int grid_width, int grid_height,
                           MovementPattern pattern = MovementPattern::LINEAR_HORIZONTAL,
                           float lifetime_seconds = 7.0f);

    // Override virtual methods
    void Update() override;  // Movement only (main thread) - lifetime managed by background thread
    void Render(SDL_Renderer* renderer, std::size_t screen_width,
               std::size_t screen_height, std::size_t grid_width,
               std::size_t grid_height) const override;
    ObstacleType GetType() const override { return ObstacleType::MOVING; }

    // Movement configuration
    void SetSpeed(float speed) { this->speed = speed; }
    void SetPattern(MovementPattern pattern);
    MovementPattern GetPattern() const { return pattern; }

private:
    MovementPattern pattern;
    float speed{0.05f};
    int direction{1}; // 1 or -1 for direction changes
    float movement_counter{0.0f}; // For circular and complex patterns

    static constexpr SDL_Color kMovingObstacleColor{255, 165, 0, 255}; // Orange
    static constexpr float kDefaultLifetime = 7.0f; // 7 seconds default

    // Movement pattern implementations
    void UpdateLinearHorizontal();
    void UpdateLinearVertical();
    void UpdateCircular();
    void UpdateZigzag();
    void UpdateRandomWalk();

    // Template method for movement bounds checking
    template<typename T>
    bool IsWithinBounds(T value, T min, T max) const;
};
```

**Implementation Checklist:**
- [ ] Implement movement pattern enumeration
- [ ] Create pattern-specific update methods
- [ ] Add speed and direction management
- [ ] Use template methods for bounds checking
- [ ] Implement circular movement with trigonometry
- [ ] Add pattern switching capability
- [ ] Use proper const correctness throughout
- [ ] Set default lifetime to 7 seconds for moving obstacles
- [ ] Update() handles movement only on main thread (lifetime managed by background thread)
- [ ] Use inherited SDL_Point position for all position operations (main thread only)
- [ ] Movement pattern updates modify position.x and position.y directly

### Sprint 2: Obstacle Management System (Session 2)

#### Task 2.1: Create ObstacleManager Class
**Rubric Criteria**: Memory Management, Smart Pointers, STL Containers
**Files**: `src/obstacle_manager.h`, `src/obstacle_manager.cpp`

**ObstacleManager Class Design:**
```cpp
class ObstacleManager {
public:
    explicit ObstacleManager(int grid_width, int grid_height);
    virtual ~ObstacleManager(); // Virtual destructor for inheritance

    // Rule of Five with move semantics
    ObstacleManager(const ObstacleManager& other) = delete;
    ObstacleManager& operator=(const ObstacleManager& other) = delete;
    ObstacleManager(ObstacleManager&& other) noexcept = default;
    ObstacleManager& operator=(ObstacleManager&& other) noexcept = default;

    // Obstacle management
    void AddFixedObstacle(int x, int y, float lifetime = 12.0f);
    void AddMovingObstacle(int x, int y, MovementPattern pattern, float lifetime = 7.0f);
    void SpawnRandomObstacle(); // Single obstacle spawning
    void ClearExpiredObstacles(); // Remove expired obstacles only
    void ClearAllObstacles();

    // Update and rendering (virtual for threading override)
    virtual void UpdateObstacleMovement(); // Movement updates only
    virtual void UpdateObstacleLifetimes(float delta_time); // Synchronous lifetime updates
    virtual void RenderObstacles(SDL_Renderer* renderer, std::size_t screen_width,
                                std::size_t screen_height, std::size_t grid_width,
                                std::size_t grid_height) const;

    // Collision detection (virtual for thread-safe override)
    virtual bool CheckCollisionWithPoint(int x, int y) const;
    virtual bool CheckCollisionWithSnake(const Snake& snake) const;
    virtual bool IsValidFoodPosition(int x, int y) const;

    // Getters for game logic
    std::size_t GetObstacleCount() const { return obstacles.size(); }
    std::size_t GetFixedObstacleCount() const;
    std::size_t GetMovingObstacleCount() const;

    // Spawning configuration
    void SetSpawnRate(float obstacles_per_second);
    void SetDifficultyLevel(int level);
    void SetMovingObstacleSpeed(float speed);
    bool ShouldSpawnObstacle(float delta_time); // Check if spawn timer elapsed

private:
    const int grid_width;
    const int grid_height;

    // Basic obstacle container (base class)
    std::vector<std::unique_ptr<Obstacle>> obstacles;

    // Random number generation for obstacle placement
    std::random_device dev;
    std::mt19937 engine;
    std::uniform_int_distribution<int> random_x;
    std::uniform_int_distribution<int> random_y;

    // Configuration
    int difficulty_level{1};
    float moving_obstacle_speed{0.05f};
    float spawn_rate{0.5f}; // obstacles per second
    float spawn_timer{0.0f}; // accumulated time since last spawn


    // Helper methods
    bool IsPositionFree(int x, int y) const;
    SDL_Point GenerateRandomPosition() const;
    MovementPattern GetRandomMovementPattern() const;

    // Template method for type-specific operations
    template<typename ObstacleType>
    std::size_t CountObstaclesOfType() const;
};
```

**Implementation Checklist:**
- [ ] Use `std::vector<std::unique_ptr<Obstacle>>` for polymorphic storage
- [ ] Implement proper move semantics (delete copy, default move)
- [ ] Create basic collision detection methods for different scenarios
- [ ] Implement difficulty scaling logic
- [ ] Use template methods for type-specific counting
- [ ] Add position validation to prevent overlaps
- [ ] Implement spawn timer system with delta time tracking
- [ ] Add synchronous lifetime management methods
- [ ] Prepare virtual methods for threading extension in derived class

#### Task 2.2: Create ThreadedObstacleManager Class
**Rubric Criteria**: Inheritance, Concurrency, Polymorphism
**Files**: `src/threaded_obstacle_manager.h`, `src/threaded_obstacle_manager.cpp`

**ThreadedObstacleManager Class Design:**
```cpp
class ThreadedObstacleManager : public ObstacleManager {
public:
    explicit ThreadedObstacleManager(int grid_width, int grid_height);
    ~ThreadedObstacleManager();

    // Threading lifecycle management
    void StartLifetimeThread();
    void StopLifetimeThread();

    // Override base class methods with thread-safe versions
    void UpdateObstacleMovement() override;
    bool CheckCollisionWithPoint(int x, int y) const override;
    bool CheckCollisionWithSnake(const Snake& snake) const override;
    bool IsValidFoodPosition(int x, int y) const override;

    // Thread-safe getters
    std::size_t GetObstacleCountSafe() const;
    std::size_t GetFixedObstacleCountSafe() const;
    std::size_t GetMovingObstacleCountSafe() const;

    // Asynchronous operations
    std::future<size_t> CleanupExpiredAsync();

private:
    // Threading components
    std::thread lifetime_thread;
    std::atomic<bool> shutdown_requested{false};
    std::atomic<bool> thread_running{false};

    // Synchronization primitives
    mutable std::shared_mutex obstacles_mutex; // Reader-writer lock
    std::condition_variable lifetime_condition;
    std::mutex lifetime_mutex;

    // Thread worker methods
    void LifetimeWorkerThread();
    void UpdateAllLifetimesAtomic(float delta_time);
    void NotifyLifetimeThread();

    // Thread-safe internal operations
    void SafelyUpdateMovingObstacles();
    void SafelyCleanupExpired();

    // Atomic lifetime management implementation
    void ProcessAtomicLifetimeUpdates() {
        // Lock-free atomic operations on all obstacles
        std::shared_lock<std::shared_mutex> lock(obstacles_mutex);
        for (auto& obstacle : obstacles) {
            obstacle->DecrementLifetime(0.1f);  // 100ms intervals
        }
    }

    // Performance monitoring
    std::atomic<uint64_t> lifetime_updates_count{0};
    std::chrono::steady_clock::time_point thread_start_time;
};
```

**Implementation Checklist:**
- [ ] Inherit from ObstacleManager base class
- [ ] Use `override` keyword for all virtual method overrides
- [ ] Add `std::shared_mutex` for thread-safe container access
- [ ] Create dedicated lifetime management thread with std::thread
- [ ] Use `std::atomic<bool>` for shutdown coordination
- [ ] Implement `std::condition_variable` for efficient thread sleeping
- [ ] Add `std::async` for asynchronous expired obstacle cleanup
- [ ] Use reader-writer locks for concurrent access patterns
- [ ] Implement proper thread cleanup in destructor
- [ ] Add thread-safe versions of all collision detection methods

#### Task 2.3: Integrate ObstacleManager with Game Class
**Rubric Criteria**: Memory Management, References, Composition
**Files**: `src/game.h`, `src/game.cpp`

**Game Class Extensions:**
```cpp
class Game {
private:
    // Add threaded obstacle management
    std::unique_ptr<ThreadedObstacleManager> obstacleManager;

    // Configuration
    static constexpr float kInitialSpawnRate = 0.3f; // obstacles per second
    static constexpr float kSpawnRateIncrease = 0.1f; // increase per difficulty level
    static constexpr int kDifficultyIncreaseInterval = 5; // Every 5 points

    // Updated methods
    void PlaceFood(); // Modified to avoid obstacles
    void Update(); // Modified to check obstacle collisions
    void UpdatePlaying(const Controller& controller, const SDL_Event& event); // Add obstacle updates
    void ResetGame(); // Reset obstacles on new game

    // New methods
    void CheckObstacleCollisions();
    void UpdateDifficulty();
    void HandleObstacleSpawning(float delta_time);
    void InitializeObstacleThreads(); // Start ThreadedObstacleManager threads
    void ShutdownObstacleThreads(); // Clean shutdown
    bool IsValidFoodPosition(int x, int y) const;
};
```

**Implementation Checklist:**
- [ ] Add `std::unique_ptr<ThreadedObstacleManager>` to Game class
- [ ] Use member initialization lists in constructor
- [ ] Initialize ThreadedObstacleManager with StartLifetimeThread() in constructor
- [ ] Implement proper thread shutdown in Game destructor with StopLifetimeThread()
- [ ] Modify PlaceFood() to use thread-safe IsValidFoodPosition()
- [ ] Game loop calls obstacleManager->UpdateObstacleMovement() (inherited virtual method)
- [ ] Implement collision checking with thread-safe override methods
- [ ] Add difficulty progression based on score (spawn rate increase)
- [ ] Use const references for collision detection parameters
- [ ] Implement HandleObstacleSpawning() for timed obstacle creation
- [ ] Leverage polymorphism: Game uses base class interface, gets threaded implementation

### Sprint 3: Enhanced Collision Detection & Optimization (Session 3)

#### Task 3.1: Optimize Collision Detection System
**Rubric Criteria**: Loops, Functions, Algorithms, Performance
**Files**: `src/collision_detector.h`, `src/collision_detector.cpp`

**CollisionDetector Utility Class:**
```cpp
class CollisionDetector {
public:
    // Static utility methods for different collision types
    static bool PointToPoint(int x1, int y1, int x2, int y2);
    static bool PointToRect(int px, int py, const SDL_Rect& rect);
    static bool RectToRect(const SDL_Rect& rect1, const SDL_Rect& rect2);

    // Snake-specific collision detection
    static bool SnakeToObstacles(const Snake& snake, const ObstacleManager& obstacleManager);
    static bool SnakeHeadToPoint(const Snake& snake, int x, int y);

    // Function overloading for different parameter types
    static bool CheckCollision(const Snake& snake, const std::vector<SDL_Point>& points);
    static bool CheckCollision(const Snake& snake, const ObstacleManager& obstacleManager);
    static bool CheckCollision(int x, int y, const std::vector<SDL_Point>& obstacles);

    // Template function for generic collision checking
    template<typename Container>
    static bool CheckCollisionWithContainer(int x, int y, const Container& container);

private:
    // No instantiation - utility class only
    CollisionDetector() = delete;
    ~CollisionDetector() = delete;
    CollisionDetector(const CollisionDetector&) = delete;
    CollisionDetector& operator=(const CollisionDetector&) = delete;
};
```

**Implementation Checklist:**
- [ ] Create static utility class for collision detection
- [ ] Implement function overloading for different collision scenarios
- [ ] Add template functions for generic container collision checking
- [ ] Use various loop types (for, while, range-based) for different algorithms
- [ ] Optimize collision detection with early exit conditions
- [ ] Add comprehensive collision detection for all game entities

#### Task 3.2: Advanced Movement Patterns with Algorithms
**Rubric Criteria**: Loops, Functions, Advanced Control Structures
**Files**: `src/movement_patterns.h`, `src/movement_patterns.cpp`

**Movement Pattern Implementation:**
```cpp
namespace MovementPatterns {
    // Function overloading for different movement types
    SDL_Point CalculateLinearMovement(const SDL_Point& current, int direction, float speed);
    SDL_Point CalculateLinearMovement(const SDL_Point& current, float angle, float speed);

    SDL_Point CalculateCircularMovement(const SDL_Point& center, float radius,
                                       float angle, int grid_width, int grid_height);

    // Advanced movement with pathfinding
    std::vector<SDL_Point> CalculateZigzagPath(const SDL_Point& start, int amplitude,
                                              int wavelength, int grid_width);

    // Template function for movement validation
    template<typename MovementFunc>
    SDL_Point ValidateMovement(const SDL_Point& current, MovementFunc&& movementFunc,
                              int grid_width, int grid_height);

    // Control structure variety
    class MovementCalculator {
    public:
        static SDL_Point ProcessMovement(const SDL_Point& current, MovementPattern pattern,
                                        float speed, float& counter, int direction,
                                        int grid_width, int grid_height);
    private:
        // Switch statement for pattern selection
        static SDL_Point HandlePatternSwitch(MovementPattern pattern, /* params */);

        // Various loop types for different calculations
        static void CalculateCircularPath(/* params */); // for loop
        static void ValidatePathBounds(/* params */);    // while loop
        static void ProcessPathPoints(/* params */);     // range-based for
    };
}
```

**Implementation Checklist:**
- [ ] Create namespace for movement pattern functions
- [ ] Implement function overloading for different movement parameters
- [ ] Use various control structures (for, while, switch, if/else)
- [ ] Add template functions for movement validation
- [ ] Create advanced pathfinding algorithms
- [ ] Implement circular movement with trigonometric calculations

### Sprint 4: Rendering Integration & UI Enhancement (Session 4)

#### Task 4.1: Extend Renderer for Obstacle Display
**Rubric Criteria**: Object-Oriented Programming, Const Correctness
**Files**: `src/renderer.h`, `src/renderer.cpp`

**Renderer Class Extensions:**
```cpp
class Renderer {
public:
    // New obstacle rendering methods
    void RenderObstacles(const ObstacleManager& obstacleManager) const;
    void RenderObstacle(const Obstacle& obstacle) const;
    void RenderFixedObstacle(int x, int y) const;
    void RenderMovingObstacle(int x, int y, MovementPattern pattern) const;

    // Enhanced game state rendering with obstacles
    void RenderPlayingWithObstacles(const Snake& snake, const SDL_Point& food,
                                   const ObstacleManager& obstacleManager) const;


private:
    // New rendering helper methods
    void DrawObstacleCell(int x, int y, const SDL_Color& color) const;
    void DrawMovingObstacle(int x, int y, MovementPattern pattern) const;
    SDL_Color GetObstacleColor(ObstacleType type) const;

    // Const methods for color management
    static constexpr SDL_Color GetFixedObstacleColor() { return {128, 64, 0, 255}; }
    static constexpr SDL_Color GetMovingObstacleColor() { return {255, 165, 0, 255}; }
};
```

**Implementation Checklist:**
- [ ] Add const methods for all rendering operations
- [ ] Use const references for all object parameters
- [ ] Implement different visual styles for obstacle types
- [ ] Use constexpr for compile-time color calculations
- [ ] Ensure simple, clean visual presentation without effects

#### Task 4.2: Game Loop Integration and State Management
**Rubric Criteria**: Loops, Functions, State Management
**Files**: `src/game.cpp`

**Enhanced Game Loop:**
```cpp
void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
    // ... existing game loop structure ...

    while (running) {
        // Event handling with obstacle awareness
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // State-specific event handling
            switch (currentState) {
            case GameState::PLAYING:
                UpdatePlayingWithObstacles(controller, event);
                break;
            // ... other states ...
            }
        }

        // State-specific updates with obstacle integration
        if (currentState == GameState::PLAYING) {
            UpdateWithObstacles(frame_duration / 1000.0f); // Pass delta time in seconds
        }

        // State-specific rendering with obstacles
        switch (currentState) {
        case GameState::PLAYING:
            renderer.RenderPlayingWithObstacles(snake, food, *obstacleManager);
            break;
        // ... other rendering states ...
        }
    }
}

void Game::UpdateWithObstacles(float delta_time) {
    if (!snake.alive) return;

    // ThreadedObstacleManager automatically handles:
    // - Background atomic lifetime countdown (DecrementLifetime on all obstacles)
    // - Automatic expired obstacle cleanup using std::async
    // - Thread-safe container management with shared_mutex
    // - Movement handled on main thread, lifetime on background thread

    obstacleManager->UpdateObstacleMovement(); // Movement only, lifetime handled by background thread

    // Handle obstacle spawning based on timer
    HandleObstacleSpawning(delta_time);

    // Update snake
    snake.Update();

    // Check all collision types (using thread-safe override methods)
    if (obstacleManager->CheckCollisionWithSnake(snake)) {
        snake.alive = false;
    }
    CheckFoodCollection();
    UpdateDifficulty();
}
```

**Implementation Checklist:**
- [ ] Integrate obstacle movement updates into main game loop
- [ ] Initialize background lifetime management threads
- [ ] Add obstacle rendering to all appropriate game states
- [ ] Implement thread-safe collision checking in Update() cycle
- [ ] Add spawning system based on timer and difficulty progression
- [ ] Background thread handles expired obstacle cleanup automatically
- [ ] Use various control structures throughout implementation
- [ ] Maintain 60 FPS performance with concurrent obstacle processing
- [ ] Ensure proper thread synchronization and shutdown

### Sprint 5: Advanced Concurrency Features & Optimization (Session 5)

#### Task 5.1: Enhanced Thread Performance & Monitoring
**Rubric Criteria**: Concurrency, Performance Optimization
**Files**: `src/obstacle_manager.cpp`, `src/performance_monitor.h`

**Enhanced Threading Features:**
```cpp
class PerformanceMonitor {
public:
    // Thread performance monitoring
    void MonitorLifetimeThreadPerformance();
    void TrackCollisionCheckTiming();
    void MeasureThreadSynchronizationOverhead();

    // Performance metrics
    std::chrono::nanoseconds GetAverageLifetimeUpdateTime() const;
    double GetThreadEfficiencyRatio() const;

private:
    std::atomic<uint64_t> lifetime_updates_count{0};
    std::atomic<uint64_t> total_update_time_ns{0};
    std::chrono::steady_clock::time_point start_time;
};
```

**Implementation Checklist:**
- [ ] Add performance monitoring for thread efficiency
- [ ] Implement timing measurements using std::chrono
- [ ] Use std::atomic for lock-free performance counters
- [ ] Add thread contention detection and optimization
- [ ] Implement load balancing for obstacle processing
- [ ] Add configurable thread sleep intervals for efficiency

#### Task 5.2: Asynchronous Obstacle Generation
**Rubric Criteria**: Concurrency, Promise/Future, Async Operations
**Files**: `src/async_obstacle_generator.h`, `src/async_obstacle_generator.cpp`

**Asynchronous Obstacle Generation:**
```cpp
class AsyncObstacleGenerator {
public:
    // Promise/Future pattern for obstacle generation
    std::future<std::vector<std::unique_ptr<Obstacle>>>
    GenerateObstaclesAsync(int fixed_count, int moving_count,
                          int grid_width, int grid_height);

    // Async operations with std::async
    std::future<bool> ValidateObstaclePlacementAsync(
        const std::vector<SDL_Point>& positions,
        const Snake& snake, const SDL_Point& food);

    // Callback-based async generation
    void GenerateObstaclesWithCallback(
        int count,
        std::function<void(std::vector<std::unique_ptr<Obstacle>>)> callback);

private:
    // Worker methods for async operations
    std::vector<std::unique_ptr<Obstacle>> GenerateObstaclesWorker(
        int fixed_count, int moving_count, int grid_width, int grid_height);

    bool ValidatePositionsWorker(const std::vector<SDL_Point>& positions,
                                const Snake& snake, const SDL_Point& food);

    // Thread pool for obstacle generation
    std::vector<std::thread> worker_threads;
    std::queue<std::function<void()>> task_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_condition;
};
```

**Implementation Checklist:**
- [ ] Use `std::async` for non-blocking obstacle generation
- [ ] Implement `std::promise` and `std::future` for result communication
- [ ] Add callback-based asynchronous operations
- [ ] Create thread pool for parallel obstacle processing
- [ ] Use `std::condition_variable` for task coordination
- [ ] Implement proper exception handling in async operations

## Rubric Criteria Mapping

### Loops, Functions, and I/O (Target: 3+ criteria) - **4/4 COMPLETE**
- ✅ **Control Structures**: Various loops (for, while, range-based), switch statements, complex conditionals
- ✅ **Function Overloading**: Multiple method signatures for collision detection and movement patterns
- ✅ **Advanced Algorithms**: Pathfinding, movement pattern calculations, collision optimization
- ✅ **Template Functions**: Generic collision detection and movement validation systems

### Object-Oriented Programming (Target: 4+ criteria) - **6/6 COMPLETE**
- ✅ **Access Specifiers**: All classes use explicit public/private/protected sections
- ✅ **Member Initialization Lists**: All constructors properly initialize members
- ✅ **Inheritance**: Obstacle base class with FixedObstacle and MovingObstacle derived classes
- ✅ **Virtual Functions**: Pure virtual methods in base class with override in derived classes
- ✅ **Encapsulation**: Private data with public getter/setter methods, protected inheritance
- ✅ **Polymorphism**: Runtime polymorphism through virtual functions and abstract base classes

### Memory Management (Target: 4+ criteria) - **5/5 COMPLETE**
- ✅ **References**: Const references throughout for performance and safety
- ✅ **RAII**: Automatic resource management in all classes
- ✅ **Smart Pointers**: `std::unique_ptr` for obstacle ownership and polymorphic containers
- ✅ **Move Semantics**: Move constructors and assignment operators for performance
- ✅ **Rule of Five**: Complete implementation for all resource-managing classes

### Concurrency (Target: 3+ criteria) - **4/4 COMPLETE**
- ✅ **Multithreading**: Separate thread for obstacle processing
- ✅ **Mutex/Lock Guard**: Thread-safe access to shared obstacle data
- ✅ **Promise/Future**: Async obstacle generation and validation
- ✅ **Condition Variables**: Thread coordination for obstacle updates

## Implementation Strategy

### Development Phases
1. **Phase 1**: Core obstacle classes with inheritance and polymorphism
2. **Phase 2**: Obstacle management system with smart pointers and STL containers
3. **Phase 3**: Advanced collision detection and movement algorithms
4. **Phase 4**: Rendering integration and UI enhancements
5. **Phase 5**: Concurrency implementation with thread safety

### Code Quality Standards
- **Const Correctness**: Use `const` wherever possible, especially in getters and rendering
- **Exception Safety**: Implement proper exception handling with RAII
- **Performance**: Maintain 60 FPS with efficient collision detection algorithms
- **Documentation**: Clear function and class documentation explaining behavior
- **Naming Conventions**: Consistent, descriptive naming following existing patterns

### Testing and Validation
- **Collision Detection**: Verify accurate collision detection for all obstacle types
- **Movement Patterns**: Test all movement patterns for correctness and boundaries
- **Thread Safety**: Validate thread-safe operations under concurrent access
- **Performance**: Ensure no FPS degradation with maximum obstacle count
- **Memory Management**: Verify no memory leaks with smart pointer usage

## Risk Mitigation

### Technical Risks
- **Performance Impact**: Optimize collision detection with spatial partitioning if needed
- **Thread Synchronization**: Careful mutex usage to prevent deadlocks
- **Memory Usage**: Monitor obstacle container growth and implement limits
- **Collision Accuracy**: Thorough testing of edge cases and boundary conditions

### Development Risks
- **Complexity Management**: Break down into small, testable components
- **Integration Issues**: Test each sprint independently before integration
- **Rubric Compliance**: Regular verification against rubric criteria

## Success Metrics
- [ ] All user stories completed with acceptance criteria met
- [ ] **19+ rubric criteria addressed across all categories** (exceeds requirements)
- [ ] No performance degradation from baseline 60 FPS
- [ ] Thread-safe concurrent obstacle processing
- [ ] Clean, maintainable code following established patterns
- [ ] **Advanced C++ features**: Templates, smart pointers, concurrency
- [ ] **Professional-grade gameplay**: Progressive difficulty, multiple obstacle types

## Definition of Done

### Code Quality Checklist
- [ ] All classes follow single responsibility principle
- [ ] Proper exception handling for all operations
- [ ] Const correctness throughout implementation
- [ ] No memory leaks (RAII and smart pointers)
- [ ] Thread-safe design with proper synchronization
- [ ] Consistent naming conventions
- [ ] All public methods clearly documented

### Integration Checklist
- [ ] Builds successfully with existing CMake configuration
- [ ] Maintains 60 FPS performance target with obstacles
- [ ] No breaking changes to existing interfaces
- [ ] Cross-platform compatibility preserved
- [ ] Thread safety verified under concurrent access

### Gameplay Checklist
- [ ] Obstacles enhance gameplay without making it impossible
- [ ] Progressive difficulty provides appropriate challenge curve
- [ ] Visual distinction between obstacle types is clear
- [ ] Collision detection is accurate and responsive
- [ ] Movement patterns provide varied and interesting challenges

---

## 🎯 **IMPLEMENTATION ROADMAP**

### ✅ **SPRINT STRUCTURE**
**Sprint 1**: Foundation & Base Classes (Inheritance, Polymorphism, OOP)
**Sprint 2**: Management System (Smart Pointers, Memory Management, STL)
**Sprint 3**: Advanced Algorithms (Collision Detection, Movement Patterns, Templates)
**Sprint 4**: Rendering & UI (Integration, Performance, Visual Enhancement)
**Sprint 5**: Concurrency (Threading, Synchronization, Async Operations)

### 🏆 **EXPECTED RUBRIC ACHIEVEMENT (19+ TOTAL)**

**Loops, Functions, and I/O (4/4)** ✅
- Control structures, function overloading, algorithms, templates

**Object-Oriented Programming (6/6)** ✅
- Access specifiers, initialization lists, inheritance, virtual functions, encapsulation, polymorphism

**Memory Management (5/5)** ✅
- References, RAII, smart pointers, move semantics, Rule of Five

**Concurrency (4/4)** ✅
- Multithreading, mutex protection, promise/future, condition variables

### 🚀 **ADVANCED FEATURES DELIVERED**
1. **Polymorphic Obstacle System** with inheritance hierarchy
2. **Advanced Movement Patterns** with algorithmic calculations
3. **Thread-Safe Concurrent Processing** for smooth gameplay
4. **Smart Pointer Memory Management** for exception safety
5. **Template-Based Generic Algorithms** for reusability
6. **Progressive Difficulty System** for enhanced gameplay

**Project Status**: Ready for implementation - comprehensive architecture designed with atomic lifetime solution
**Achievement**: **EXCEEDS ALL RUBRIC REQUIREMENTS** with advanced C++ features

---

## 🔧 **ATOMIC LIFETIME SOLUTION SUMMARY**

### **Core Architecture Decisions**
1. **Consistent Lifetime Management**: All obstacles use `std::atomic<float> remaining_lifetime` managed by background thread
2. **Clear Separation**: Movement and position handled on main thread, lifetime countdown on background thread
3. **Lock-Free Operations**: Atomic operations eliminate need for additional mutexes on lifetime data
4. **Thread Safety**: `std::shared_mutex` for container access, SDL_Point position is main-thread only

### **Class Responsibilities**
- **Base Obstacle**: Provides atomic lifetime infrastructure with `DecrementLifetime()` method
- **FixedObstacle**: Empty `Update()` method, lifetime managed by background thread
- **MovingObstacle**: `Update()` handles movement only on main thread using SDL_Point position
- **ThreadedObstacleManager**: Background thread calls `DecrementLifetime()` on all obstacles every 100ms

### **Threading Model**
- **Main Thread**: Movement updates, collision detection, rendering, spawning
- **Background Thread**: Atomic lifetime countdown with lock-free operations
- **Async Cleanup**: `std::async` for non-blocking removal of expired obstacles

This solution eliminates the lifetime management contradiction, provides excellent thread safety, and maintains 60 FPS performance with concurrent background processing.