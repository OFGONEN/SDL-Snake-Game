#include "game.h"
#include "SDL.h"
#include <iostream>

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height), engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)),
      highScoreManager(std::make_unique<HighScoreManager>()),
      obstacleManager(std::make_unique<ThreadedObstacleManager>(grid_width, grid_height)) {
  PlaceFood();
  InitializeObstacleThreads();
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

    // Poll ALL events once per frame
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        // Save score if quitting during gameplay
        if (currentState == GameState::PLAYING) {
          SaveCurrentScore();
        }
        running = false;
        continue;
      }

      // Process event based on current state
      switch (currentState) {
      case GameState::ENTER_NAME:
        UpdateEnterName(controller, e);
        break;
      case GameState::PLAYING:
        UpdatePlaying(controller, e);
        break;
      case GameState::GAME_OVER:
        UpdateGameOver(controller, e);
        break;
      case GameState::SHOW_SCORES:
        UpdateShowScores(controller, e);
        break;
      }
    }

    // Update game logic (separate from event handling)
    if (currentState == GameState::PLAYING) {
      Update();
    }

    // State-specific rendering
    switch (currentState) {
    case GameState::ENTER_NAME:
      renderer.RenderNameInput(playerName);
      break;
    case GameState::PLAYING:
      renderer.RenderPlaying(snake, food);
      break;
    case GameState::GAME_OVER:
      renderer.RenderGameOverScreen(score, highScoreManager->IsNewHighestScore(score));
      break;
    case GameState::SHOW_SCORES:
      renderer.RenderEnhancedHighScores(highScoreManager->GetTopScores(10),
        [this](const std::string& timestamp) { return highScoreManager->FormatTimestamp(timestamp); });
      break;
    }

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
    // Check that the location is not occupied by a snake item or obstacle before placing food
    if (!snake.SnakeCell(x, y) && IsValidFoodPosition(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) {
    if (currentState == GameState::PLAYING) {
      SaveCurrentScore(); // Save score before transitioning
      TransitionToState(GameState::GAME_OVER);
    }
    return;
  }

  // Handle obstacle updates and spawning
  obstacleManager->UpdateObstacleMovement();

  // Calculate delta time for obstacle spawning (60 FPS = ~16.67ms per frame)
  static const float frame_delta_time = 1.0f / 60.0f;
  HandleObstacleSpawning(frame_delta_time);

  snake.Update();

  // Check obstacle collisions
  CheckObstacleCollisions();

  if (!snake.alive) {
    if (currentState == GameState::PLAYING) {
      SaveCurrentScore();
      TransitionToState(GameState::GAME_OVER);
    }
    return;
  }

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFood();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.02;

    // Update difficulty based on score
    UpdateDifficulty();
  }
}

void Game::SaveCurrentScore() {
  if (!playerName.empty() && score > 0) {
    highScoreManager->SaveScore(playerName, score);
  }
}

void Game::UpdateEnterName(const Controller& controller, const SDL_Event& event) {
  bool inputComplete = false;
  controller.HandleTextInput(event, playerName, inputComplete);

  if (inputComplete) {
    if (!playerName.empty()) {
      TransitionToState(GameState::PLAYING);
    } else {
      // User pressed ESC - could transition to quit or stay in name input
      playerName.clear();
    }
  }
}

void Game::UpdatePlaying(const Controller& controller, const SDL_Event& event) {
  controller.HandleInput(event, snake);
}

void Game::UpdateGameOver(const Controller& controller, const SDL_Event& event) {
  // Handle input for navigation
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
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

void Game::UpdateShowScores(const Controller& controller, const SDL_Event& event) {
  // Handle input for navigation
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
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

void Game::TransitionToState(GameState newState) {
  currentState = newState;
}

void Game::ResetGame() {
  score = 0;
  playerName.clear();
  snake = Snake(random_w.max() + 1, random_h.max() + 1);
  obstacleManager->ClearAllObstacles();
  PlaceFood();
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }
GameState Game::GetState() const { return currentState; }
const std::string& Game::GetPlayerName() const { return playerName; }

void Game::CheckObstacleCollisions() {
  if (obstacleManager->CheckCollisionWithSnake(snake)) {
    snake.alive = false;
  }
}

void Game::UpdateDifficulty() {
  int difficulty_level = score / kDifficultyIncreaseInterval + 1;
  obstacleManager->SetDifficultyLevel(difficulty_level);
}

void Game::HandleObstacleSpawning(float delta_time) {
  if (obstacleManager->ShouldSpawnObstacle(delta_time)) {
    obstacleManager->SpawnRandomObstacle();
  }
}

void Game::InitializeObstacleThreads() {
  obstacleManager->StartLifetimeThread();
}

void Game::ShutdownObstacleThreads() {
  obstacleManager->StopLifetimeThread();
}

bool Game::IsValidFoodPosition(int x, int y) const {
  return obstacleManager->IsValidFoodPosition(x, y);
}