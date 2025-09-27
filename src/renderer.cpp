#include "renderer.h"
#include "game.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
    : screen_width(screen_width), screen_height(screen_height),
      grid_width(grid_width), grid_height(grid_height), font(nullptr), large_font(nullptr) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Initialize TTF
  if (TTF_Init() == -1) {
    std::cerr << "TTF could not initialize.\n";
    std::cerr << "TTF_Error: " << TTF_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Load fonts
  LoadFonts();
}

Renderer::~Renderer() {
  CleanupFonts();
  SDL_DestroyRenderer(sdl_renderer);
  SDL_DestroyWindow(sdl_window);
  TTF_Quit();
  SDL_Quit();
}

void Renderer::Render(Snake const snake, SDL_Point const &food) {
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Render food
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF);
  block.x = food.x * block.w;
  block.y = food.y * block.h;
  SDL_RenderFillRect(sdl_renderer, &block);

  // Render snake's body
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render snake's head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  if (snake.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}

void Renderer::Render(Snake const snake, SDL_Point const &food, GameState state,
                      const std::string& playerName,
                      const std::vector<ScoreEntry>& highScores,
                      int currentScore) {
  ClearScreen();

  switch (state) {
  case GameState::ENTER_NAME:
    RenderNameInput(playerName);
    break;
  case GameState::PLAYING:
    Render(snake, food);
    return;
  case GameState::GAME_OVER:
    RenderGameOverScreen(currentScore, !highScores.empty());
    break;
  case GameState::SHOW_SCORES:
    RenderHighScores(highScores);
    break;
  }

  PresentScreen();
}

void Renderer::UpdateWindowTitle(int score, int fps) {
  std::string title{"Snake Score: " + std::to_string(score) +
                    " FPS: " + std::to_string(fps)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}

void Renderer::RenderText(const std::string& text, int x, int y, SDL_Color color) {
  RenderTextTTF(text, x, y, color, false);
}

void Renderer::RenderNameInput(const std::string& currentInput) {
  SDL_Color white = GetColor(255, 255, 255);
  SDL_Color green = GetColor(0, 255, 0);
  SDL_Color gray = GetColor(128, 128, 128);

  int centerX = screen_width / 2;
  int centerY = screen_height / 2;

  RenderTextTTF("SNAKE GAME", centerX - 100, centerY - 150, green, true);
  RenderTextTTF("Enter your name:", centerX - 80, centerY - 50, white);

  std::string displayInput = currentInput.empty() ? "_" : currentInput + "_";
  RenderTextTTF(displayInput, centerX - 100, centerY, green);

  RenderTextTTF("Press ENTER to start", centerX - 90, centerY + 50, gray);
  RenderTextTTF("Press ESC to cancel", centerX - 90, centerY + 80, gray);
}

void Renderer::RenderHighScores(const std::vector<ScoreEntry>& scores) {
  SDL_Color white = GetColor(255, 255, 255);
  SDL_Color gold = GetColor(255, 215, 0);
  SDL_Color silver = GetColor(192, 192, 192);
  SDL_Color bronze = GetColor(205, 127, 50);

  int centerX = screen_width / 2;
  int startY = 80;

  RenderTextTTF("HIGH SCORES", centerX - 80, 30, gold, true);

  if (scores.empty()) {
    RenderTextTTF("No scores yet!", centerX - 60, startY + 50, white);
  } else {
    for (std::size_t i = 0; i < scores.size() && i < 10; ++i) {
      SDL_Color rankColor = white;
      if (i == 0) rankColor = gold;
      else if (i == 1) rankColor = silver;
      else if (i == 2) rankColor = bronze;

      std::string rank = std::to_string(i + 1) + ".";
      std::string scoreText = scores[i].playerName + " - " + std::to_string(scores[i].score);

      RenderTextTTF(rank, centerX - 150, startY + i * 30, rankColor);
      RenderTextTTF(scoreText, centerX - 120, startY + i * 30, white);
    }
  }

  RenderTextTTF("Press R to restart", centerX - 80, startY + 320, GetColor(128, 128, 128));
  RenderTextTTF("Press ESC to go back", centerX - 90, startY + 350, GetColor(128, 128, 128));
}

void Renderer::RenderGameOverScreen(int score, bool isHighScore) {
  SDL_Color white = GetColor(255, 255, 255);
  SDL_Color red = GetColor(255, 0, 0);
  SDL_Color green = GetColor(0, 255, 0);
  SDL_Color gray = GetColor(128, 128, 128);

  int centerX = screen_width / 2;
  int centerY = screen_height / 2;

  RenderTextTTF("GAME OVER", centerX - 80, centerY - 100, red, true);

  std::string scoreText = "Final Score: " + std::to_string(score);
  RenderTextTTF(scoreText, centerX - 70, centerY - 50, white);

  if (isHighScore) {
    RenderTextTTF("NEW HIGH SCORE!", centerX - 90, centerY - 20, green);
  }

  RenderTextTTF("Press SPACE for high scores", centerX - 120, centerY + 30, gray);
  RenderTextTTF("Press R to restart", centerX - 80, centerY + 60, gray);
}

void Renderer::RenderTextTTF(const std::string& text, int x, int y, SDL_Color color, bool large) {
  if (text.empty()) return;

  TTF_Font* currentFont = large ? large_font : font;
  if (currentFont == nullptr) {
    std::cerr << "Font not loaded!\n";
    return;
  }

  SDL_Surface* textSurface = TTF_RenderUTF8_Solid(currentFont, text.c_str(), color);
  if (textSurface == nullptr) {
    std::cerr << "Unable to render text surface! TTF_Error: " << TTF_GetError() << "\n";
    return;
  }

  SDL_Texture* textTexture = SDL_CreateTextureFromSurface(sdl_renderer, textSurface);
  if (textTexture == nullptr) {
    std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << "\n";
    SDL_FreeSurface(textSurface);
    return;
  }

  SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
  SDL_RenderCopy(sdl_renderer, textTexture, nullptr, &renderQuad);

  SDL_DestroyTexture(textTexture);
  SDL_FreeSurface(textSurface);
}

void Renderer::ClearScreen() {
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);
}

void Renderer::PresentScreen() {
  SDL_RenderPresent(sdl_renderer);
}

SDL_Color Renderer::GetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  SDL_Color color;
  color.r = r;
  color.g = g;
  color.b = b;
  color.a = a;
  return color;
}

void Renderer::LoadFonts() {
  // Try to load a system font - this will need to be adjusted based on the system
  const char* fontPaths[] = {
    "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    "/usr/share/fonts/TTF/DejaVuSans.ttf",
    "/System/Library/Fonts/Arial.ttf",
    "/Windows/Fonts/arial.ttf",
    nullptr
  };

  for (int i = 0; fontPaths[i] != nullptr; ++i) {
    font = TTF_OpenFont(fontPaths[i], kFontSize);
    if (font != nullptr) {
      large_font = TTF_OpenFont(fontPaths[i], kLargeFontSize);
      if (large_font != nullptr) {
        std::cout << "Loaded font: " << fontPaths[i] << "\n";
        return;
      } else {
        TTF_CloseFont(font);
        font = nullptr;
      }
    }
  }

  std::cerr << "Warning: No system fonts found! Text rendering will not work properly.\n";
  std::cerr << "TTF_Error: " << TTF_GetError() << "\n";
}

void Renderer::CleanupFonts() {
  if (font != nullptr) {
    TTF_CloseFont(font);
    font = nullptr;
  }
  if (large_font != nullptr) {
    TTF_CloseFont(large_font);
    large_font = nullptr;
  }
}
