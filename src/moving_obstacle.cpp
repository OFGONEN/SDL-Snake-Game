#include "moving_obstacle.h"
#include "movement_patterns.h"
#include <random>

MovingObstacle::MovingObstacle(int x, int y, int grid_width, int grid_height,
                               MovementPattern pattern, float lifetime_seconds)
    : Obstacle(x, y, grid_width, grid_height, lifetime_seconds),
      pattern(pattern) {
}

void MovingObstacle::Update() {
    // Use advanced movement pattern calculation with optimization
    SDL_Point new_pos = MovementPatterns::MovementCalculator::ProcessMovement(
        position, pattern, speed, movement_counter, direction, grid_width, grid_height);

    // Validate and apply movement
    position = MovementPatterns::ValidateMovement(
        position,
        [new_pos](const SDL_Point&) { return new_pos; },
        grid_width, grid_height);

    movement_counter += speed;
}

void MovingObstacle::Render(SDL_Renderer* renderer, std::size_t screen_width,
                           std::size_t screen_height, std::size_t grid_width,
                           std::size_t grid_height) const {
    SDL_Rect block;
    block.w = screen_width / grid_width;
    block.h = screen_height / grid_height;
    block.x = position.x * block.w;
    block.y = position.y * block.h;

    SDL_SetRenderDrawColor(renderer, kMovingObstacleColor.r, kMovingObstacleColor.g,
                          kMovingObstacleColor.b, kMovingObstacleColor.a);
    SDL_RenderFillRect(renderer, &block);
}

void MovingObstacle::SetPattern(MovementPattern pattern) {
    this->pattern = pattern;
    movement_counter = 0.0f; // Reset counter when pattern changes
}

void MovingObstacle::UpdateLinearHorizontal() {
    float new_x = position.x + (speed * direction);

    if (new_x >= grid_width || new_x < 0) {
        direction *= -1; // Reverse direction at boundaries
        new_x = position.x + (speed * direction);
    }

    position.x = static_cast<int>(new_x);
}

void MovingObstacle::UpdateLinearVertical() {
    float new_y = position.y + (speed * direction);

    if (new_y >= grid_height || new_y < 0) {
        direction *= -1; // Reverse direction at boundaries
        new_y = position.y + (speed * direction);
    }

    position.y = static_cast<int>(new_y);
}

void MovingObstacle::UpdateCircular() {
    movement_counter += speed;

    // Calculate circular movement around a center point
    int center_x = grid_width / 2;
    int center_y = grid_height / 2;
    int radius = std::min(grid_width, grid_height) / 4;

    position.x = center_x + static_cast<int>(radius * std::cos(movement_counter));
    position.y = center_y + static_cast<int>(radius * std::sin(movement_counter));

    // Ensure we stay within bounds
    position.x = std::max(0, std::min(grid_width - 1, position.x));
    position.y = std::max(0, std::min(grid_height - 1, position.y));
}

void MovingObstacle::UpdateZigzag() {
    movement_counter += speed;

    // Horizontal movement with vertical oscillation
    position.x += direction;
    if (position.x >= grid_width || position.x < 0) {
        direction *= -1;
        position.x += direction;
    }

    // Zigzag vertical movement
    int amplitude = 3; // Vertical oscillation range
    position.y = (grid_height / 2) + static_cast<int>(amplitude * std::sin(movement_counter * 2));

    // Ensure we stay within bounds
    position.y = std::max(0, std::min(grid_height - 1, position.y));
}

void MovingObstacle::UpdateRandomWalk() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> direction_dist(0, 3);

    // Random direction every few steps
    if (static_cast<int>(movement_counter) % 10 == 0) {
        int random_direction = direction_dist(gen);
        switch (random_direction) {
            case 0: // Up
                if (position.y > 0) position.y--;
                break;
            case 1: // Down
                if (position.y < grid_height - 1) position.y++;
                break;
            case 2: // Left
                if (position.x > 0) position.x--;
                break;
            case 3: // Right
                if (position.x < grid_width - 1) position.x++;
                break;
        }
    }

    movement_counter += speed;
}

template<typename T>
bool MovingObstacle::IsWithinBounds(T value, T min, T max) const {
    return value >= min && value <= max;
}