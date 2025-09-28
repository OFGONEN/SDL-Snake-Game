#include "obstacle_manager.h"
#include <algorithm>
#include <random>

ObstacleManager::ObstacleManager(int grid_width, int grid_height)
    : grid_width(grid_width),
      grid_height(grid_height),
      engine(dev()),
      random_x(0, grid_width - 1),
      random_y(0, grid_height - 1) {
}

ObstacleManager::~ObstacleManager() = default;

void ObstacleManager::AddFixedObstacle(int x, int y, float lifetime) {
    if (x >= 0 && x < grid_width && y >= 0 && y < grid_height && IsPositionFree(x, y)) {
        obstacles.emplace_back(std::make_unique<FixedObstacle>(x, y, grid_width, grid_height, lifetime));
    }
}

void ObstacleManager::AddMovingObstacle(int x, int y, MovementPattern pattern, float lifetime) {
    if (x >= 0 && x < grid_width && y >= 0 && y < grid_height && IsPositionFree(x, y)) {
        auto moving_obstacle = std::make_unique<MovingObstacle>(x, y, grid_width, grid_height, pattern, lifetime);
        moving_obstacle->SetSpeed(moving_obstacle_speed);
        obstacles.emplace_back(std::move(moving_obstacle));
    }
}

void ObstacleManager::SpawnRandomObstacle() {
    SDL_Point pos = GenerateRandomPosition();
    if (!IsPositionFree(pos.x, pos.y)) {
        return; // Skip if position is occupied
    }

    // 60% chance for fixed obstacle, 40% for moving
    std::uniform_real_distribution<float> type_dist(0.0f, 1.0f);

    if (type_dist(engine) < 0.6f) {
        AddFixedObstacle(pos.x, pos.y);
    } else {
        MovementPattern pattern = GetRandomMovementPattern();
        AddMovingObstacle(pos.x, pos.y, pattern);
    }
}

void ObstacleManager::ClearExpiredObstacles() {
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
                      [](const std::unique_ptr<Obstacle>& obstacle) {
                          return obstacle->IsExpired();
                      }),
        obstacles.end()
    );
}

void ObstacleManager::ClearAllObstacles() {
    obstacles.clear();
}

void ObstacleManager::UpdateObstacleMovement() {
    for (auto& obstacle : obstacles) {
        obstacle->Update();
    }
}

void ObstacleManager::UpdateObstacleLifetimes(float delta_time) {
    for (auto& obstacle : obstacles) {
        obstacle->DecrementLifetime(delta_time);
    }
}

void ObstacleManager::RenderObstacles(SDL_Renderer* renderer, std::size_t screen_width,
                                     std::size_t screen_height, std::size_t grid_width,
                                     std::size_t grid_height) const {
    for (const auto& obstacle : obstacles) {
        obstacle->Render(renderer, screen_width, screen_height, grid_width, grid_height);
    }
}

bool ObstacleManager::CheckCollisionWithPoint(int x, int y) const {
    return std::any_of(obstacles.begin(), obstacles.end(),
                      [x, y](const std::unique_ptr<Obstacle>& obstacle) {
                          return obstacle->CollidesWithPoint(x, y);
                      });
}

bool ObstacleManager::CheckCollisionWithSnake(const Snake& snake) const {
    int head_x = static_cast<int>(snake.head_x);
    int head_y = static_cast<int>(snake.head_y);

    return CheckCollisionWithPoint(head_x, head_y);
}

bool ObstacleManager::IsValidFoodPosition(int x, int y) const {
    return !CheckCollisionWithPoint(x, y);
}

std::size_t ObstacleManager::GetFixedObstacleCount() const {
    return std::count_if(obstacles.begin(), obstacles.end(),
                        [](const std::unique_ptr<Obstacle>& obstacle) {
                            return obstacle->GetType() == ObstacleType::FIXED;
                        });
}

std::size_t ObstacleManager::GetMovingObstacleCount() const {
    return std::count_if(obstacles.begin(), obstacles.end(),
                        [](const std::unique_ptr<Obstacle>& obstacle) {
                            return obstacle->GetType() == ObstacleType::MOVING;
                        });
}

void ObstacleManager::SetSpawnRate(float obstacles_per_second) {
    spawn_rate = obstacles_per_second;
}

void ObstacleManager::SetDifficultyLevel(int level) {
    difficulty_level = level;
    // Increase spawn rate with difficulty
    spawn_rate = 0.3f + (level * 0.1f);
    // Increase moving obstacle speed with difficulty
    moving_obstacle_speed = 0.05f + (level * 0.01f);
}

void ObstacleManager::SetMovingObstacleSpeed(float speed) {
    moving_obstacle_speed = speed;
    // Update existing moving obstacles
    for (auto& obstacle : obstacles) {
        if (obstacle->GetType() == ObstacleType::MOVING) {
            auto* moving_obstacle = static_cast<MovingObstacle*>(obstacle.get());
            moving_obstacle->SetSpeed(speed);
        }
    }
}

bool ObstacleManager::ShouldSpawnObstacle(float delta_time) {
    spawn_timer += delta_time;
    float spawn_interval = 1.0f / spawn_rate; // Convert rate to interval

    if (spawn_timer >= spawn_interval) {
        spawn_timer = 0.0f;
        return true;
    }
    return false;
}

bool ObstacleManager::IsPositionFree(int x, int y) const {
    return !CheckCollisionWithPoint(x, y);
}

SDL_Point ObstacleManager::GenerateRandomPosition() const {
    return {random_x(engine), random_y(engine)};
}

MovementPattern ObstacleManager::GetRandomMovementPattern() const {
    std::uniform_int_distribution<int> pattern_dist(0, 4);
    int pattern_int = pattern_dist(engine);

    switch (pattern_int) {
        case 0: return MovementPattern::LINEAR_HORIZONTAL;
        case 1: return MovementPattern::LINEAR_VERTICAL;
        case 2: return MovementPattern::CIRCULAR;
        case 3: return MovementPattern::ZIGZAG;
        case 4: return MovementPattern::RANDOM_WALK;
        default: return MovementPattern::LINEAR_HORIZONTAL;
    }
}

template<typename ObstacleType>
std::size_t ObstacleManager::CountObstaclesOfType() const {
    return std::count_if(obstacles.begin(), obstacles.end(),
                        [](const std::unique_ptr<Obstacle>& obstacle) {
                            return dynamic_cast<const ObstacleType*>(obstacle.get()) != nullptr;
                        });
}