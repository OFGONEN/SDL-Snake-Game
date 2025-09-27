#include "score_entry.h"

ScoreEntry::ScoreEntry(const std::string& name, int s, const std::string& time)
    : playerName(name), score(s), timestamp(time) {}