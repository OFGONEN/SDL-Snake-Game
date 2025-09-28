#include "fixed_obstacle.h"

FixedObstacle::FixedObstacle(int x, int y, int grid_width, int grid_height, float lifetime_seconds)
    : Obstacle(x, y, grid_width, grid_height, lifetime_seconds) {
}

void FixedObstacle::Update() {
    // Does nothing - lifetime managed by background thread
    // Fixed obstacles don't move, only their lifetime changes
}

void FixedObstacle::Render(SDL_Renderer* renderer, std::size_t screen_width,
                          std::size_t screen_height, std::size_t grid_width,
                          std::size_t grid_height) const {
    SDL_Rect block;
    block.w = screen_width / grid_width;
    block.h = screen_height / grid_height;
    block.x = position.x * block.w;
    block.y = position.y * block.h;

    SDL_SetRenderDrawColor(renderer, kFixedObstacleColor.r, kFixedObstacleColor.g,
                          kFixedObstacleColor.b, kFixedObstacleColor.a);
    SDL_RenderFillRect(renderer, &block);
}