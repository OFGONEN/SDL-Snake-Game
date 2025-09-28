#include "performance_monitor.h"
#include <iostream>
#include <iomanip>

PerformanceMonitor::PerformanceMonitor()
    : monitor_start_time(std::chrono::steady_clock::now()),
      last_reset_time(std::chrono::steady_clock::now()) {
}

void PerformanceMonitor::MonitorLifetimeThreadPerformance() {
    // This method can be called periodically to analyze performance trends
    std::lock_guard<std::mutex> lock(stats_mutex);

    auto current_time = std::chrono::steady_clock::now();
    auto total_runtime = current_time - monitor_start_time;

    // Calculate performance metrics over time
    double runtime_seconds = std::chrono::duration<double>(total_runtime).count();
    if (runtime_seconds > 0.0) {
        double updates_per_sec = static_cast<double>(lifetime_updates_count.load()) / runtime_seconds;
        double checks_per_sec = static_cast<double>(collision_checks_count.load()) / runtime_seconds;

        // Log performance summary if needed
        if (lifetime_updates_count.load() % 1000 == 0) { // Every 1000 updates
            std::cout << "Performance Summary - Updates/sec: " << std::fixed << std::setprecision(2)
                      << updates_per_sec << ", Checks/sec: " << checks_per_sec << std::endl;
        }
    }
}

void PerformanceMonitor::TrackCollisionCheckTiming(std::chrono::nanoseconds duration) {
    collision_checks_count.fetch_add(1);
    total_collision_check_time_ns.fetch_add(duration.count());
}

void PerformanceMonitor::MeasureThreadSynchronizationOverhead(std::chrono::nanoseconds overhead) {
    thread_sync_overhead_count.fetch_add(1);
    total_thread_sync_overhead_ns.fetch_add(overhead.count());
}

void PerformanceMonitor::RecordLifetimeUpdate(std::chrono::nanoseconds update_duration) {
    lifetime_updates_count.fetch_add(1);
    total_lifetime_update_time_ns.fetch_add(update_duration.count());
}

std::chrono::nanoseconds PerformanceMonitor::GetAverageLifetimeUpdateTime() const {
    return CalculateAverageTime(total_lifetime_update_time_ns.load(), lifetime_updates_count.load());
}

std::chrono::nanoseconds PerformanceMonitor::GetAverageCollisionCheckTime() const {
    return CalculateAverageTime(total_collision_check_time_ns.load(), collision_checks_count.load());
}

std::chrono::nanoseconds PerformanceMonitor::GetAverageThreadSyncOverhead() const {
    return CalculateAverageTime(total_thread_sync_overhead_ns.load(), thread_sync_overhead_count.load());
}

double PerformanceMonitor::GetThreadEfficiencyRatio() const {
    std::lock_guard<std::mutex> lock(stats_mutex);
    return CalculateEfficiencyRatio();
}

uint64_t PerformanceMonitor::GetTotalLifetimeUpdates() const {
    return lifetime_updates_count.load();
}

uint64_t PerformanceMonitor::GetTotalCollisionChecks() const {
    return collision_checks_count.load();
}

double PerformanceMonitor::GetUpdatesPerSecond() const {
    std::lock_guard<std::mutex> lock(stats_mutex);
    auto current_time = std::chrono::steady_clock::now();
    auto elapsed = current_time - last_reset_time;
    double seconds = std::chrono::duration<double>(elapsed).count();

    if (seconds > 0.0) {
        return static_cast<double>(lifetime_updates_count.load()) / seconds;
    }
    return 0.0;
}

void PerformanceMonitor::IncrementLockContentionCount() {
    lock_contention_count.fetch_add(1);
}

uint64_t PerformanceMonitor::GetLockContentionCount() const {
    return lock_contention_count.load();
}

double PerformanceMonitor::GetContentionRatio() const {
    uint64_t total_operations = collision_checks_count.load() + lifetime_updates_count.load();
    if (total_operations > 0) {
        return static_cast<double>(lock_contention_count.load()) / static_cast<double>(total_operations);
    }
    return 0.0;
}

