#include "game.h"
#include "SDL.h"
#include <iostream>

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height), engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)),
      highScoreManager(std::make_unique<HighScoreManager>()) {
  PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // State-based input and update handling
    switch (currentState) {
    case GameState::ENTER_NAME:
      if (!UpdateEnterName(controller)) running = false;
      break;
    case GameState::PLAYING:
      if (!UpdatePlaying(controller)) running = false;
      break;
    case GameState::GAME_OVER:
      if (!UpdateGameOver(controller)) running = false;
      break;
    case GameState::SHOW_SCORES:
      if (!UpdateShowScores(controller)) running = false;
      break;
    }

    // State-aware rendering
    std::vector<ScoreEntry> scores;
    if (currentState == GameState::SHOW_SCORES) {
      scores = highScoreManager->GetTopScores(10);
    }

    renderer.Render(snake, food, currentState, playerName, scores, score);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) {
    if (currentState == GameState::PLAYING) {
      TransitionToState(GameState::GAME_OVER);
    }
    return;
  }

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;
  }
}

bool Game::UpdateEnterName(const Controller& controller) {
  bool inputComplete = false;
  if (!controller.HandleTextInput(playerName, inputComplete)) {
    return false; // User clicked X to quit
  }

  if (inputComplete) {
    if (!playerName.empty()) {
      TransitionToState(GameState::PLAYING);
    } else {
      return false; // User pressed ESC or quit without entering name
    }
  }
  return true;
}

bool Game::UpdatePlaying(const Controller& controller) {
  bool running = true;
  controller.HandleInput(running, snake);
  if (!running) {
    return false; // User quit during gameplay
  }
  Update();
  return true;
}

bool Game::UpdateGameOver(const Controller& controller) {
  // Save high score if qualified
  if (!playerName.empty() && highScoreManager->IsHighScore(score)) {
    highScoreManager->SaveScore(playerName, score);
  }

  // Wait for input to show scores or restart
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      return false;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
      case SDLK_SPACE:
        TransitionToState(GameState::SHOW_SCORES);
        break;
      case SDLK_r:
        ResetGame();
        TransitionToState(GameState::ENTER_NAME);
        break;
      }
    }
  }
  return true;
}

bool Game::UpdateShowScores(const Controller& controller) {
  // Wait for input to restart
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      return false;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
      case SDLK_r:
        ResetGame();
        TransitionToState(GameState::ENTER_NAME);
        break;
      case SDLK_ESCAPE:
        TransitionToState(GameState::GAME_OVER);
        break;
      }
    }
  }
  return true;
}

void Game::TransitionToState(GameState newState) {
  currentState = newState;
}

void Game::ResetGame() {
  score = 0;
  playerName.clear();
  snake = Snake(random_w.max() + 1, random_h.max() + 1);
  PlaceFood();
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }
GameState Game::GetState() const { return currentState; }
const std::string& Game::GetPlayerName() const { return playerName; }