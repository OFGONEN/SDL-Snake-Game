#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "highscore_manager.h"
#include "threaded_obstacle_manager.h"
#include "async_obstacle_generator.h"
#include <random>
#include <string>
#include <memory>
#include <future>

enum class GameState {
  ENTER_NAME,
  PLAYING,
  GAME_OVER,
  SHOW_SCORES
};

class Game {
public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  GameState GetState() const;
  const std::string& GetPlayerName() const;

private:
  Snake snake;
  SDL_Point food;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};
  GameState currentState{GameState::ENTER_NAME};
  std::string playerName;
  std::unique_ptr<HighScoreManager> highScoreManager;

  // Add threaded obstacle management
  std::unique_ptr<ThreadedObstacleManager> obstacleManager;
  std::unique_ptr<AsyncObstacleGenerator> asyncGenerator;

  // Configuration
  static constexpr float kInitialSpawnRate = 0.3f; // obstacles per second
  static constexpr float kSpawnRateIncrease = 0.1f; // increase per difficulty level
  static constexpr int kDifficultyIncreaseInterval = 5; // Every 5 points

  void PlaceFood();
  void Update();
  void SaveCurrentScore();
  void UpdateEnterName(const Controller& controller, const SDL_Event& event);
  void UpdatePlaying(const Controller& controller, const SDL_Event& event);
  void UpdateGameOver(const Controller& controller, const SDL_Event& event);
  void UpdateShowScores(const Controller& controller, const SDL_Event& event);

  void TransitionToState(GameState newState);
  void ResetGame();

  // New obstacle-related methods
  void CheckObstacleCollisions();
  void UpdateDifficulty();
  void HandleObstacleSpawning(float delta_time);
  void HandleAsyncObstacleGeneration(); // Async obstacle generation
  void InitializeObstacleThreads(); // Start ThreadedObstacleManager threads
  void ShutdownObstacleThreads(); // Clean shutdown
  bool IsValidFoodPosition(int x, int y) const;

  // Async obstacle generation methods
  void StartAsyncObstacleGeneration(int fixed_count, int moving_count);
  void ProcessPendingAsyncObstacles();
  void LogPerformanceReport() const;

private:
  // Async generation state
  std::future<std::vector<std::unique_ptr<Obstacle>>> pending_obstacles_future;
  bool async_generation_pending{false};
  float async_generation_timer{0.0f};
  static constexpr float kAsyncGenerationInterval = 10.0f; // Every 10 seconds
};

#endif