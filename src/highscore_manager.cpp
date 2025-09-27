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

    std::string sanitizedName = SanitizePlayerName(name);
    if (!IsValidPlayerName(sanitizedName)) {
        throw std::invalid_argument("Invalid player name: " + name);
    }

    std::string timestamp = GetCurrentTimestamp();
    scores_.emplace_back(sanitizedName, score, timestamp);

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

std::string HighScoreManager::FormatTimestamp(const std::string& timestamp) const {
    if (timestamp.empty()) return "Unknown";

    // Convert from "2025-09-28_02:12:53" to "Sep 28, 2025 02:12"
    if (timestamp.length() >= 19) {
        std::string year = timestamp.substr(0, 4);
        std::string month = timestamp.substr(5, 2);
        std::string day = timestamp.substr(8, 2);
        std::string time = timestamp.substr(11, 5); // HH:MM

        // Convert month number to name
        const std::vector<std::string> months = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };

        int monthNum = std::stoi(month) - 1;
        if (monthNum >= 0 && monthNum < 12) {
            return months[monthNum] + " " + day + ", " + year + " " + time;
        }
    }

    return timestamp; // Fallback to original if parsing fails
}


void HighScoreManager::CreateEmptyFile() const {
    std::ofstream file(filename_);
    if (file.is_open()) {
        file << "PlayerName,Score,Timestamp\n";
    }
}

bool HighScoreManager::IsValidPlayerName(const std::string& name) {
    if (name.empty() || name.length() > 20) {
        return false;
    }

    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != ' ') {
            return false;
        }
    }

    std::string trimmed = name;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

    return !trimmed.empty();
}

std::string HighScoreManager::SanitizePlayerName(const std::string& name) {
    std::string sanitized = name;

    sanitized.erase(0, sanitized.find_first_not_of(" \t"));
    sanitized.erase(sanitized.find_last_not_of(" \t") + 1);

    for (char& c : sanitized) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != ' ') {
            c = '_';
        }
    }

    if (sanitized.length() > 20) {
        sanitized = sanitized.substr(0, 20);
    }

    if (sanitized.empty()) {
        sanitized = "Player";
    }

    return sanitized;
}