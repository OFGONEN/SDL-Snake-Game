#include "async_obstacle_generator.h"
#include <algorithm>
#include <cmath>

AsyncObstacleGenerator::AsyncObstacleGenerator(int grid_width, int grid_height, size_t thread_pool_size)
    : grid_width(grid_width), grid_height(grid_height), thread_pool_size(thread_pool_size),
      engine(dev()), random_x(0, grid_width - 1), random_y(0, grid_height - 1),
      random_lifetime(5.0f, 15.0f) {
    StartThreadPool();
}

AsyncObstacleGenerator::~AsyncObstacleGenerator() {
    StopThreadPool();
}

std::future<std::vector<std::unique_ptr<Obstacle>>>
AsyncObstacleGenerator::GenerateObstaclesAsync(int fixed_count, int moving_count) {
    return std::async(std::launch::async, [this, fixed_count, moving_count]() {
        return TimeExecution([this, fixed_count, moving_count]() {
            return GenerateObstaclesWorker(fixed_count, moving_count);
        });
    });
}

std::future<std::vector<std::unique_ptr<Obstacle>>>
AsyncObstacleGenerator::GenerateObstaclesAsync(int fixed_count, int moving_count,
                                              const std::vector<SDL_Point>& forbidden_positions) {
    return std::async(std::launch::async, [this, fixed_count, moving_count, forbidden_positions]() {
        return TimeExecution([this, fixed_count, moving_count, &forbidden_positions]() {
            return GenerateObstaclesWorker(fixed_count, moving_count, forbidden_positions);
        });
    });
}

std::future<bool> AsyncObstacleGenerator::ValidateObstaclePlacementAsync(
    const std::vector<SDL_Point>& positions,
    const Snake& snake, const SDL_Point& food) {

    return std::async(std::launch::async, [this, positions, snake, food]() {
        return ValidatePositionsWorker(positions, snake, food);
    });
}

std::future<std::vector<SDL_Point>> AsyncObstacleGenerator::GenerateValidPositionsAsync(
    int count, const std::vector<SDL_Point>& forbidden_positions) {

    return std::async(std::launch::async, [this, count, forbidden_positions]() {
        return GenerateValidPositionsWorker(count, forbidden_positions);
    });
}

void AsyncObstacleGenerator::GenerateObstaclesWithCallback(
    int fixed_count, int moving_count,
    std::function<void(std::vector<std::unique_ptr<Obstacle>>)> callback) {

    EnqueueTask([this, fixed_count, moving_count, callback = std::move(callback)]() {
        auto obstacles = TimeExecution([this, fixed_count, moving_count]() {
            return GenerateObstaclesWorker(fixed_count, moving_count);
        });
        callback(std::move(obstacles));
    });
}

void AsyncObstacleGenerator::GenerateObstaclesWithCallback(
    int fixed_count, int moving_count,
    const std::vector<SDL_Point>& forbidden_positions,
    std::function<void(std::vector<std::unique_ptr<Obstacle>>)> callback) {

    EnqueueTask([this, fixed_count, moving_count, forbidden_positions, callback = std::move(callback)]() {
        auto obstacles = TimeExecution([this, fixed_count, moving_count, &forbidden_positions]() {
            return GenerateObstaclesWorker(fixed_count, moving_count, forbidden_positions);
        });
        callback(std::move(obstacles));
    });
}

std::future<std::vector<std::unique_ptr<Obstacle>>>
AsyncObstacleGenerator::GenerateObstaclesAsync(const GenerationConfig& config) {
    return std::async(std::launch::async, [this, config]() {
        return TimeExecution([this, &config]() {
            return GenerateObstaclesWorker(config);
        });
    });
}

void AsyncObstacleGenerator::StartThreadPool() {
    stop_threads.store(false);
    worker_threads.reserve(thread_pool_size);

    for (size_t i = 0; i < thread_pool_size; ++i) {
        worker_threads.emplace_back(&AsyncObstacleGenerator::ThreadPoolWorker, this);
    }
}

