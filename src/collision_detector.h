#ifndef COLLISION_DETECTOR_H
#define COLLISION_DETECTOR_H

#include "SDL.h"
#include "snake.h"
#include "obstacle_manager.h"
#include <vector>
#include <algorithm>

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

    // Template function for generic collision checking with early exit
    template<typename Container>
    static bool CheckCollisionWithContainer(int x, int y, const Container& container);

    // Template function for range-based collision checking
    template<typename Iterator>
    static bool CheckCollisionInRange(int x, int y, Iterator begin, Iterator end);

    // Advanced collision detection with spatial optimization
    static bool CheckCollisionOptimized(const Snake& snake, const ObstacleManager& obstacleManager);

    // Collision detection with custom predicate
    template<typename Predicate>
    static bool CheckCollisionWithPredicate(const Snake& snake, const ObstacleManager& obstacleManager, Predicate pred);

private:
    // No instantiation - utility class only
    CollisionDetector() = delete;
    ~CollisionDetector() = delete;
    CollisionDetector(const CollisionDetector&) = delete;
    CollisionDetector& operator=(const CollisionDetector&) = delete;

    // Helper methods for optimization
    static bool IsInBounds(int x, int y, int width, int height);
    static SDL_Rect GetSnakeHeadRect(const Snake& snake);
};

// Template implementations
template<typename Container>
bool CollisionDetector::CheckCollisionWithContainer(int x, int y, const Container& container) {
    return std::any_of(container.begin(), container.end(),
                      [x, y](const auto& item) {
                          return item.x == x && item.y == y;
                      });
}

template<typename Iterator>
bool CollisionDetector::CheckCollisionInRange(int x, int y, Iterator begin, Iterator end) {
    return std::any_of(begin, end,
                      [x, y](const auto& item) {
                          return item.x == x && item.y == y;
                      });
}

template<typename Predicate>
bool CollisionDetector::CheckCollisionWithPredicate(const Snake& snake, const ObstacleManager& obstacleManager, Predicate pred) {
    int head_x = static_cast<int>(snake.head_x);
    int head_y = static_cast<int>(snake.head_y);

    // Early exit if predicate fails
    if (!pred(head_x, head_y)) {
        return false;
    }

    return obstacleManager.CheckCollisionWithPoint(head_x, head_y);
}

#endif