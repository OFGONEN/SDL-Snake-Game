#include "obstacle.h"
#include <cmath>
#include <limits>

Obstacle::Obstacle(int x, int y, int grid_width, int grid_height, float lifetime_seconds)
    : position{x, y},
      grid_width(grid_width),
      grid_height(grid_height),
      remaining_lifetime(lifetime_seconds) {
}

int Obstacle::GetX() const {
    return position.x;
}

int Obstacle::GetY() const {
    return position.y;
}

const SDL_Point& Obstacle::GetPosition() const {
    return position;
}

bool Obstacle::IsExpired() const {
    constexpr float epsilon = std::numeric_limits<float>::epsilon();
    return remaining_lifetime.load() < epsilon;
}

float Obstacle::GetRemainingLifetime() const {
    return remaining_lifetime.load();
}

void Obstacle::DecrementLifetime(float delta_time) {
    float current = remaining_lifetime.load();
    remaining_lifetime.store(std::max(0.0f, current - delta_time));
}

bool Obstacle::CollidesWithPoint(int x, int y) const {
    return position.x == x && position.y == y;
}

bool Obstacle::CollidesWithRect(const SDL_Rect& rect) const {
    return position.x >= rect.x &&
           position.x < rect.x + rect.w &&
           position.y >= rect.y &&
           position.y < rect.y + rect.h;
}

void Obstacle::WrapPosition() {
    if (position.x < 0) {
        position.x = grid_width - 1;
    } else if (position.x >= grid_width) {
        position.x = 0;
    }

    if (position.y < 0) {
        position.y = grid_height - 1;
    } else if (position.y >= grid_height) {
        position.y = 0;
    }
}

bool Obstacle::IsValidPosition(int x, int y) const {
    return x >= 0 && x < grid_width && y >= 0 && y < grid_height;
}