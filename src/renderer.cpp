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

void Renderer::RenderPlaying(Snake const snake, SDL_Point const &food) {
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


void Renderer::UpdateWindowTitle(int score, int fps) {
  std::string title{"Snake Score: " + std::to_string(score) +
                    " FPS: " + std::to_string(fps)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}

void Renderer::RenderText(const std::string& text, int x, int y, SDL_Color color) {
  RenderTextTTF(text, x, y, color, false);
}

void Renderer::RenderNameInput(const std::string& currentInput) {
  RenderNameInputWithValidation(currentInput, "");
}

void Renderer::RenderNameInputWithValidation(const std::string& currentInput, const std::string& validationMessage) {
  ClearScreen();

  SDL_Color white = GetColor(255, 255, 255);
  SDL_Color green = GetColor(0, 255, 0);
  SDL_Color red = GetColor(255, 0, 0);
  SDL_Color yellow = GetColor(255, 255, 0);
  SDL_Color gray = GetColor(128, 128, 128);

  int centerX = screen_width / 2;
  int centerY = screen_height / 2;

  RenderTextTTF("SNAKE GAME", centerX - 100, centerY - 150, green, true);
  RenderTextTTF("Enter your name:", centerX - 80, centerY - 50, white);

  std::string displayInput = currentInput.empty() ? "_" : currentInput + "_";
  SDL_Color inputColor = green;

  if (currentInput.length() > 20) {
    inputColor = red;
  } else if (currentInput.length() > 15) {
    inputColor = yellow;
  }

  RenderTextTTF(displayInput, centerX - 100, centerY, inputColor);

  if (!validationMessage.empty()) {
    RenderTextTTF(validationMessage, centerX - 120, centerY + 30, red);
  }

  std::string charCount = "(" + std::to_string(currentInput.length()) + "/20 characters)";
  RenderTextTTF(charCount, centerX - 80, centerY + 60, gray);

  RenderTextTTF("Press ENTER to start", centerX - 90, centerY + 90, gray);
  RenderTextTTF("Press ESC to cancel", centerX - 90, centerY + 120, gray);

  PresentScreen();
}

void Renderer::RenderHighScores(const std::vector<ScoreEntry>& scores) {
  ClearScreen();

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

  PresentScreen();
}

void Renderer::RenderEnhancedHighScores(const std::vector<ScoreEntry>& scores,
                                        std::function<std::string(const std::string&)> formatTimestamp) {
  ClearScreen();

  SDL_Color white = GetColor(255, 255, 255);
  SDL_Color gold = GetColor(255, 215, 0);
  SDL_Color silver = GetColor(192, 192, 192);
  SDL_Color bronze = GetColor(205, 127, 50);
  SDL_Color lightGray = GetColor(160, 160, 160);

  int centerX = screen_width / 2;
  int startY = 60;

  RenderTextTTF("HIGH SCORES", centerX - 80, 20, gold, true);

  if (scores.empty()) {
    RenderTextTTF("No scores yet!", centerX - 60, startY + 50, white);
  } else {
    RenderTextTTF("Rank  Player               Score    Date", centerX - 180, startY, lightGray);

    for (std::size_t i = 0; i < scores.size() && i < 10; ++i) {
      SDL_Color rankColor = white;
      if (i == 0) rankColor = gold;
      else if (i == 1) rankColor = silver;
      else if (i == 2) rankColor = bronze;

      int lineY = startY + 40 + i * 25;

      std::string rank = std::to_string(i + 1) + ".";
      std::string playerName = scores[i].playerName;
      std::string scoreStr = std::to_string(scores[i].score);
      std::string formattedTime = formatTimestamp(scores[i].timestamp);

      if (playerName.length() > 15) {
        playerName = playerName.substr(0, 12) + "...";
      }

      RenderTextTTF(rank, centerX - 180, lineY, rankColor);
      RenderTextTTF(playerName, centerX - 150, lineY, white);
      RenderTextTTF(scoreStr, centerX - 30, lineY, rankColor);
      RenderTextTTF(formattedTime, centerX + 20, lineY, lightGray);
    }
  }

  RenderTextTTF("Press R to restart", centerX - 80, startY + 320, GetColor(128, 128, 128));
  RenderTextTTF("Press ESC to go back", centerX - 90, startY + 350, GetColor(128, 128, 128));

  PresentScreen();
}

void Renderer::RenderGameOverScreen(int score, bool isHighScore) {
  ClearScreen();

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

  PresentScreen();
}

void Renderer::RenderTextTTF(const std::string& text, int x, int y, SDL_Color color, bool large) {
  if (text.empty()) return;

  TTF_Font* currentFont = large ? large_font.get() : font.get();
  if (currentFont == nullptr) {
    std::cerr << "Font not loaded!\n";
    return;
  }

  // Use smart pointers for automatic resource management
  std::unique_ptr<SDL_Surface, void(*)(SDL_Surface*)> textSurface(
    TTF_RenderUTF8_Solid(currentFont, text.c_str(), color), SDL_FreeSurface);

  if (!textSurface) {
    std::cerr << "Unable to render text surface! TTF_Error: " << TTF_GetError() << "\n";
    return;
  }

  std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> textTexture(
    SDL_CreateTextureFromSurface(sdl_renderer, textSurface.get()), SDL_DestroyTexture);

  if (!textTexture) {
    std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << "\n";
    return;
  }

  SDL_Rect renderQuad = {x, y, textSurface->w, textSurface->h};
  SDL_RenderCopy(sdl_renderer, textTexture.get(), nullptr, &renderQuad);

  // Resources automatically cleaned up by smart pointers
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
    font.reset(TTF_OpenFont(fontPaths[i], kFontSize));
    if (font) {
      large_font.reset(TTF_OpenFont(fontPaths[i], kLargeFontSize));
      if (large_font) {
        std::cout << "Loaded font: " << fontPaths[i] << "\n";
        return;
      } else {
        font.reset(); // Automatically calls TTF_CloseFont via custom deleter
      }
    }
  }

  std::cerr << "Warning: No system fonts found! Text rendering will not work properly.\n";
  std::cerr << "TTF_Error: " << TTF_GetError() << "\n";
}

void Renderer::CleanupFonts() {
  // Smart pointers automatically handle cleanup via custom deleter
  font.reset();
  large_font.reset();
}