void PerformanceMonitor::ResetCounters() {
    std::lock_guard<std::mutex> lock(stats_mutex);

    lifetime_updates_count.store(0);
    total_lifetime_update_time_ns.store(0);
    collision_checks_count.store(0);
    total_collision_check_time_ns.store(0);
    thread_sync_overhead_count.store(0);
    total_thread_sync_overhead_ns.store(0);
    lock_contention_count.store(0);

    last_reset_time = std::chrono::steady_clock::now();
}

std::chrono::nanoseconds PerformanceMonitor::CalculateAverageTime(uint64_t total_time_ns, uint64_t count) const {
    if (count > 0) {
        return std::chrono::nanoseconds(total_time_ns / count);
    }
    return std::chrono::nanoseconds(0);
}

double PerformanceMonitor::CalculateEfficiencyRatio() const {
    uint64_t total_operations = collision_checks_count.load() + lifetime_updates_count.load();
    uint64_t contention_events = lock_contention_count.load();

    if (total_operations > 0) {
        double contention_ratio = static_cast<double>(contention_events) / static_cast<double>(total_operations);
        return 1.0 - contention_ratio; // Higher efficiency = lower contention
    }
    return 1.0; // Perfect efficiency if no operations yet
}

bool PerformanceMonitor::IsPerformanceAcceptable() const {
    auto avg_update_time = GetAverageLifetimeUpdateTime();
    auto avg_collision_time = GetAverageCollisionCheckTime();
    double efficiency = GetThreadEfficiencyRatio();

    return (avg_update_time <= kMaxAcceptableUpdateTime) &&
           (avg_collision_time <= kMaxAcceptableCollisionTime) &&
           (efficiency >= kMinAcceptableEfficiencyRatio);
}

std::vector<std::string> PerformanceMonitor::GetPerformanceWarnings() const {
    std::vector<std::string> warnings;

    auto avg_update_time = GetAverageLifetimeUpdateTime();
    if (avg_update_time > kMaxAcceptableUpdateTime) {
        warnings.push_back("Lifetime update time exceeds acceptable threshold");
    }

    auto avg_collision_time = GetAverageCollisionCheckTime();
    if (avg_collision_time > kMaxAcceptableCollisionTime) {
        warnings.push_back("Collision check time exceeds acceptable threshold");
    }

    double efficiency = GetThreadEfficiencyRatio();
    if (efficiency < kMinAcceptableEfficiencyRatio) {
        warnings.push_back("Thread efficiency below acceptable ratio");
    }

    double contention_ratio = GetContentionRatio();
    if (contention_ratio > 0.1) { // More than 10% contention
        warnings.push_back("High lock contention detected");
    }

    return warnings;
}

void PerformanceMonitor::LogPerformanceReport() const {
    std::cout << "\n=== Thread Performance Report ===" << std::endl;
    std::cout << "Lifetime Updates: " << GetTotalLifetimeUpdates() << std::endl;
    std::cout << "Collision Checks: " << GetTotalCollisionChecks() << std::endl;
    std::cout << "Updates per Second: " << std::fixed << std::setprecision(2) << GetUpdatesPerSecond() << std::endl;

    auto avg_update = GetAverageLifetimeUpdateTime();
    auto avg_collision = GetAverageCollisionCheckTime();
    auto avg_sync = GetAverageThreadSyncOverhead();

    std::cout << "Average Update Time: " << avg_update.count() / 1000 << " μs" << std::endl;
    std::cout << "Average Collision Time: " << avg_collision.count() / 1000 << " μs" << std::endl;
    std::cout << "Average Sync Overhead: " << avg_sync.count() / 1000 << " μs" << std::endl;

    std::cout << "Thread Efficiency: " << std::fixed << std::setprecision(1)
              << (GetThreadEfficiencyRatio() * 100.0) << "%" << std::endl;
    std::cout << "Lock Contention: " << GetLockContentionCount()
              << " (" << std::fixed << std::setprecision(1)
              << (GetContentionRatio() * 100.0) << "%)" << std::endl;

    auto warnings = GetPerformanceWarnings();
    if (!warnings.empty()) {
        std::cout << "\nPerformance Warnings:" << std::endl;
        for (const auto& warning : warnings) {
            std::cout << "  - " << warning << std::endl;
        }
    }
    std::cout << "=================================" << std::endl;
}