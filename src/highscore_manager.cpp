#include "highscore_manager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <stdexcept>

HighScoreManager::HighScoreManager(const std::string& filename)
    : filename_(filename) {
    LoadScores();
}

HighScoreManager::~HighScoreManager() = default;

HighScoreManager::HighScoreManager(HighScoreManager&& other) noexcept
    : filename_(std::move(other.filename_)), scores_(std::move(other.scores_)) {}

HighScoreManager& HighScoreManager::operator=(HighScoreManager&& other) noexcept {
    if (this != &other) {
        filename_ = std::move(other.filename_);
        scores_ = std::move(other.scores_);
    }
    return *this;
}

void HighScoreManager::LoadScores() {
    scores_.clear();

    if (!FileExists(filename_)) {
        CreateEmptyFile();
        return;
    }

    std::ifstream file(filename_);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open scores file: " << filename_ << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string name, scoreStr, timestamp;

        if (std::getline(ss, name, ',') &&
            std::getline(ss, scoreStr, ',') &&
            std::getline(ss, timestamp)) {

            try {
                int score = std::stoi(scoreStr);
                scores_.emplace_back(name, score, timestamp);
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid score entry: " << line << std::endl;
            }
        }
    }

    SortScores();
}

void HighScoreManager::SaveScore(const std::string& name, int score) {
    if (name.empty()) {
        throw std::invalid_argument("Player name cannot be empty");
    }

    std::string timestamp = GetCurrentTimestamp();
    scores_.emplace_back(name, score, timestamp);

    SortScores();
    TrimScores();

    std::ofstream file(filename_);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open scores file for writing: " + filename_);
    }

    file << "PlayerName,Score,Timestamp\n";
    for (const auto& entry : scores_) {
        file << entry.playerName << "," << entry.score << "," << entry.timestamp << "\n";
    }

    if (file.fail()) {
        throw std::runtime_error("Error writing to scores file: " + filename_);
    }
}

std::vector<ScoreEntry> HighScoreManager::GetTopScores(std::size_t count) const {
    std::size_t actualCount = std::min(count, scores_.size());
    return std::vector<ScoreEntry>(scores_.begin(), scores_.begin() + actualCount);
}

std::shared_ptr<const std::vector<ScoreEntry>> HighScoreManager::GetSharedTopScores(std::size_t count) const {
    std::size_t actualCount = std::min(count, scores_.size());
    auto sharedScores = std::make_shared<std::vector<ScoreEntry>>(
        scores_.begin(), scores_.begin() + actualCount);
    return sharedScores;
}

bool HighScoreManager::IsNewHighestScore(int score) const {
    if (scores_.empty()) {
        return true; // First score ever is always the highest
    }

    // Check if this score is higher than the current highest score
    return score > scores_.front().score;
}

std::size_t HighScoreManager::GetScoreCount() const {
    return scores_.size();
}

void HighScoreManager::ClearScores() {
    scores_.clear();
    CreateEmptyFile();
}

std::string HighScoreManager::GetCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d_%H:%M:%S");
    return ss.str();
}

void HighScoreManager::SortScores() {
    std::sort(scores_.begin(), scores_.end(),
              [](const ScoreEntry& a, const ScoreEntry& b) {
                  return a.score > b.score;
              });
}

void HighScoreManager::TrimScores() {
    if (scores_.size() > kMaxScores) {
        scores_.resize(kMaxScores);
    }
}

bool HighScoreManager::FileExists(const std::string& filename) const {
    std::ifstream file(filename);
    return file.good();
}

void HighScoreManager::CreateEmptyFile() const {
    std::ofstream file(filename_);
    if (file.is_open()) {
        file << "PlayerName,Score,Timestamp\n";
    }
}