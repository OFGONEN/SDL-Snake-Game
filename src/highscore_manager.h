#ifndef HIGHSCORE_MANAGER_H
#define HIGHSCORE_MANAGER_H

#include "score_entry.h"
#include <string>
#include <vector>
#include <memory>

class HighScoreManager {
public:
    explicit HighScoreManager(const std::string& filename = "scores.txt");
    ~HighScoreManager();

    HighScoreManager(const HighScoreManager& other) = delete;
    HighScoreManager& operator=(const HighScoreManager& other) = delete;
    HighScoreManager(HighScoreManager&& other) noexcept;
    HighScoreManager& operator=(HighScoreManager&& other) noexcept;

    void LoadScores();
    void SaveScore(const std::string& name, int score);
    std::vector<ScoreEntry> GetTopScores(std::size_t count = 10) const;
    bool IsHighScore(int score) const;
    std::size_t GetScoreCount() const;
    void ClearScores();

private:
    std::string filename_;
    std::vector<ScoreEntry> scores_;
    static constexpr std::size_t kMaxScores = 10;

    std::string GetCurrentTimestamp() const;
    void SortScores();
    void TrimScores();
    bool FileExists(const std::string& filename) const;
    void CreateEmptyFile() const;
};

#endif