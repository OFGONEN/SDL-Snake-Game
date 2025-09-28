#include "threaded_obstacle_manager.h"
#include <algorithm>

ThreadedObstacleManager::ThreadedObstacleManager(int grid_width, int grid_height)
    : ObstacleManager(grid_width, grid_height),
      thread_start_time(std::chrono::steady_clock::now()),
      performance_monitor(std::make_unique<PerformanceMonitor>()) {
}

ThreadedObstacleManager::~ThreadedObstacleManager() {
    StopLifetimeThread();
}

void ThreadedObstacleManager::StartLifetimeThread() {
    if (!thread_running.load()) {
        shutdown_requested.store(false);
        thread_running.store(true);
        lifetime_thread = std::thread(&ThreadedObstacleManager::LifetimeWorkerThread, this);
    }
}

void ThreadedObstacleManager::StopLifetimeThread() {
    if (thread_running.load()) {
        shutdown_requested.store(true);
        lifetime_condition.notify_all();

        if (lifetime_thread.joinable()) {
            lifetime_thread.join();
        }

        thread_running.store(false);
    }
}

void ThreadedObstacleManager::UpdateObstacleMovement() {
    SafelyUpdateMovingObstacles();
}

bool ThreadedObstacleManager::CheckCollisionWithPoint(int x, int y) const {
    bool result = false;
    TrackCollisionTiming([this, x, y, &result]() {
        std::shared_lock<std::shared_mutex> lock(obstacles_mutex);
        result = ObstacleManager::CheckCollisionWithPoint(x, y);
        return result;
    }, result);
    return result;
}

bool ThreadedObstacleManager::CheckCollisionWithSnake(const Snake& snake) const {
    bool result = false;
    TrackCollisionTiming([this, &snake, &result]() {
        std::shared_lock<std::shared_mutex> lock(obstacles_mutex);
        result = ObstacleManager::CheckCollisionWithSnake(snake);
        return result;
    }, result);
    return result;
}

bool ThreadedObstacleManager::IsValidFoodPosition(int x, int y) const {
    bool result = false;
    TrackCollisionTiming([this, x, y, &result]() {
        std::shared_lock<std::shared_mutex> lock(obstacles_mutex);
        result = ObstacleManager::IsValidFoodPosition(x, y);
        return result;
    }, result);
    return result;
}

std::size_t ThreadedObstacleManager::GetObstacleCountSafe() const {
    std::shared_lock<std::shared_mutex> lock(obstacles_mutex);
    return ObstacleManager::GetObstacleCount();
}

std::size_t ThreadedObstacleManager::GetFixedObstacleCountSafe() const {
    std::shared_lock<std::shared_mutex> lock(obstacles_mutex);
    return ObstacleManager::GetFixedObstacleCount();
}

std::size_t ThreadedObstacleManager::GetMovingObstacleCountSafe() const {
    std::shared_lock<std::shared_mutex> lock(obstacles_mutex);
    return ObstacleManager::GetMovingObstacleCount();
}

std::future<std::size_t> ThreadedObstacleManager::CleanupExpiredAsync() {
    return std::async(std::launch::async, [this]() -> std::size_t {
        std::unique_lock<std::shared_mutex> lock(obstacles_mutex);
        std::size_t initial_count = obstacles.size();
        SafelyCleanupExpired();
        return initial_count - obstacles.size();
    });
}

void ThreadedObstacleManager::LifetimeWorkerThread() {
    const auto update_interval = std::chrono::milliseconds(100); // 100ms intervals

    while (!shutdown_requested.load()) {
        auto start_time = std::chrono::high_resolution_clock::now();

        ProcessAtomicLifetimeUpdates();
        lifetime_updates_count.fetch_add(1);

        auto end_time = std::chrono::high_resolution_clock::now();
        auto update_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
        performance_monitor->RecordLifetimeUpdate(update_duration);

        // Monitor performance periodically
        if (lifetime_updates_count.load() % 100 == 0) { // Every 10 seconds
            performance_monitor->MonitorLifetimeThreadPerformance();
        }

        // Clean up expired obstacles periodically
        if (lifetime_updates_count.load() % 50 == 0) { // Every 5 seconds
            auto cleanup_future = CleanupExpiredAsync();
            // Don't wait for cleanup to complete, let it run asynchronously
        }

        // Wait for next update interval or shutdown
        std::unique_lock<std::mutex> lock(lifetime_mutex);
        lifetime_condition.wait_for(lock, update_interval,
                                   [this]() { return shutdown_requested.load(); });
    }
}

void ThreadedObstacleManager::UpdateAllLifetimesAtomic(float delta_time) {
    std::shared_lock<std::shared_mutex> lock(obstacles_mutex);
    for (auto& obstacle : obstacles) {
        obstacle->DecrementLifetime(delta_time);
    }
}

void ThreadedObstacleManager::NotifyLifetimeThread() {
    lifetime_condition.notify_one();
}

void ThreadedObstacleManager::SafelyUpdateMovingObstacles() {
    std::shared_lock<std::shared_mutex> lock(obstacles_mutex);
    for (auto& obstacle : obstacles) {
        if (obstacle->GetType() == ObstacleType::MOVING) {
            obstacle->Update();
        }
    }
}

void ThreadedObstacleManager::SafelyCleanupExpired() {
    // This method assumes the caller already holds a unique lock
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
                      [](const std::unique_ptr<Obstacle>& obstacle) {
                          return obstacle->IsExpired();
                      }),
        obstacles.end()
    );
}

const PerformanceMonitor& ThreadedObstacleManager::GetPerformanceMonitor() const {
    return *performance_monitor;
}

void ThreadedObstacleManager::LogPerformanceReport() const {
    performance_monitor->LogPerformanceReport();
}

bool ThreadedObstacleManager::IsPerformanceAcceptable() const {
    return performance_monitor->IsPerformanceAcceptable();
}

void ThreadedObstacleManager::TrackCollisionTiming(std::function<bool()> collision_func, bool& result) const {
    auto start_time = std::chrono::high_resolution_clock::now();

    try {
        result = collision_func();
    } catch (...) {
        // Track contention if lock acquisition fails
        performance_monitor->IncrementLockContentionCount();
        throw;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    performance_monitor->TrackCollisionCheckTiming(duration);
}

void ThreadedObstacleManager::TrackSyncOverhead(std::function<void()> sync_func) const {
    auto start_time = std::chrono::high_resolution_clock::now();

    sync_func();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto overhead = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    performance_monitor->MeasureThreadSynchronizationOverhead(overhead);
}