#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

#include <atomic>
#include <chrono>
#include <mutex>
#include <vector>
#include <string>

class PerformanceMonitor {
public:
    explicit PerformanceMonitor();
    ~PerformanceMonitor() = default;

    // Rule of Five - no copy, move only
    PerformanceMonitor(const PerformanceMonitor& other) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor& other) = delete;
    PerformanceMonitor(PerformanceMonitor&& other) noexcept = default;
    PerformanceMonitor& operator=(PerformanceMonitor&& other) noexcept = default;

    // Thread performance monitoring
    void MonitorLifetimeThreadPerformance();
    void TrackCollisionCheckTiming(std::chrono::nanoseconds duration);
    void MeasureThreadSynchronizationOverhead(std::chrono::nanoseconds overhead);
    void RecordLifetimeUpdate(std::chrono::nanoseconds update_duration);

    // Performance metrics getters
    std::chrono::nanoseconds GetAverageLifetimeUpdateTime() const;
    std::chrono::nanoseconds GetAverageCollisionCheckTime() const;
    std::chrono::nanoseconds GetAverageThreadSyncOverhead() const;
    double GetThreadEfficiencyRatio() const;

    // Performance statistics
    uint64_t GetTotalLifetimeUpdates() const;
    uint64_t GetTotalCollisionChecks() const;
    double GetUpdatesPerSecond() const;

    // Thread contention analysis
    void IncrementLockContentionCount();
    uint64_t GetLockContentionCount() const;
    double GetContentionRatio() const;

    // Reset performance counters
    void ResetCounters();

private:
    // Timing measurements with atomic counters
    std::atomic<uint64_t> lifetime_updates_count{0};
    std::atomic<uint64_t> total_lifetime_update_time_ns{0};
    std::atomic<uint64_t> collision_checks_count{0};
    std::atomic<uint64_t> total_collision_check_time_ns{0};
    std::atomic<uint64_t> thread_sync_overhead_count{0};
    std::atomic<uint64_t> total_thread_sync_overhead_ns{0};
    std::atomic<uint64_t> lock_contention_count{0};

    // Performance monitoring state
    std::chrono::steady_clock::time_point monitor_start_time;
    std::chrono::steady_clock::time_point last_reset_time;

    // Thread safety for statistics calculation
    mutable std::mutex stats_mutex;

    // Helper methods for safe average calculations
    std::chrono::nanoseconds CalculateAverageTime(uint64_t total_time_ns, uint64_t count) const;
    double CalculateEfficiencyRatio() const;

    // Performance thresholds (configurable)
    static constexpr std::chrono::nanoseconds kMaxAcceptableUpdateTime{1000000}; // 1ms
    static constexpr std::chrono::nanoseconds kMaxAcceptableCollisionTime{100000}; // 100μs
    static constexpr double kMinAcceptableEfficiencyRatio = 0.8; // 80%

public:
    // Performance analysis methods
    bool IsPerformanceAcceptable() const;
    std::vector<std::string> GetPerformanceWarnings() const;
    void LogPerformanceReport() const;
};

#endif