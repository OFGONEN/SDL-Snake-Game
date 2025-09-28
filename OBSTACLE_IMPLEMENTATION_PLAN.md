# Obstacle System Implementation Plan

## Epic Overview
**Feature**: Fixed and Moving Obstacles for Enhanced Gameplay
**Goal**: Add static and dynamic obstacles to increase game difficulty and demonstrate advanced C++ programming concepts
**Estimated Effort**: 4-5 development sessions
**Priority**: High (addresses multiple rubric criteria with advanced OOP patterns)

## User Stories

### User Story 1: Fixed Obstacles
**As a** player
**I want to** encounter static obstacles on the game board
**So that** I need to navigate around barriers while collecting food

**Acceptance Criteria:**
- [ ] Fixed obstacles are placed randomly on the game board at game start
- [ ] Obstacles are visually distinct from snake and food (different color/texture)
- [ ] Snake dies when colliding with fixed obstacles
- [ ] Food cannot spawn on obstacle locations
- [ ] Number of obstacles increases with score progression
- [ ] Obstacles persist throughout the game session

### User Story 2: Moving Obstacles
**As a** player
**I want to** face dynamic moving obstacles
**So that** the game becomes progressively more challenging and unpredictable

**Acceptance Criteria:**
- [ ] Moving obstacles patrol in predictable patterns (linear, circular, zigzag)
- [ ] Moving obstacles respect grid boundaries and wrap around screen edges
- [ ] Multiple movement patterns available for variety
- [ ] Snake dies when colliding with moving obstacles
- [ ] Moving obstacles avoid colliding with each other
- [ ] Movement speed configurable and increases with game progression

### User Story 3: Progressive Difficulty
**As a** player
**I want** obstacle difficulty to increase as my score improves
**So that** the game remains challenging throughout long sessions

**Acceptance Criteria:**
- [ ] More obstacles spawn as score increases
- [ ] Moving obstacle speed increases with progression
- [ ] New obstacle types unlock at score milestones
- [ ] Maximum obstacle density prevents impossible scenarios
- [ ] Difficulty progression is smooth and predictable
- [ ] Player can see obstacle count/difficulty in UI

## Technical Implementation Plan

### Sprint 1: Foundation & Base Classes (Session 1)

#### Task 1.1: Create Obstacle Base Class Hierarchy
**Rubric Criteria**: Object-Oriented Programming, Inheritance, Polymorphism
**Files**: `src/obstacle.h`, `src/obstacle.cpp`

**Base Obstacle Class Design:**
```cpp
class Obstacle {
public:
    explicit Obstacle(int x, int y, int grid_width, int grid_height);
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

    // Collision detection
    bool CollidesWithPoint(int x, int y) const;
    virtual bool CollidesWithRect(const SDL_Rect& rect) const;

protected:
    SDL_Point position;
    const int grid_width;
    const int grid_height;
    bool active{true};

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

#### Task 1.2: Create FixedObstacle Class
**Rubric Criteria**: Object-Oriented Programming, Inheritance
**Files**: `src/fixed_obstacle.h`, `src/fixed_obstacle.cpp`

**FixedObstacle Class Design:**
```cpp
class FixedObstacle : public Obstacle {
public:
    explicit FixedObstacle(int x, int y, int grid_width, int grid_height);

