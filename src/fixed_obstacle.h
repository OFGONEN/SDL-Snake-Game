#ifndef FIXED_OBSTACLE_H
#define FIXED_OBSTACLE_H

#include "obstacle.h"

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

#endif