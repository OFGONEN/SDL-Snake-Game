#ifndef RENDERER_H
#define RENDERER_H

#include "SDL.h"
#include "SDL_ttf.h"
#include "snake.h"
#include "score_entry.h"
#include <vector>
#include <string>
#include <memory>

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
  void UpdateWindowTitle(int score, int fps);

  void RenderText(const std::string& text, int x, int y, SDL_Color color);
  void RenderNameInput(const std::string& currentInput);
  void RenderHighScores(const std::vector<ScoreEntry>& scores);
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
};

#endif