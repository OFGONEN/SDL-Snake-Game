#ifndef SCORE_ENTRY_H
#define SCORE_ENTRY_H

#include <string>
#include <utility>

struct ScoreEntry {
public:
    std::string playerName;
    int score;
    std::string timestamp;

    ScoreEntry() = default;
    ScoreEntry(const std::string& name, int s, const std::string& time);

    // Perfect forwarding constructor for efficient construction
    template<typename Name, typename Time>
    ScoreEntry(Name&& name, int s, Time&& time)
        : playerName(std::forward<Name>(name)), score(s), timestamp(std::forward<Time>(time)) {}

    ScoreEntry(const ScoreEntry& other) = default;
    ScoreEntry& operator=(const ScoreEntry& other) = default;
    ScoreEntry(ScoreEntry&& other) noexcept = default;
    ScoreEntry& operator=(ScoreEntry&& other) noexcept = default;

    ~ScoreEntry() = default;
};

#endif