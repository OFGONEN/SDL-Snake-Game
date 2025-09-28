#include "collision_detector.h"

bool CollisionDetector::PointToPoint(int x1, int y1, int x2, int y2) {
    return x1 == x2 && y1 == y2;
}

bool CollisionDetector::PointToRect(int px, int py, const SDL_Rect& rect) {
    return px >= rect.x && px < rect.x + rect.w &&
           py >= rect.y && py < rect.y + rect.h;
}

bool CollisionDetector::RectToRect(const SDL_Rect& rect1, const SDL_Rect& rect2) {
    return !(rect1.x + rect1.w <= rect2.x ||
             rect2.x + rect2.w <= rect1.x ||
             rect1.y + rect1.h <= rect2.y ||
             rect2.y + rect2.h <= rect1.y);
}

bool CollisionDetector::SnakeToObstacles(const Snake& snake, const ObstacleManager& obstacleManager) {
    int head_x = static_cast<int>(snake.head_x);
    int head_y = static_cast<int>(snake.head_y);

    return obstacleManager.CheckCollisionWithPoint(head_x, head_y);
}

bool CollisionDetector::SnakeHeadToPoint(const Snake& snake, int x, int y) {
    int head_x = static_cast<int>(snake.head_x);
    int head_y = static_cast<int>(snake.head_y);

    return PointToPoint(head_x, head_y, x, y);
}

bool CollisionDetector::CheckCollision(const Snake& snake, const std::vector<SDL_Point>& points) {
    int head_x = static_cast<int>(snake.head_x);
    int head_y = static_cast<int>(snake.head_y);

    return CheckCollisionWithContainer(head_x, head_y, points);
}

bool CollisionDetector::CheckCollision(const Snake& snake, const ObstacleManager& obstacleManager) {
    return SnakeToObstacles(snake, obstacleManager);
}

bool CollisionDetector::CheckCollision(int x, int y, const std::vector<SDL_Point>& obstacles) {
    return CheckCollisionWithContainer(x, y, obstacles);
}

bool CollisionDetector::CheckCollisionOptimized(const Snake& snake, const ObstacleManager& obstacleManager) {
    // Early exit optimization - check bounds first
    int head_x = static_cast<int>(snake.head_x);
    int head_y = static_cast<int>(snake.head_y);

    // Quick bounds check (assuming grid dimensions are reasonable)
    if (!IsInBounds(head_x, head_y, 32, 32)) { // Default grid size
        return false;
    }

    // Use optimized collision detection
    return obstacleManager.CheckCollisionWithPoint(head_x, head_y);
}

bool CollisionDetector::IsInBounds(int x, int y, int width, int height) {
    return x >= 0 && x < width && y >= 0 && y < height;
}

SDL_Rect CollisionDetector::GetSnakeHeadRect(const Snake& snake) {
    SDL_Rect headRect;
    headRect.x = static_cast<int>(snake.head_x);
    headRect.y = static_cast<int>(snake.head_y);
    headRect.w = 1;
    headRect.h = 1;
    return headRect;
}