void AsyncObstacleGenerator::StopThreadPool() {
    stop_threads.store(true);
    queue_condition.notify_all();

    for (auto& thread : worker_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    worker_threads.clear();
}

bool AsyncObstacleGenerator::IsThreadPoolRunning() const {
    return !stop_threads.load() && !worker_threads.empty();
}

size_t AsyncObstacleGenerator::GetActiveThreadCount() const {
    return active_threads.load();
}

uint64_t AsyncObstacleGenerator::GetTotalGeneratedObstacles() const {
    return total_generated_obstacles.load();
}

std::chrono::nanoseconds AsyncObstacleGenerator::GetAverageGenerationTime() const {
    uint64_t total_obstacles = total_generated_obstacles.load();
    if (total_obstacles > 0) {
        return std::chrono::nanoseconds(total_generation_time_ns.load() / total_obstacles);
    }
    return std::chrono::nanoseconds(0);
}

std::vector<std::unique_ptr<Obstacle>> AsyncObstacleGenerator::GenerateObstaclesWorker(
    int fixed_count, int moving_count) {

    std::vector<SDL_Point> empty_forbidden;
    return GenerateObstaclesWorker(fixed_count, moving_count, empty_forbidden);
}

std::vector<std::unique_ptr<Obstacle>> AsyncObstacleGenerator::GenerateObstaclesWorker(
    int fixed_count, int moving_count,
    const std::vector<SDL_Point>& forbidden_positions) {

    GenerationConfig config;
    config.fixed_count = fixed_count;
    config.moving_count = moving_count;
    config.forbidden_positions = forbidden_positions;

    return GenerateObstaclesWorker(config);
}

std::vector<std::unique_ptr<Obstacle>> AsyncObstacleGenerator::GenerateObstaclesWorker(
    const GenerationConfig& config) {

    std::vector<std::unique_ptr<Obstacle>> obstacles;
    std::vector<SDL_Point> used_positions = config.forbidden_positions;

    // Generate fixed obstacles
    for (int i = 0; i < config.fixed_count; ++i) {
        int retries = 0;
        SDL_Point position;

        do {
            position = GenerateRandomPosition(used_positions);
            ++retries;
        } while (retries < config.max_retries &&
                 IsPositionValid(position, used_positions) == false);

        if (retries < config.max_retries) {
            float lifetime = GenerateRandomLifetime(config.min_lifetime, config.max_lifetime);
            auto obstacle = std::make_unique<FixedObstacle>(
                position.x, position.y, grid_width, grid_height, lifetime);

            used_positions.push_back(position);
            obstacles.push_back(std::move(obstacle));
            total_generated_obstacles.fetch_add(1);
        }
    }

    // Generate moving obstacles
    for (int i = 0; i < config.moving_count; ++i) {
        int retries = 0;
        SDL_Point position;

        do {
            position = GenerateRandomPosition(used_positions);
            ++retries;
        } while (retries < config.max_retries &&
                 IsPositionValid(position, used_positions) == false);

        if (retries < config.max_retries) {
            MovementPattern pattern = GetRandomMovementPattern(config.preferred_patterns);
            float lifetime = GenerateRandomLifetime(config.min_lifetime, config.max_lifetime);

            auto obstacle = std::make_unique<MovingObstacle>(
                position.x, position.y, grid_width, grid_height, pattern, lifetime);

            used_positions.push_back(position);
            obstacles.push_back(std::move(obstacle));
            total_generated_obstacles.fetch_add(1);
        }
    }

    return obstacles;
}

bool AsyncObstacleGenerator::ValidatePositionsWorker(const std::vector<SDL_Point>& positions,
                                                    const Snake& snake, const SDL_Point& food) {
    // Check if any position conflicts with snake or food
    for (const auto& pos : positions) {
        // Check food position
        if (pos.x == food.x && pos.y == food.y) {
            return false;
        }

        // Check snake head position
        if (pos.x == static_cast<int>(snake.head_x) && pos.y == static_cast<int>(snake.head_y)) {
            return false;
        }

        // Check snake body
        for (const auto& body_segment : snake.body) {
            if (pos.x == body_segment.x && pos.y == body_segment.y) {
                return false;
            }
        }

        // Check if too close to snake (optional safety margin)
        if (IsPositionNearSnake(pos, snake, 2)) {
            return false;
        }
    }

    return true;
}

std::vector<SDL_Point> AsyncObstacleGenerator::GenerateValidPositionsWorker(
    int count, const std::vector<SDL_Point>& forbidden_positions) {

    std::vector<SDL_Point> valid_positions;
    valid_positions.reserve(count);

    int attempts = 0;
    const int max_attempts = count * 10; // Reasonable limit

    while (valid_positions.size() < static_cast<size_t>(count) && attempts < max_attempts) {
        SDL_Point candidate = GenerateRandomPosition();

        if (IsPositionValid(candidate, forbidden_positions) &&
            IsPositionValid(candidate, valid_positions)) {
            valid_positions.push_back(candidate);
        }

        ++attempts;
    }

    return valid_positions;
}

void AsyncObstacleGenerator::ThreadPoolWorker() {
    active_threads.fetch_add(1);

    while (!stop_threads.load()) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_condition.wait(lock, [this]() {
                return stop_threads.load() || !task_queue.empty();
            });

            if (stop_threads.load()) {
                break;
            }

            if (!task_queue.empty()) {
                task = std::move(task_queue.front());
                task_queue.pop();
            }
        }

        if (task) {
            task();
        }
    }

    active_threads.fetch_sub(1);
}

