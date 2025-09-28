#ifndef MOVING_OBSTACLE_H
#define MOVING_OBSTACLE_H

#include "obstacle.h"
#include <cmath>

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

#endif