#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "snake.h"
#include <string>

class Controller {
public:
  void HandleInput(bool &running, Snake &snake) const;
  void HandleTextInput(std::string& inputText, bool& inputComplete) const;
  void HandleTextInput(SDL_Event& event, std::string& inputText) const;
  bool ValidatePlayerName(const std::string& name) const;

private:
  void ChangeDirection(Snake &snake, Snake::Direction input,
                       Snake::Direction opposite) const;
  void ProcessTextEvent(const SDL_Event& event, std::string& inputText) const;

  static constexpr std::size_t kMaxNameLength = 20;
};

#endif