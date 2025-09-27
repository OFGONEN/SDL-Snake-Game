#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include "highscore_manager.h"
#include <random>
#include <string>
#include <memory>

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

  void PlaceFood();
  void Update();
  void UpdateEnterName(const Controller& controller, const SDL_Event& event);
  void UpdatePlaying(const Controller& controller, const SDL_Event& event);
  void UpdateGameOver(const Controller& controller, const SDL_Event& event);
  void UpdateShowScores(const Controller& controller, const SDL_Event& event);

  void TransitionToState(GameState newState);
  void ResetGame();
};

#endif