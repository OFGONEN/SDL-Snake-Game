#ifndef OBSTACLE_H
#define OBSTACLE_H

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
    int GetX() const;
    int GetY() const;
    const SDL_Point& GetPosition() const;

    // Thread-safe lifetime management (managed by background thread)
    bool IsExpired() const;
    float GetRemainingLifetime() const;
    void DecrementLifetime(float delta_time);

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

#endif