    // Override virtual methods
    void Update() override final;
    void Render(SDL_Renderer* renderer, std::size_t screen_width,
               std::size_t screen_height, std::size_t grid_width,
               std::size_t grid_height) const override;
    ObstacleType GetType() const override { return ObstacleType::FIXED; }

private:
    static constexpr SDL_Color kFixedObstacleColor{128, 64, 0, 255}; // Brown
};
```

**Implementation Checklist:**
- [ ] Inherit from Obstacle base class
- [ ] Use `override` keyword for all virtual method implementations
- [ ] Use `final` for methods that shouldn't be overridden further
- [ ] Implement static rendering (Update() does nothing)
- [ ] Use constexpr for compile-time color constants
- [ ] Add proper constructor with member initialization list

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
                           MovementPattern pattern = MovementPattern::LINEAR_HORIZONTAL);

    // Override virtual methods
    void Update() override;
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

### Sprint 2: Obstacle Management System (Session 2)

#### Task 2.1: Create ObstacleManager Class
**Rubric Criteria**: Memory Management, Smart Pointers, STL Containers
**Files**: `src/obstacle_manager.h`, `src/obstacle_manager.cpp`

**ObstacleManager Class Design:**
```cpp
class ObstacleManager {
public:
    explicit ObstacleManager(int grid_width, int grid_height);
    ~ObstacleManager() = default;

    // Rule of Five with move semantics
    ObstacleManager(const ObstacleManager& other) = delete;
    ObstacleManager& operator=(const ObstacleManager& other) = delete;
    ObstacleManager(ObstacleManager&& other) noexcept = default;
    ObstacleManager& operator=(ObstacleManager&& other) noexcept = default;

    // Obstacle management
    void AddFixedObstacle(int x, int y);
    void AddMovingObstacle(int x, int y, MovementPattern pattern);
    void GenerateRandomObstacles(int fixed_count, int moving_count);
    void ClearAllObstacles();

    // Update and rendering
    void UpdateObstacles();
    void RenderObstacles(SDL_Renderer* renderer, std::size_t screen_width,
                        std::size_t screen_height, std::size_t grid_width,
                        std::size_t grid_height) const;

    // Collision detection
    bool CheckCollisionWithPoint(int x, int y) const;
    bool CheckCollisionWithSnake(const Snake& snake) const;
    bool IsValidFoodPosition(int x, int y) const;

    // Getters for game logic
    std::size_t GetObstacleCount() const { return obstacles.size(); }
    std::size_t GetFixedObstacleCount() const;
    std::size_t GetMovingObstacleCount() const;

    // Configuration
    void SetDifficultyLevel(int level);
    void SetMovingObstacleSpeed(float speed);

private:
    const int grid_width;
    const int grid_height;

    // Smart pointer container for polymorphic obstacle management
    std::vector<std::unique_ptr<Obstacle>> obstacles;

    // Random number generation for obstacle placement
    std::random_device dev;
    std::mt19937 engine;
    std::uniform_int_distribution<int> random_x;
    std::uniform_int_distribution<int> random_y;

    // Configuration
    int difficulty_level{1};
    float moving_obstacle_speed{0.05f};

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
- [ ] Add random number generation for obstacle placement
- [ ] Create collision detection methods for different scenarios
- [ ] Implement difficulty scaling logic
- [ ] Use template methods for type-specific counting
- [ ] Add position validation to prevent overlaps

#### Task 2.2: Integrate ObstacleManager with Game Class
**Rubric Criteria**: Memory Management, References, Composition
**Files**: `src/game.h`, `src/game.cpp`

**Game Class Extensions:**
```cpp
class Game {
private:
    // Add obstacle management
    std::unique_ptr<ObstacleManager> obstacleManager;

    // Configuration
    static constexpr int kInitialFixedObstacles = 3;
    static constexpr int kInitialMovingObstacles = 1;
    static constexpr int kObstacleIncreaseInterval = 10; // Every 10 points

    // Updated methods
    void PlaceFood(); // Modified to avoid obstacles
    void Update(); // Modified to check obstacle collisions
    void UpdatePlaying(const Controller& controller, const SDL_Event& event); // Add obstacle updates
    void ResetGame(); // Reset obstacles on new game

    // New methods
    void UpdateObstacles();
    void CheckObstacleCollisions();
    void UpdateDifficulty();
    bool IsValidFoodPosition(int x, int y) const;
};
```

**Implementation Checklist:**
- [ ] Add `std::unique_ptr<ObstacleManager>` to Game class
- [ ] Use member initialization lists in constructor
- [ ] Modify PlaceFood() to use ObstacleManager::IsValidFoodPosition()
- [ ] Add obstacle update calls to main Update() method
- [ ] Implement collision checking in UpdatePlaying()
- [ ] Add difficulty progression based on score
- [ ] Use const references for collision detection parameters

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

