#ifndef SCORE_ENTRY_H
#define SCORE_ENTRY_H

#include <string>

struct ScoreEntry {
public:
    std::string playerName;
    int score;
    std::string timestamp;

    ScoreEntry() = default;
    ScoreEntry(const std::string& name, int s, const std::string& time);

    ScoreEntry(const ScoreEntry& other) = default;
    ScoreEntry& operator=(const ScoreEntry& other) = default;
    ScoreEntry(ScoreEntry&& other) noexcept = default;
    ScoreEntry& operator=(ScoreEntry&& other) noexcept = default;

    ~ScoreEntry() = default;

    bool operator<(const ScoreEntry& other) const;
    bool operator>(const ScoreEntry& other) const;
    bool operator==(const ScoreEntry& other) const;
};

#endif