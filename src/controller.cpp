#include "controller.h"
#include "SDL.h"
#include "snake.h"
#include <iostream>
#include <algorithm>
#include <cctype>

void Controller::ChangeDirection(Snake &snake, Snake::Direction input,
                                 Snake::Direction opposite) const {
  if (snake.direction != opposite || snake.size == 1)
    snake.direction = input;
  return;
}

void Controller::HandleInput(bool &running, Snake &snake) const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      running = false;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
      case SDLK_UP:
        ChangeDirection(snake, Snake::Direction::kUp, Snake::Direction::kDown);
        break;

      case SDLK_DOWN:
        ChangeDirection(snake, Snake::Direction::kDown, Snake::Direction::kUp);
        break;

      case SDLK_LEFT:
        ChangeDirection(snake, Snake::Direction::kLeft,
                        Snake::Direction::kRight);
        break;

      case SDLK_RIGHT:
        ChangeDirection(snake, Snake::Direction::kRight,
                        Snake::Direction::kLeft);
        break;
      }
    }
  }
}

bool Controller::HandleTextInput(std::string& inputText, bool& inputComplete) const {
  inputComplete = false;

  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      return false;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
      case SDLK_RETURN:
      case SDLK_KP_ENTER:
        if (ValidatePlayerName(inputText)) {
          inputComplete = true;
        }
        break;

      case SDLK_BACKSPACE:
        if (!inputText.empty()) {
          inputText.pop_back();
        }
        break;

      case SDLK_ESCAPE:
        inputText.clear();
        inputComplete = true;
        break;

      default:
        ProcessTextEvent(e, inputText);
        break;
      }
    } else if (e.type == SDL_TEXTINPUT) {
      ProcessTextEvent(e, inputText);
    }
  }
  return true;
}

void Controller::HandleTextInput(SDL_Event& event, std::string& inputText) const {
  if (event.type == SDL_TEXTINPUT) {
    ProcessTextEvent(event, inputText);
  } else if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
    case SDLK_BACKSPACE:
      if (!inputText.empty()) {
        inputText.pop_back();
      }
      break;
    }
  }
}

bool Controller::ValidatePlayerName(const std::string& name) const {
  if (name.empty()) {
    return false;
  }

  if (name.length() > kMaxNameLength) {
    return false;
  }

  return std::all_of(name.begin(), name.end(), [](char c) {
    return std::isalnum(c) || c == ' ' || c == '_' || c == '-';
  });
}

void Controller::ProcessTextEvent(const SDL_Event& event, std::string& inputText) const {
  if (event.type == SDL_TEXTINPUT) {
    const char* text = event.text.text;

    for (int i = 0; text[i] != '\0' && inputText.length() < kMaxNameLength; ++i) {
      char c = text[i];
      if (std::isalnum(c) || c == ' ' || c == '_' || c == '-') {
        inputText += c;
      }
    }
  }
}