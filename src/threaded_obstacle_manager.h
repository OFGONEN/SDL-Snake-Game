#ifndef THREADED_OBSTACLE_MANAGER_H
#define THREADED_OBSTACLE_MANAGER_H

#include "obstacle_manager.h"
#include <thread>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <future>
#include <chrono>

class ThreadedObstacleManager : public ObstacleManager {
public:
    explicit ThreadedObstacleManager(int grid_width, int grid_height);
    ~ThreadedObstacleManager();

    // Threading lifecycle management
    void StartLifetimeThread();
    void StopLifetimeThread();

    // Override base class methods with thread-safe versions
    void UpdateObstacleMovement() override;
    bool CheckCollisionWithPoint(int x, int y) const override;
    bool CheckCollisionWithSnake(const Snake& snake) const override;
    bool IsValidFoodPosition(int x, int y) const override;

    // Thread-safe getters
    std::size_t GetObstacleCountSafe() const;
    std::size_t GetFixedObstacleCountSafe() const;
    std::size_t GetMovingObstacleCountSafe() const;

    // Asynchronous operations
    std::future<std::size_t> CleanupExpiredAsync();

private:
    // Threading components
    std::thread lifetime_thread;
    std::atomic<bool> shutdown_requested{false};
    std::atomic<bool> thread_running{false};

    // Synchronization primitives
    mutable std::shared_mutex obstacles_mutex; // Reader-writer lock
    std::condition_variable lifetime_condition;
    std::mutex lifetime_mutex;

    // Thread worker methods
    void LifetimeWorkerThread();
    void UpdateAllLifetimesAtomic(float delta_time);
    void NotifyLifetimeThread();

    // Thread-safe internal operations
    void SafelyUpdateMovingObstacles();
    void SafelyCleanupExpired();

    // Atomic lifetime management implementation
    void ProcessAtomicLifetimeUpdates() {
        // Lock-free atomic operations on all obstacles
        std::shared_lock<std::shared_mutex> lock(obstacles_mutex);
        for (auto& obstacle : obstacles) {
            obstacle->DecrementLifetime(0.1f);  // 100ms intervals
        }
    }

    // Performance monitoring
    std::atomic<uint64_t> lifetime_updates_count{0};
    std::chrono::steady_clock::time_point thread_start_time;
};

#endif