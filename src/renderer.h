#ifndef RENDERER_H
#define RENDERER_H

#include "SDL.h"
#include "SDL_ttf.h"
#include "snake.h"
#include "score_entry.h"
#include "obstacle_manager.h"
#include "obstacle.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>

enum class GameState;

// Custom deleter for TTF_Font
struct TTFFontDeleter {
  void operator()(TTF_Font* font) const {
    if (font) {
      TTF_CloseFont(font);
    }
  }
};

// Smart pointer type alias for TTF fonts
using TTFFontPtr = std::unique_ptr<TTF_Font, TTFFontDeleter>;

class Renderer {
public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height,
           const std::size_t grid_width, const std::size_t grid_height);
  ~Renderer();

  void RenderPlaying(Snake const snake, SDL_Point const &food);
  void RenderPlayingWithObstacles(const Snake& snake, const SDL_Point& food,
                                 const ObstacleManager& obstacleManager);
  void UpdateWindowTitle(int score, int fps);

  // New obstacle rendering methods
  void RenderObstacles(const ObstacleManager& obstacleManager) const;
  void RenderObstacle(const Obstacle& obstacle) const;
  void RenderFixedObstacle(int x, int y) const;
  void RenderMovingObstacle(int x, int y, MovementPattern pattern) const;

  void RenderNameInput(const std::string& currentInput);
  void RenderNameInputWithValidation(const std::string& currentInput, const std::string& validationMessage);
  void RenderEnhancedHighScores(const std::vector<ScoreEntry>& scores,
                                std::function<std::string(const std::string&)> formatTimestamp);
  void RenderGameOverScreen(int score, bool isHighScore);

private:
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;
  TTFFontPtr font;
  TTFFontPtr large_font;

  const std::size_t screen_width;
  const std::size_t screen_height;
  const std::size_t grid_width;
  const std::size_t grid_height;

  static constexpr int kFontSize = 18;
  static constexpr int kLargeFontSize = 28;

  void RenderTextTTF(const std::string& text, int x, int y, SDL_Color color, bool large = false);
  void ClearScreen();
  void PresentScreen();
  SDL_Color GetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
  void LoadFonts();
  void CleanupFonts();

  // New rendering helper methods
  void DrawObstacleCell(int x, int y, const SDL_Color& color) const;
  void DrawMovingObstacleWithPattern(int x, int y, MovementPattern pattern) const;
  SDL_Color GetObstacleColor(ObstacleType type) const;

  // Const methods for color management
  static constexpr SDL_Color GetFixedObstacleColor() { return {128, 64, 0, 255}; }
  static constexpr SDL_Color GetMovingObstacleColor() { return {255, 165, 0, 255}; }
  static constexpr SDL_Color GetMovingObstacleAccentColor() { return {255, 200, 100, 255}; }
};

#endif