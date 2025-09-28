#include "movement_patterns.h"
#include <random>
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <functional>

namespace MovementPatterns {

SDL_Point CalculateLinearMovement(const SDL_Point& current, int direction, float speed) {
    SDL_Point new_pos = current;

    switch (direction) {
        case 0: // Up
            new_pos.y -= static_cast<int>(speed);
            break;
        case 1: // Right
            new_pos.x += static_cast<int>(speed);
            break;
        case 2: // Down
            new_pos.y += static_cast<int>(speed);
            break;
        case 3: // Left
            new_pos.x -= static_cast<int>(speed);
            break;
    }

    return new_pos;
}

SDL_Point CalculateLinearMovement(const SDL_Point& current, float angle, float speed) {
    SDL_Point new_pos;
    new_pos.x = current.x + static_cast<int>(speed * std::cos(angle));
    new_pos.y = current.y + static_cast<int>(speed * std::sin(angle));
    return new_pos;
}

SDL_Point CalculateCircularMovement(const SDL_Point& center, float radius, float angle, int grid_width, int grid_height) {
    SDL_Point new_pos;
    new_pos.x = center.x + static_cast<int>(radius * std::cos(angle));
    new_pos.y = center.y + static_cast<int>(radius * std::sin(angle));

    // Ensure within grid bounds
    new_pos.x = std::max(0, std::min(grid_width - 1, new_pos.x));
    new_pos.y = std::max(0, std::min(grid_height - 1, new_pos.y));

    return new_pos;
}

std::vector<SDL_Point> CalculateZigzagPath(const SDL_Point& start, int amplitude, int wavelength, int grid_width) {
    std::vector<SDL_Point> path;

    for (int x = start.x; x < grid_width; x += wavelength / 4) {
        for (int phase = 0; phase < 4; ++phase) {
            SDL_Point point;
            point.x = x + (wavelength / 4) * phase;

            // Calculate zigzag y position
            float cycle_pos = static_cast<float>(phase) / 4.0f;
            if (cycle_pos < 0.5f) {
                point.y = start.y + static_cast<int>(amplitude * (cycle_pos * 4.0f - 1.0f));
            } else {
                point.y = start.y + static_cast<int>(amplitude * (3.0f - cycle_pos * 4.0f));
            }

            if (point.x >= grid_width) break;
            path.push_back(point);
        }
    }

    return path;
}

SDL_Point CalculateSpiralMovement(const SDL_Point& center, float radius, float angle, float spiral_rate) {
    float spiral_radius = radius + spiral_rate * angle;
    SDL_Point new_pos;
    new_pos.x = center.x + static_cast<int>(spiral_radius * std::cos(angle));
    new_pos.y = center.y + static_cast<int>(spiral_radius * std::sin(angle));
    return new_pos;
}

SDL_Point CalculateFigureEightMovement(const SDL_Point& center, float radius, float angle) {
    SDL_Point new_pos;
    new_pos.x = center.x + static_cast<int>(radius * std::cos(angle));
    new_pos.y = center.y + static_cast<int>(radius * std::sin(2 * angle));
    return new_pos;
}

SDL_Point CalculatePerlinNoiseMovement(const SDL_Point& current, float time, float scale) {
    // Simplified Perlin noise implementation
    float noise_x = MovementCalculator::PerlinNoise(current.x * scale, time);
    float noise_y = MovementCalculator::PerlinNoise(current.y * scale, time + 1000.0f);

    SDL_Point new_pos;
    new_pos.x = current.x + static_cast<int>(noise_x * 2.0f - 1.0f);
    new_pos.y = current.y + static_cast<int>(noise_y * 2.0f - 1.0f);

    return new_pos;
}

SDL_Point MovementCalculator::ProcessMovement(const SDL_Point& current, MovementPattern pattern,
                                             float speed, float& counter, int direction,
                                             int grid_width, int grid_height) {
    return HandlePatternSwitch(pattern, current, speed, counter, direction, grid_width, grid_height);
}

SDL_Point MovementCalculator::HandlePatternSwitch(MovementPattern pattern, const SDL_Point& current,
                                                 float speed, float& counter, int direction,
                                                 int grid_width, int grid_height) {
    switch (pattern) {
        case MovementPattern::LINEAR_HORIZONTAL:
            return CalculateLinearMovement(current, direction == 1 ? 0 : 2, speed);

        case MovementPattern::LINEAR_VERTICAL:
            return CalculateLinearMovement(current, direction == 1 ? 1 : 3, speed);

        case MovementPattern::CIRCULAR: {
            SDL_Point center = {grid_width / 2, grid_height / 2};
            float radius = std::min(grid_width, grid_height) / 4.0f;
            return CalculateCircularMovement(center, radius, counter, grid_width, grid_height);
        }

        case MovementPattern::ZIGZAG: {
            auto path = CalculateZigzagPath(current, 3, 8, grid_width);
            if (!path.empty()) {
                int index = static_cast<int>(counter) % path.size();
                return path[index];
            }
            return current;
        }

        case MovementPattern::RANDOM_WALK: {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            static std::uniform_int_distribution<> dist(0, 3);

            return CalculateLinearMovement(current, dist(gen), speed);
        }

        default:
            return current;
    }
}

std::vector<SDL_Point> MovementCalculator::CalculateAStarPath(const SDL_Point& start, const SDL_Point& goal,
                                                             const std::vector<SDL_Point>& obstacles,
                                                             int grid_width, int grid_height) {
    // A* pathfinding implementation
    struct Node {
        SDL_Point pos;
        float g_cost;
        float h_cost;
        SDL_Point parent;

        float f_cost() const { return g_cost + h_cost; }
    };

    auto heuristic = [](const SDL_Point& a, const SDL_Point& b) -> float {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    };

    std::priority_queue<Node, std::vector<Node>, std::function<bool(const Node&, const Node&)>>
        open_set([](const Node& a, const Node& b) { return a.f_cost() > b.f_cost(); });

    std::unordered_set<int> closed_set;
    std::unordered_set<int> obstacle_set;

    // Convert obstacles to hash set for O(1) lookup
    for (const auto& obs : obstacles) {
        obstacle_set.insert(obs.y * grid_width + obs.x);
    }

    Node start_node = {start, 0.0f, heuristic(start, goal), {-1, -1}};
    open_set.push(start_node);

    std::vector<Node> all_nodes;
    all_nodes.push_back(start_node);

    while (!open_set.empty()) {
        Node current = open_set.top();
        open_set.pop();

        if (current.pos.x == goal.x && current.pos.y == goal.y) {
            // Reconstruct path
            std::vector<SDL_Point> path;
            SDL_Point current_pos = current.pos;

            while (current_pos.x != -1 && current_pos.y != -1) {
                path.push_back(current_pos);
                // Find parent node
                bool found = false;
                for (const auto& node : all_nodes) {
                    if (node.pos.x == current.parent.x && node.pos.y == current.parent.y) {
                        current_pos = node.parent;
                        found = true;
                        break;
                    }
                }
                if (!found) break;
            }

            std::reverse(path.begin(), path.end());
            return path;
        }

        closed_set.insert(current.pos.y * grid_width + current.pos.x);

        // Check all neighbors
        const int dx[] = {0, 1, 0, -1};
        const int dy[] = {-1, 0, 1, 0};

        for (int i = 0; i < 4; ++i) {
            SDL_Point neighbor = {current.pos.x + dx[i], current.pos.y + dy[i]};

            // Check bounds
            if (neighbor.x < 0 || neighbor.x >= grid_width ||
                neighbor.y < 0 || neighbor.y >= grid_height) {
                continue;
            }

            int neighbor_hash = neighbor.y * grid_width + neighbor.x;

            // Skip if obstacle or already processed
            if (obstacle_set.count(neighbor_hash) || closed_set.count(neighbor_hash)) {
                continue;
            }

            float tentative_g = current.g_cost + 1.0f;

            Node neighbor_node = {neighbor, tentative_g, heuristic(neighbor, goal), current.pos};
            all_nodes.push_back(neighbor_node);
            open_set.push(neighbor_node);
        }
    }

    return {}; // No path found
}

SDL_Point MovementCalculator::CalculateFlockingMovement(const SDL_Point& current,
                                                       const std::vector<SDL_Point>& neighbors,
                                                       float separation_weight, float alignment_weight,
                                                       float cohesion_weight) {
    if (neighbors.empty()) return current;

    SDL_Point separation = {0, 0};
    SDL_Point alignment = {0, 0};
    SDL_Point cohesion = {0, 0};

    // Calculate separation (avoid crowding)
    for (const auto& neighbor : neighbors) {
        float distance = CalculateDistance(current, neighbor);
        if (distance > 0 && distance < 3.0f) {
            separation.x += current.x - neighbor.x;
            separation.y += current.y - neighbor.y;
        }
    }

    // Calculate cohesion (move toward center of mass)
    for (const auto& neighbor : neighbors) {
        cohesion.x += neighbor.x;
        cohesion.y += neighbor.y;
    }
    cohesion.x /= neighbors.size();
    cohesion.y /= neighbors.size();
    cohesion.x -= current.x;
    cohesion.y -= current.y;

    // Combine forces
    SDL_Point result;
    result.x = current.x + static_cast<int>(separation.x * separation_weight +
                                           alignment.x * alignment_weight +
                                           cohesion.x * cohesion_weight);
    result.y = current.y + static_cast<int>(separation.y * separation_weight +
                                           alignment.y * alignment_weight +
                                           cohesion.y * cohesion_weight);

    return result;
}

void MovementCalculator::CalculateCircularPath(std::vector<SDL_Point>& path, const SDL_Point& center,
                                              float radius, int segments) {
    // for loop implementation
    for (int i = 0; i < segments; ++i) {
        float angle = (2.0f * M_PI * i) / segments;
        SDL_Point point;
        point.x = center.x + static_cast<int>(radius * std::cos(angle));
        point.y = center.y + static_cast<int>(radius * std::sin(angle));
        path.push_back(point);
    }
}

void MovementCalculator::ValidatePathBounds(std::vector<SDL_Point>& path, int grid_width, int grid_height) {
    // while loop implementation
    auto it = path.begin();
    while (it != path.end()) {
        if (it->x < 0 || it->x >= grid_width || it->y < 0 || it->y >= grid_height) {
            it = path.erase(it);
        } else {
            ++it;
        }
    }
}

void MovementCalculator::ProcessPathPoints(const std::vector<SDL_Point>& input,
                                          std::vector<SDL_Point>& output) {
    // range-based for loop implementation
    for (const auto& point : input) {
        if (point.x >= 0 && point.y >= 0) {
            output.push_back(point);
        }
    }
}

float MovementCalculator::CalculateDistance(const SDL_Point& p1, const SDL_Point& p2) {
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    return std::sqrt(dx * dx + dy * dy);
}

float MovementCalculator::CalculateAngle(const SDL_Point& from, const SDL_Point& to) {
    return std::atan2(to.y - from.y, to.x - from.x);
}

SDL_Point MovementCalculator::ClampToGrid(const SDL_Point& point, int grid_width, int grid_height) {
    SDL_Point clamped;
    clamped.x = std::max(0, std::min(grid_width - 1, point.x));
    clamped.y = std::max(0, std::min(grid_height - 1, point.y));
    return clamped;
}

float MovementCalculator::PerlinNoise(float x, float y) {
    // Simplified Perlin noise implementation
    int xi = static_cast<int>(x) & 255;
    int yi = static_cast<int>(y) & 255;

    float xf = x - static_cast<int>(x);
    float yf = y - static_cast<int>(y);

    // Fade function
    auto fade = [](float t) { return t * t * t * (t * (t * 6 - 15) + 10); };

    float u = fade(xf);
    float v = fade(yf);

    // Simple pseudo-random gradients
    auto hash = [](int x, int y) { return ((x * 73856093) ^ (y * 19349663)) & 0xFF; };

    int aa = hash(xi, yi);
    int ab = hash(xi, yi + 1);
    int ba = hash(xi + 1, yi);
    int bb = hash(xi + 1, yi + 1);

    // Linear interpolation
    auto lerp = [](float a, float b, float t) { return a + t * (b - a); };

    float x1 = lerp(aa / 255.0f, ba / 255.0f, u);
    float x2 = lerp(ab / 255.0f, bb / 255.0f, u);

    return lerp(x1, x2, v);
}

float MovementCalculator::SimplexNoise(float x, float y) {
    // Simplified simplex noise implementation
    const float F2 = 0.5f * (std::sqrt(3.0f) - 1.0f);
    const float G2 = (3.0f - std::sqrt(3.0f)) / 6.0f;

    float s = (x + y) * F2;
    int i = static_cast<int>(std::floor(x + s));
    int j = static_cast<int>(std::floor(y + s));

    float t = (i + j) * G2;
    float X0 = i - t;
    float Y0 = j - t;
    float x0 = x - X0;
    float y0 = y - Y0;

    // Determine which simplex we are in
    int i1, j1;
    if (x0 > y0) {
        i1 = 1; j1 = 0;
    } else {
        i1 = 0; j1 = 1;
    }

    float x1 = x0 - i1 + G2;
    float y1 = y0 - j1 + G2;
    float x2 = x0 - 1.0f + 2.0f * G2;
    float y2 = y0 - 1.0f + 2.0f * G2;

    // Calculate contributions from each corner
    auto contrib = [](float x, float y) -> float {
        float t = 0.5f - x * x - y * y;
        if (t < 0) return 0.0f;
        t *= t;
        return t * t * (x + y); // Simplified gradient
    };

    float n0 = contrib(x0, y0);
    float n1 = contrib(x1, y1);
    float n2 = contrib(x2, y2);

    return 70.0f * (n0 + n1 + n2);
}

} // namespace MovementPatterns