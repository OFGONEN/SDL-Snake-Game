#ifndef MOVEMENT_PATTERNS_H
#define MOVEMENT_PATTERNS_H

#include "SDL.h"
#include "moving_obstacle.h"
#include <vector>
#include <functional>
#include <cmath>

namespace MovementPatterns {
    // Function overloading for different movement types
    SDL_Point CalculateLinearMovement(const SDL_Point& current, int direction, float speed);
    SDL_Point CalculateLinearMovement(const SDL_Point& current, float angle, float speed);

    SDL_Point CalculateCircularMovement(const SDL_Point& center, float radius,
                                       float angle, int grid_width, int grid_height);

    // Advanced movement with pathfinding
    std::vector<SDL_Point> CalculateZigzagPath(const SDL_Point& start, int amplitude,
                                              int wavelength, int grid_width);

    // Template function for movement validation
    template<typename MovementFunc>
    SDL_Point ValidateMovement(const SDL_Point& current, MovementFunc&& movementFunc,
                              int grid_width, int grid_height);

    // Advanced algorithmic movement patterns
    SDL_Point CalculateSpiralMovement(const SDL_Point& center, float radius, float angle, float spiral_rate);
    SDL_Point CalculateFigureEightMovement(const SDL_Point& center, float radius, float angle);
    SDL_Point CalculatePerlinNoiseMovement(const SDL_Point& current, float time, float scale);

    // Control structure variety
    class MovementCalculator {
    public:
        static SDL_Point ProcessMovement(const SDL_Point& current, MovementPattern pattern,
                                        float speed, float& counter, int direction,
                                        int grid_width, int grid_height);

        // Advanced pathfinding algorithms
        static std::vector<SDL_Point> CalculateAStarPath(const SDL_Point& start, const SDL_Point& goal,
                                                        const std::vector<SDL_Point>& obstacles,
                                                        int grid_width, int grid_height);

        // Swarm behavior algorithms
        static SDL_Point CalculateFlockingMovement(const SDL_Point& current,
                                                  const std::vector<SDL_Point>& neighbors,
                                                  float separation_weight, float alignment_weight,
                                                  float cohesion_weight);

        // Noise functions for procedural movement (public for external use)
        static float PerlinNoise(float x, float y);
        static float SimplexNoise(float x, float y);

    private:
        // Switch statement for pattern selection
        static SDL_Point HandlePatternSwitch(MovementPattern pattern, const SDL_Point& current,
                                            float speed, float& counter, int direction,
                                            int grid_width, int grid_height);

        // Various loop types for different calculations
        static void CalculateCircularPath(std::vector<SDL_Point>& path, const SDL_Point& center,
                                         float radius, int segments); // for loop
        static void ValidatePathBounds(std::vector<SDL_Point>& path, int grid_width, int grid_height); // while loop
        static void ProcessPathPoints(const std::vector<SDL_Point>& input,
                                     std::vector<SDL_Point>& output); // range-based for

        // Mathematical helper functions
        static float CalculateDistance(const SDL_Point& p1, const SDL_Point& p2);
        static float CalculateAngle(const SDL_Point& from, const SDL_Point& to);
        static SDL_Point ClampToGrid(const SDL_Point& point, int grid_width, int grid_height);
    };

    // Template function for generic movement validation
    template<typename Validator>
    bool ValidateMovementWithPredicate(const SDL_Point& point, Validator&& validator);

    // Template function for path optimization
    template<typename PathContainer>
    void OptimizePath(PathContainer& path, float optimization_factor);
}

// Template implementations
template<typename MovementFunc>
SDL_Point MovementPatterns::ValidateMovement(const SDL_Point& current, MovementFunc&& movementFunc,
                                            int grid_width, int grid_height) {
    SDL_Point new_pos = movementFunc(current);

    // Clamp to grid boundaries
    if (new_pos.x < 0) new_pos.x = grid_width - 1;
    else if (new_pos.x >= grid_width) new_pos.x = 0;

    if (new_pos.y < 0) new_pos.y = grid_height - 1;
    else if (new_pos.y >= grid_height) new_pos.y = 0;

    return new_pos;
}

template<typename Validator>
bool MovementPatterns::ValidateMovementWithPredicate(const SDL_Point& point, Validator&& validator) {
    return validator(point);
}

template<typename PathContainer>
void MovementPatterns::OptimizePath(PathContainer& path, float optimization_factor) {
    if (path.size() < 3) return;

    // Douglas-Peucker algorithm for path simplification
    std::vector<bool> keep(path.size(), false);
    keep[0] = keep[path.size() - 1] = true;

    // Recursive simplification
    std::function<void(size_t, size_t)> simplify = [&](size_t start, size_t end) {
        if (end - start <= 1) return;

        float max_distance = 0.0f;
        size_t max_index = start;

        for (size_t i = start + 1; i < end; ++i) {
            float distance = PointToLineDistance(path[i], path[start], path[end]);
            if (distance > max_distance) {
                max_distance = distance;
                max_index = i;
            }
        }

        if (max_distance > optimization_factor) {
            keep[max_index] = true;
            simplify(start, max_index);
            simplify(max_index, end);
        }
    };

    simplify(0, path.size() - 1);

    // Rebuild path with only kept points
    PathContainer optimized_path;
    for (size_t i = 0; i < path.size(); ++i) {
        if (keep[i]) {
            optimized_path.push_back(path[i]);
        }
    }

    path = std::move(optimized_path);
}

// Helper function for path optimization
inline float PointToLineDistance(const SDL_Point& point, const SDL_Point& line_start, const SDL_Point& line_end) {
    float A = point.x - line_start.x;
    float B = point.y - line_start.y;
    float C = line_end.x - line_start.x;
    float D = line_end.y - line_start.y;

    float dot = A * C + B * D;
    float len_sq = C * C + D * D;

    if (len_sq == 0) return std::sqrt(A * A + B * B);

    float param = dot / len_sq;

    float xx, yy;
    if (param < 0) {
        xx = line_start.x;
        yy = line_start.y;
    } else if (param > 1) {
        xx = line_end.x;
        yy = line_end.y;
    } else {
        xx = line_start.x + param * C;
        yy = line_start.y + param * D;
    }

    float dx = point.x - xx;
    float dy = point.y - yy;
    return std::sqrt(dx * dx + dy * dy);
}

#endif