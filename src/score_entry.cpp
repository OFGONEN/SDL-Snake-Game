#include "score_entry.h"

ScoreEntry::ScoreEntry(const std::string& name, int s, const std::string& time)
    : playerName(name), score(s), timestamp(time) {}

bool ScoreEntry::operator<(const ScoreEntry& other) const {
    return score < other.score;
}

bool ScoreEntry::operator>(const ScoreEntry& other) const {
    return score > other.score;
}

bool ScoreEntry::operator==(const ScoreEntry& other) const {
    return playerName == other.playerName &&
           score == other.score &&
           timestamp == other.timestamp;
}