#ifndef OBSTACLE_MANAGER_H
#define OBSTACLE_MANAGER_H

#include "obstacle.h"
#include "fixed_obstacle.h"
#include "moving_obstacle.h"
#include "snake.h"
#include <vector>
#include <memory>
#include <random>
#include <algorithm>

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

protected:
    const int grid_width;
    const int grid_height;

    // Basic obstacle container (base class)
    std::vector<std::unique_ptr<Obstacle>> obstacles;

private:

    // Random number generation for obstacle placement
    std::random_device dev;
    mutable std::mt19937 engine;
    mutable std::uniform_int_distribution<int> random_x;
    mutable std::uniform_int_distribution<int> random_y;

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

#endif