SDL_Point AsyncObstacleGenerator::GenerateRandomPosition() const {
    return {random_x(engine), random_y(engine)};
}

SDL_Point AsyncObstacleGenerator::GenerateRandomPosition(const std::vector<SDL_Point>& forbidden) const {
    SDL_Point position;
    int attempts = 0;
    const int max_attempts = 100;

    do {
        position = GenerateRandomPosition();
        ++attempts;
    } while (attempts < max_attempts && !IsPositionValid(position, forbidden));

    return position;
}

MovementPattern AsyncObstacleGenerator::GetRandomMovementPattern() const {
    std::uniform_int_distribution<int> pattern_dist(0, 4);
    int pattern_index = pattern_dist(engine);

    switch (pattern_index) {
    case 0: return MovementPattern::LINEAR_HORIZONTAL;
    case 1: return MovementPattern::LINEAR_VERTICAL;
    case 2: return MovementPattern::CIRCULAR;
    case 3: return MovementPattern::ZIGZAG;
    case 4: return MovementPattern::RANDOM_WALK;
    default: return MovementPattern::LINEAR_HORIZONTAL;
    }
}

MovementPattern AsyncObstacleGenerator::GetRandomMovementPattern(
    const std::vector<MovementPattern>& preferred) const {

    if (preferred.empty()) {
        return GetRandomMovementPattern();
    }

    std::uniform_int_distribution<size_t> pattern_dist(0, preferred.size() - 1);
    return preferred[pattern_dist(engine)];
}

float AsyncObstacleGenerator::GenerateRandomLifetime(float min_lifetime, float max_lifetime) const {
    std::uniform_real_distribution<float> lifetime_dist(min_lifetime, max_lifetime);
    return lifetime_dist(engine);
}

bool AsyncObstacleGenerator::IsPositionValid(const SDL_Point& pos,
                                           const std::vector<SDL_Point>& forbidden) const {
    // Check bounds
    if (pos.x < 0 || pos.x >= grid_width || pos.y < 0 || pos.y >= grid_height) {
        return false;
    }

    // Check forbidden positions
    return std::find_if(forbidden.begin(), forbidden.end(),
                       [&pos](const SDL_Point& forbidden_pos) {
                           return pos.x == forbidden_pos.x && pos.y == forbidden_pos.y;
                       }) == forbidden.end();
}

bool AsyncObstacleGenerator::IsPositionNearSnake(const SDL_Point& pos, const Snake& snake,
                                               int min_distance) const {
    // Check distance from snake head
    int head_x = static_cast<int>(snake.head_x);
    int head_y = static_cast<int>(snake.head_y);

    int distance_to_head = std::abs(pos.x - head_x) + std::abs(pos.y - head_y);
    if (distance_to_head < min_distance) {
        return true;
    }

    // Check distance from snake body segments
    for (const auto& segment : snake.body) {
        int distance_to_segment = std::abs(pos.x - segment.x) + std::abs(pos.y - segment.y);
        if (distance_to_segment < min_distance) {
            return true;
        }
    }

    return false;
}