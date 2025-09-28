#ifndef ASYNC_OBSTACLE_GENERATOR_H
#define ASYNC_OBSTACLE_GENERATOR_H

#include "obstacle.h"
#include "fixed_obstacle.h"
#include "moving_obstacle.h"
#include "snake.h"
#include <future>
#include <functional>
#include <vector>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <random>

class AsyncObstacleGenerator {
public:
    explicit AsyncObstacleGenerator(int grid_width, int grid_height, size_t thread_pool_size = 2);
    ~AsyncObstacleGenerator();

    // Rule of Five - no copy, move only
    AsyncObstacleGenerator(const AsyncObstacleGenerator& other) = delete;
    AsyncObstacleGenerator& operator=(const AsyncObstacleGenerator& other) = delete;
    AsyncObstacleGenerator(AsyncObstacleGenerator&& other) noexcept = default;
    AsyncObstacleGenerator& operator=(AsyncObstacleGenerator&& other) noexcept = default;

    // Promise/Future pattern for obstacle generation
    std::future<std::vector<std::unique_ptr<Obstacle>>>
    GenerateObstaclesAsync(int fixed_count, int moving_count);

    std::future<std::vector<std::unique_ptr<Obstacle>>>
    GenerateObstaclesAsync(int fixed_count, int moving_count,
                          const std::vector<SDL_Point>& forbidden_positions);

    // Async operations with std::async
    std::future<bool> ValidateObstaclePlacementAsync(
        const std::vector<SDL_Point>& positions,
        const Snake& snake, const SDL_Point& food);

    std::future<std::vector<SDL_Point>> GenerateValidPositionsAsync(
        int count, const std::vector<SDL_Point>& forbidden_positions);

    // Callback-based async generation
    void GenerateObstaclesWithCallback(
        int fixed_count, int moving_count,
        std::function<void(std::vector<std::unique_ptr<Obstacle>>)> callback);

    void GenerateObstaclesWithCallback(
        int fixed_count, int moving_count,
        const std::vector<SDL_Point>& forbidden_positions,
        std::function<void(std::vector<std::unique_ptr<Obstacle>>)> callback);

    // Advanced async generation with configuration
    struct GenerationConfig {
        int fixed_count{0};
        int moving_count{0};
        std::vector<SDL_Point> forbidden_positions;
        std::vector<MovementPattern> preferred_patterns;
        float min_lifetime{5.0f};
        float max_lifetime{15.0f};
        bool avoid_snake_path{true};
        int max_retries{10};
    };

    std::future<std::vector<std::unique_ptr<Obstacle>>>
    GenerateObstaclesAsync(const GenerationConfig& config);

    // Thread pool management
    void StartThreadPool();
    void StopThreadPool();
    bool IsThreadPoolRunning() const;
    size_t GetActiveThreadCount() const;

    // Performance monitoring
    uint64_t GetTotalGeneratedObstacles() const;
    std::chrono::nanoseconds GetAverageGenerationTime() const;

private:
    const int grid_width;
    const int grid_height;
    const size_t thread_pool_size;

    // Random number generation (mutable for const methods)
    mutable std::random_device dev;
    mutable std::mt19937 engine;
    mutable std::uniform_int_distribution<int> random_x;
    mutable std::uniform_int_distribution<int> random_y;
    mutable std::uniform_real_distribution<float> random_lifetime;

    // Thread pool components
    std::vector<std::thread> worker_threads;
    std::queue<std::function<void()>> task_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_condition;
    std::atomic<bool> stop_threads{false};
    std::atomic<size_t> active_threads{0};

    // Performance tracking
    std::atomic<uint64_t> total_generated_obstacles{0};
    std::atomic<uint64_t> total_generation_time_ns{0};

    // Worker methods for async operations
    std::vector<std::unique_ptr<Obstacle>> GenerateObstaclesWorker(
        int fixed_count, int moving_count);

    std::vector<std::unique_ptr<Obstacle>> GenerateObstaclesWorker(
        int fixed_count, int moving_count,
        const std::vector<SDL_Point>& forbidden_positions);

    std::vector<std::unique_ptr<Obstacle>> GenerateObstaclesWorker(
        const GenerationConfig& config);

    bool ValidatePositionsWorker(const std::vector<SDL_Point>& positions,
                                const Snake& snake, const SDL_Point& food);

    std::vector<SDL_Point> GenerateValidPositionsWorker(
        int count, const std::vector<SDL_Point>& forbidden_positions);

    // Thread pool worker function
    void ThreadPoolWorker();

    // Helper methods
    SDL_Point GenerateRandomPosition() const;
    SDL_Point GenerateRandomPosition(const std::vector<SDL_Point>& forbidden) const;
    MovementPattern GetRandomMovementPattern() const;
    MovementPattern GetRandomMovementPattern(const std::vector<MovementPattern>& preferred) const;
    float GenerateRandomLifetime(float min_lifetime, float max_lifetime) const;

    bool IsPositionValid(const SDL_Point& pos, const std::vector<SDL_Point>& forbidden) const;
    bool IsPositionNearSnake(const SDL_Point& pos, const Snake& snake, int min_distance = 3) const;

    // Template method for enqueuing tasks
    template<typename F>
    void EnqueueTask(F&& task);

    // Performance timing helper
    template<typename F>
    auto TimeExecution(F&& func) -> decltype(func());
};

// Template method implementations
template<typename F>
void AsyncObstacleGenerator::EnqueueTask(F&& task) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        task_queue.emplace(std::forward<F>(task));
    }
    queue_condition.notify_one();
}

template<typename F>
auto AsyncObstacleGenerator::TimeExecution(F&& func) -> decltype(func()) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = func();
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    total_generation_time_ns.fetch_add(duration.count());

    return result;
}

#endif