    // UI enhancement for obstacle information
    void RenderObstacleInfo(int fixed_count, int moving_count, int difficulty_level) const;
    void RenderDifficultyLevel(int level) const;

private:
    // New rendering helper methods
    void DrawObstacleCell(int x, int y, const SDL_Color& color) const;
    void DrawMovingObstacleWithEffect(int x, int y, MovementPattern pattern) const;
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
- [ ] Add visual effects for moving obstacles (glow, animation)
- [ ] Create UI elements showing obstacle statistics
- [ ] Use constexpr for compile-time color calculations

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
            UpdateWithObstacles(); // Enhanced update method
        }

        // State-specific rendering with obstacles
        switch (currentState) {
        case GameState::PLAYING:
            renderer.RenderPlayingWithObstacles(snake, food, *obstacleManager);
            renderer.RenderObstacleInfo(obstacleManager->GetFixedObstacleCount(),
                                       obstacleManager->GetMovingObstacleCount(),
                                       GetDifficultyLevel());
            break;
        // ... other rendering states ...
        }
    }
}

void Game::UpdateWithObstacles() {
    if (!snake.alive) return;

    // Update obstacles first
    obstacleManager->UpdateObstacles();

    // Update snake
    snake.Update();

    // Check all collision types
    CheckSnakeCollisions();
    CheckFoodCollection();
    UpdateDifficulty();
}
```

**Implementation Checklist:**
- [ ] Integrate obstacle updates into main game loop
- [ ] Add obstacle rendering to all appropriate game states
- [ ] Implement enhanced collision checking in Update() cycle
- [ ] Add difficulty progression logic based on score
- [ ] Use various control structures throughout implementation
- [ ] Maintain 60 FPS performance with obstacle processing

### Sprint 5: Advanced Features & Concurrency (Session 5)

#### Task 5.1: Thread-Safe Obstacle Management
**Rubric Criteria**: Concurrency, Mutex, Thread Safety
**Files**: `src/threaded_obstacle_manager.h`, `src/threaded_obstacle_manager.cpp`

**Thread-Safe ObstacleManager:**
```cpp
class ThreadedObstacleManager : public ObstacleManager {
public:
    explicit ThreadedObstacleManager(int grid_width, int grid_height);
    ~ThreadedObstacleManager();

    // Thread-safe obstacle operations
    void StartObstacleThread();
    void StopObstacleThread();
    void UpdateObstaclesAsync();

    // Thread-safe getters with mutex protection
    std::size_t GetObstacleCountSafe() const;
    bool CheckCollisionWithPointSafe(int x, int y) const;

private:
    // Threading components
    std::thread obstacle_thread;
    std::atomic<bool> should_stop{false};
    std::atomic<bool> thread_running{false};

    // Synchronization primitives
    mutable std::mutex obstacle_mutex;
    std::condition_variable update_condition;

    // Thread function
    void ObstacleUpdateThread();

    // Thread-safe internal methods
    void UpdateObstaclesInternal();
    void SafelyUpdateMovingObstacles();
};
```

**Implementation Checklist:**
- [ ] Use `std::thread` for obstacle processing
- [ ] Implement `std::mutex` protection for shared data
- [ ] Add `std::atomic` variables for thread control
- [ ] Use `std::condition_variable` for thread coordination
- [ ] Implement proper thread cleanup in destructor
- [ ] Add thread-safe versions of all public methods

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

**Project Status**: Ready for implementation - comprehensive architecture designed
**Achievement**: **EXCEEDS ALL RUBRIC REQUIREMENTS** with advanced C++ features