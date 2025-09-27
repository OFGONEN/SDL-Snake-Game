# High Score Feature Implementation Plan

## Epic Overview
**Feature**: Player Name Input and High Score Persistence
**Goal**: Allow players to enter their names and save high scores to a persistent text file
**Estimated Effort**: 3-4 development sessions
**Priority**: High (addresses multiple rubric criteria)

## User Stories

### User Story 1: Player Name Input
**As a** player
**I want to** enter my name when the game starts
**So that** my high scores are associated with my identity

**Acceptance Criteria:**
- [x] Player can enter name using keyboard before game starts
- [x] Name input supports alphanumeric characters (max 20 chars)
- [x] Input validates against empty names with real-time feedback
- [x] Player can use Backspace to edit and Enter to confirm
- [x] Visual feedback with color-coded input and character counter
- [x] Automatic name sanitization and validation

### User Story 2: High Score Persistence
**As a** player
**I want to** see my score saved when I achieve a high score
**So that** I can track my progress over time

**Acceptance Criteria:**
- [x] Game saves score to `scores.txt` when game ends
- [x] File stores player name, score, and timestamp
- [x] Top 10 scores are maintained in sorted order
- [x] File persists between game sessions
- [x] Automatic score saving on both death and quit scenarios
- [x] Proper high score detection (only highest score shows "NEW HIGH SCORE!")

### User Story 3: High Score Display
**As a** player
**I want to** view the current high scores
**So that** I can see how I rank against previous games

**Acceptance Criteria:**
- [x] High scores displayed after game over
- [x] Shows rank, name, score, and date with professional formatting
- [x] Highlights if current score made the top 10
- [x] Option to view scores without playing
- [x] Enhanced tabular layout with gold/silver/bronze ranking colors
- [x] Human-readable timestamp formatting (Sep 28, 2025 02:12)
- [x] Player name truncation for long names

## Technical Implementation Plan

### Sprint 1: Foundation & File I/O (Session 1) ✅ **COMPLETED**

#### Task 1.1: Create HighScoreManager Class ✅ **COMPLETED**
**Rubric Criteria**: Object-Oriented Programming, Memory Management
**Files**: `src/highscore_manager.h`, `src/highscore_manager.cpp`

**Implementation Checklist:**
- [x] Create class with proper access specifiers (private data, public interface)
- [x] Use member initialization lists in constructor
- [x] Implement RAII for file resource management
- [x] Add const methods for data access
- [x] Use `std::vector<ScoreEntry>` for score storage
- [x] Implement move semantics and Rule of Five

#### Task 1.2: Create ScoreEntry Structure ✅ **COMPLETED**
**Rubric Criteria**: Object-Oriented Programming, Function Overloading
**Files**: `src/score_entry.h`, `src/score_entry.cpp`

**ScoreEntry Structure:**
```cpp
struct ScoreEntry {
public:
    std::string playerName;
    int score;
    std::string timestamp;

    ScoreEntry() = default;
    ScoreEntry(const std::string& name, int s, const std::string& time);

    // Rule of Five implementation
    ScoreEntry(const ScoreEntry& other) = default;
    ScoreEntry& operator=(const ScoreEntry& other) = default;
    ScoreEntry(ScoreEntry&& other) noexcept = default;
    ScoreEntry& operator=(ScoreEntry&& other) noexcept = default;
    ~ScoreEntry() = default;

    // Comparison operators for sorting
    bool operator<(const ScoreEntry& other) const;
    bool operator>(const ScoreEntry& other) const;
    bool operator==(const ScoreEntry& other) const;
};
```

**Core Methods Implemented:**
- [x] `LoadScores()` - Read from file with exception handling
- [x] `SaveScore(const std::string& name, int score)` - Write to file
- [x] `GetTopScores(size_t count = 10)` - Return sorted scores
- [x] `IsHighScore(int score)` - Check if score qualifies
- [x] `GetScoreCount()` - Return number of scores
- [x] `ClearScores()` - Clear all scores and reset file

#### Task 1.3: File I/O Implementation ✅ **COMPLETED**
**Rubric Criteria**: Loops, Functions, and I/O
**File Format**: `scores.txt` (CSV format)

**Implementation Checklist:**
- [x] Use `std::ifstream`/`std::ofstream` for file operations
- [x] Implement proper exception handling (`try-catch` blocks)
- [x] Use various control structures (for loops, while loops, if/else)
- [x] Add file existence checking and creation
- [x] Implement data parsing and formatting functions
- [x] Automatic timestamp generation with `std::chrono`
- [x] Score sorting and top-10 maintenance

**File Format Example:**
```
PlayerName,Score,Timestamp
Alice,150,2024-09-27_14:30:15
Bob,120,2024-09-27_13:45:22
```

### Sprint 2: Input System Enhancement (Session 2) ✅ **COMPLETED**

#### Task 2.1: Extend Controller for Text Input ✅ **COMPLETED**
**Rubric Criteria**: Object-Oriented Programming, Loops, Functions, and I/O
**Files**: `src/controller.h`, `src/controller.cpp`

**Implementation Checklist:**
- [x] Add text input handling methods with function overloading
- [x] Implement input validation functions
- [x] Use switch statements for key handling
- [x] Add member variables with proper access specifiers
- [x] Use const references for string parameters

**New Methods Implemented:**
- [x] `HandleTextInput(std::string& inputText, bool& inputComplete)` - Main text input handler
- [x] `HandleTextInput(SDL_Event& event, std::string& inputText)` - Event-specific overload
- [x] `ValidatePlayerName(const std::string& name)` - Name validation with length/character checks
- [x] `ProcessTextEvent(const SDL_Event& event, std::string& inputText)` - Text processing logic

**Key Features Added:**
- Input validation: Alphanumeric + space/underscore/dash characters, max 20 chars
- Key handling: Enter (confirm), Backspace (edit), Escape (cancel)
- SDL text input events support with proper filtering
- Const correctness throughout all methods

#### Task 2.2: Game State Management ✅ **COMPLETED**
**Rubric Criteria**: Object-Oriented Programming, Memory Management
**Files**: `src/game.h`, `src/game.cpp`

**Implementation Checklist:**
- [x] Add game state enumeration with clear naming
- [x] Use private game state member with public getter
- [x] Implement state transition methods
- [x] Add player name storage using `std::string`
- [x] Use smart pointers for HighScoreManager ownership

**Game States Implemented:**
```cpp
enum class GameState {
    ENTER_NAME,    // Player enters name before game starts
    PLAYING,       // Main gameplay state
    GAME_OVER,     // Game ended, save high score
    SHOW_SCORES    // Display high score table
};
```

**State Management Features:**
- [x] `UpdateEnterName()` - Handle name input with validation
- [x] `UpdatePlaying()` - Existing game logic with state awareness
- [x] `UpdateGameOver()` - High score saving and transition logic
- [x] `UpdateShowScores()` - Score display and navigation
- [x] `TransitionToState()` - Clean state transitions
- [x] `ResetGame()` - Game reset functionality
- [x] `std::unique_ptr<HighScoreManager>` - Smart pointer integration

### Sprint 3: UI Enhancement & Integration (Session 3)

#### Task 3.1: Enable SDL_ttf Text Rendering ✅ **COMPLETED**
**Rubric Criteria**: Build System Integration, Memory Management
**Files**: `CMakeLists.txt`, `src/renderer.h`, `src/renderer.cpp`

**Implementation Checklist:**
- [x] Add SDL_ttf dependency to CMakeLists.txt
- [x] Update build system to link SDL_ttf library
- [x] Initialize TTF subsystem in Renderer constructor
- [x] Load default font for text rendering
- [x] Implement proper TTF resource cleanup (RAII)
- [x] Test SDL_ttf integration with simple text rendering

**SDL_ttf Key Functions Implemented:**
- [x] `TTF_Init()` - Initialize TTF library
- [x] `TTF_OpenFont()` - Load font file
- [x] `TTF_RenderUTF8_Solid()` - Render text to SDL_Surface
- [x] `SDL_CreateTextureFromSurface()` - Convert surface to texture
- [x] `TTF_CloseFont()` - Clean up font resources
- [x] `TTF_Quit()` - Shutdown TTF library

#### Task 3.2: Text Rendering Capabilities ✅ **COMPLETED**
**Rubric Criteria**: Object-Oriented Programming, Memory Management
**Files**: `src/renderer.h`, `src/renderer.cpp`

**Implementation Checklist:**
- [x] Extend Renderer class with SDL_ttf-based text rendering methods
- [x] Use const references for string parameters
- [x] Implement proper resource management for TTF_Font and SDL_Surface
- [x] Add private helper methods for text operations
- [x] Use RAII for SDL_Surface and SDL_Texture cleanup

**New Methods Implemented:**
- [x] `RenderText(const std::string& text, int x, int y, SDL_Color color)`
- [x] `RenderNameInput(const std::string& currentInput)`
- [x] `RenderHighScores(const std::vector<ScoreEntry>& scores)`
- [x] `RenderGameOverScreen(int score, bool isHighScore)`

#### Task 3.3: Game Loop State Integration ✅ **COMPLETED**
**Rubric Criteria**: Loops, Functions, and I/O, Object-Oriented Programming
**Files**: `src/game.cpp`, `src/main.cpp`

**Implementation Checklist:**
- [x] Modify main game loop with state-specific logic
- [x] Use various control structures (while, for, if/else, switch)
- [x] Implement function decomposition for complex operations
- [x] Add proper method overriding where appropriate
- [x] Use member initialization lists for new data members

**State-Specific Updates:**
- [x] `UpdateEnterName()` - Handle name input state
- [x] `UpdatePlaying()` - Existing game logic
- [x] `UpdateGameOver()` - High score processing
- [x] `UpdateShowScores()` - Score display logic

**Additional Bug Fixes:**
- [x] Fixed window close (X button) event handling across all game states
- [x] Improved Controller interface to return quit status from text input
- [x] Streamlined event polling to prevent conflicts between states
- [x] Refactored to top-down event handling architecture with single-pass polling

#### Task 3.4: Refactor Rendering to State-Specific Methods ✅ **COMPLETED**
**Rubric Criteria**: Object-Oriented Programming, Separation of Concerns
**Files**: `src/game.cpp`, `src/renderer.h`, `src/renderer.cpp`

**Problem Identified:**
Current monolithic rendering approach violates separation of concerns:
- Game class fetches UI data (high scores) for renderer
- Single render method handles all states with complex parameter passing
- Inconsistent with state-specific Update method pattern
- Unnecessary data fetching and parameter passing for unused states

**Implementation Checklist:**
- [x] Remove monolithic `Render` method with complex parameters
- [x] Implement state-specific render methods in Game class
- [x] Move data fetching responsibility to appropriate render methods
- [x] Create focused Renderer methods for each game state
- [x] Update main game loop to use state-specific rendering approach
- [x] Ensure proper separation between game logic and rendering logic

**New Rendering Architecture:**
```cpp
// State-specific rendering in Game::Run()
switch (currentState) {
case GameState::ENTER_NAME:
  renderer.RenderNameInput(playerName);
  break;
case GameState::PLAYING:
  renderer.RenderGame(snake, food);
  break;
case GameState::GAME_OVER:
  renderer.RenderGameOver(score, highScoreManager->IsHighScore(score));
  break;
case GameState::SHOW_SCORES:
  renderer.RenderHighScores(highScoreManager->GetTopScores(10));
  break;
}
```

**Benefits:**
- ✅ Clean separation of concerns (data fetching where needed)
- ✅ Consistent pattern with Update methods
- ✅ Improved maintainability and testability
- ✅ Reduced unnecessary parameter passing
- ✅ More focused and readable code

### Sprint 4: Advanced Features & Polish (Session 4)

#### Task 4.1: Smart Pointer Integration ✅ **COMPLETED**
**Rubric Criteria**: Memory Management
**Files**: `src/renderer.h`, `src/renderer.cpp`, `src/highscore_manager.h`, `src/highscore_manager.cpp`, `src/score_entry.h`

**Implementation Checklist:**
- [x] Use `std::unique_ptr` for HighScoreManager ownership
- [x] Implement `std::shared_ptr` for score data sharing
- [x] Add move semantics where beneficial
- [x] Implement Rule of Five where needed
- [x] Enhanced TTF font management with custom deleters
- [x] Smart SDL resource management for surfaces and textures
- [x] Perfect forwarding constructors for optimal performance

**Advanced Features Implemented:**
- **TTF Font Smart Pointers**: Custom `TTFFontDeleter` with `std::unique_ptr<TTF_Font, TTFFontDeleter>`
- **SDL Resource Management**: Smart pointers for `SDL_Surface` and `SDL_Texture` with automatic cleanup
- **Shared Score Collections**: `GetSharedTopScores()` returning `std::shared_ptr<const std::vector<ScoreEntry>>`
- **Perfect Forwarding**: Template constructor with `std::forward` for efficient ScoreEntry construction
- **Memory Safety**: Eliminated all manual resource cleanup, exception-safe resource management

#### Task 4.2: Enhanced User Experience ✅ **COMPLETED**
**Rubric Criteria**: Loops, Functions, and I/O
**Files**: `src/highscore_manager.h`, `src/highscore_manager.cpp`, `src/renderer.h`, `src/renderer.cpp`, `src/game.cpp`

**Implementation Checklist:**
- [x] Add input validation with detailed error handling
- [x] Implement score highlighting for new high scores
- [x] Add timestamp formatting functions
- [x] Create utility functions for string manipulation
- [x] Enhanced name input screen with visual feedback

**Features Implemented:**
- **Input Validation**: `IsValidPlayerName()` and `SanitizePlayerName()` static methods
- **Enhanced High Score Display**: `RenderEnhancedHighScores()` with formatted timestamps and tabular layout
- **Timestamp Formatting**: `FormatTimestamp()` converts "2025-09-28_02:12:53" to "Sep 28, 2025 02:12"
- **Real-time Input Feedback**: Character count, color-coded input (green→yellow→red), validation messages
- **Player Name Sanitization**: Automatic cleanup of invalid characters, length limiting, whitespace trimming

## Rubric Criteria Mapping

### Loops, Functions, and I/O (Target: 2+ criteria) - **4/4 COMPLETE** ✅
- ✅ **File I/O**: High score persistence (`scores.txt`) - *Implemented in HighScoreManager*
- ✅ **Control Structures**: Various loops and conditionals - *Switch statements, for/while loops in parsing and state management*
- ✅ **Function Overloading**: Multiple method signatures - *Controller text input methods, ScoreEntry operators*
- ✅ **Enhanced User Input**: Name entry beyond basic movement - *Text input system with validation*

### Object-Oriented Programming (Target: 3+ criteria) - **5/5 COMPLETE** ✅
- ✅ **Access Specifiers**: All classes use explicit public/private/protected - *All classes properly structured*
- ✅ **Member Initialization Lists**: All constructors properly initialize members - *All classes*
- ✅ **Encapsulation**: Private data with public getter/setter methods - *Game state management, data hiding*
- ✅ **Rule of Five**: Complete special member function implementation - *ScoreEntry and HighScoreManager*
- ✅ **Operator Overloading**: Comparison operators for sorting - *ScoreEntry operators*

### Memory Management (Target: 3+ criteria) - **5/5 COMPLETE** ✅
- ✅ **References**: Const references in function parameters - *Throughout Controller and HighScoreManager*
- ✅ **RAII**: Automatic resource cleanup in destructors - *File handle and smart pointer management*
- ✅ **Move Semantics**: Move constructors for performance - *HighScoreManager move operations*
- ✅ **Rule of Five**: Complete implementation for resource-managing classes - *Both major classes*
- ✅ **Smart Pointers**: unique_ptr for resource management - *Game class HighScoreManager ownership*

### Concurrency (Target: 2+ criteria)
- 🔄 **Async File I/O**: Use `std::async` for non-blocking file operations
- 🔄 **Thread Safety**: Mutex protection for score file access
- 🔄 **Promise/Future**: Communication between file I/O and game threads

## Definition of Done

### Code Quality Checklist
- [x] All classes follow single responsibility principle
- [x] Proper exception handling for all file operations
- [x] Const correctness throughout implementation
- [x] No memory leaks (RAII and smart pointers eliminate manual cleanup)
- [x] Consistent naming conventions
- [x] All public methods clearly documented with proper interfaces

### Testing Checklist
- [x] File creation and reading works correctly
- [x] Invalid input handling (empty names, invalid characters, file access errors)
- [x] Score sorting and persistence verification
- [x] Game state transitions work properly
- [x] Memory management verified (smart pointers and RAII patterns)

### Integration Checklist
- [x] Builds successfully with existing CMake configuration
- [x] Maintains 60 FPS performance target
- [x] No breaking changes to existing interfaces
- [x] Cross-platform compatibility preserved (font loading supports multiple paths)

## Risk Mitigation

### Technical Risks
- **File I/O Failures**: Implement comprehensive exception handling
- **Performance Impact**: Use efficient data structures and minimal file operations
- **State Management Complexity**: Clear state machine with documented transitions

### Development Risks
- **Scope Creep**: Focus on core functionality first, enhancements later
- **Integration Issues**: Test each component independently before integration
- **Rubric Compliance**: Regular verification against rubric criteria

## Success Metrics
- [x] All user stories completed with acceptance criteria met
- [x] **15+ rubric criteria addressed across all categories** (exceeded minimum)
- [x] No performance degradation from baseline
- [x] Successful file persistence across game sessions
- [x] Clean, maintainable code following established patterns
- [x] **Professional-grade user experience with enhanced UI/UX**
- [x] **Advanced memory management with smart pointers and RAII**

## Progress Summary

### ✅ **Sprint 1 Complete**: Foundation & File I/O
**Files Created:**
- `src/score_entry.h` - ScoreEntry struct with comparison operators
- `src/score_entry.cpp` - ScoreEntry implementation
- `src/highscore_manager.h` - HighScoreManager class definition
- `src/highscore_manager.cpp` - File I/O operations and score management
- Updated `CMakeLists.txt` - Build system integration
- Created `Makefile` - Development convenience commands

### ✅ **Sprint 2 Complete**: Input System Enhancement
**Files Modified:**
- `src/controller.h` - Extended with text input methods
- `src/controller.cpp` - Text input handling and validation
- `src/game.h` - Game state management and smart pointer integration
- `src/game.cpp` - State machine implementation and transitions

**Rubric Criteria Now Complete:**
- **Loops/Functions/I/O**: 4/4 criteria ✅ **COMPLETE**
- **Object-Oriented Programming**: 5/5 criteria ✅ **COMPLETE**
- **Memory Management**: 5/5 criteria ✅ **COMPLETE**
- **Build System**: Successfully compiles and links ✅

**Key Features Working:**
- Text input system with validation (alphanumeric + special chars)
- Game state management (ENTER_NAME → PLAYING → GAME_OVER → SHOW_SCORES)
- Smart pointer integration for resource management
- Function overloading for different input scenarios
- High score persistence integrated with game flow
- Proper const correctness and reference usage

### ✅ **Sprint 3 Complete**: UI Enhancement & Integration
**Files Modified:**
- `CMakeLists.txt` - Added SDL_ttf dependency and linking
- `src/renderer.h` - Extended with TTF text rendering capabilities
- `src/renderer.cpp` - Implemented proper TTF font loading and text rendering
- `src/game.cpp` - Enhanced state machine with better event handling
- `src/controller.h` - Updated HandleTextInput to return quit status
- `src/controller.cpp` - Fixed quit event handling in text input

**Text Rendering Features Added:**
- Multi-font support (regular and large fonts) with cross-platform font loading
- State-aware rendering with color-coded text (gold, silver, bronze rankings)
- Comprehensive UI screens: name input, game over, high scores display
- Proper RAII resource management for TTF fonts and SDL surfaces
- Event handling improvements for smooth user experience

### ✅ **Sprint 4 Complete**: Advanced Features & Polish
**Files Modified:**
- `src/renderer.h` & `src/renderer.cpp` - Smart pointer TTF font management with custom deleters, enhanced UX
- `src/highscore_manager.h` & `src/highscore_manager.cpp` - Shared score collections, input validation, timestamp formatting
- `src/score_entry.h` - Perfect forwarding constructors and enhanced move semantics
- `src/game.cpp` - Enhanced high score display integration

**Advanced Features Added:**
- Exception-safe resource management with RAII smart pointers
- Custom deleters for SDL/TTF resources (TTFFontDeleter)
- Shared immutable score collections for safe data sharing
- Perfect forwarding template constructors for optimal performance
- Eliminated all manual resource cleanup and potential memory leaks
- Real-time input validation with visual feedback
- Enhanced high score display with formatted timestamps
- Professional tabular score layout with ranking colors

---

**Current Status**: ✅ **ALL SPRINTS COMPLETE** - Sprint 1, 2, 3, 4 Complete
**Next Phase**: Optional Concurrency Implementation (Sprint 5)
**Achievement**: **ALL REQUIREMENTS + ADVANCED FEATURES COMPLETE** 🎯
**Features**: Input validation, timestamp formatting, enhanced UX, smart pointers, professional UI
**Rubric Status**: **15+ CRITERIA FULFILLED ACROSS ALL CATEGORIES** ✅
**Final Review**: Ready for submission - comprehensive feature set implemented

---

## 🎯 **FINAL IMPLEMENTATION SUMMARY**

### ✅ **CORE FEATURES DELIVERED**
1. **Player Name Input System**
   - Real-time input validation with visual feedback
   - Character counter and color-coded warnings
   - Automatic name sanitization (removes invalid chars, limits length)
   - Support for alphanumeric characters plus '_', '-', ' '

2. **High Score Persistence**
   - CSV file format with player name, score, timestamp
   - Automatic saving on game over and window close
   - Top 10 score maintenance with proper sorting
   - Robust file I/O with exception handling

3. **Enhanced High Score Display**
   - Professional tabular layout with headers
   - Gold/silver/bronze ranking colors for top 3
   - Human-readable timestamp formatting (Sep 28, 2025 02:12)
   - Player name truncation for optimal display
   - "NEW HIGH SCORE!" detection (only for actual highest scores)

4. **Advanced Memory Management**
   - Smart pointers with custom deleters (TTF fonts, SDL resources)
   - RAII pattern throughout for exception safety
   - Shared immutable score collections
   - Perfect forwarding constructors for optimal performance

5. **Professional Game State Management**
   - Clean state machine: ENTER_NAME → PLAYING → GAME_OVER → SHOW_SCORES
   - Top-down event handling architecture
   - Proper separation of concerns between game logic and rendering

### 🏆 **RUBRIC CRITERIA ACHIEVED (15+ TOTAL)**

**Loops, Functions, and I/O (4/4)** ✅
- File I/O operations, control structures, function overloading, enhanced user input

**Object-Oriented Programming (5/5)** ✅
- Access specifiers, member initialization, encapsulation, Rule of Five, operator overloading

**Memory Management (5/5)** ✅
- Const references, RAII, move semantics, smart pointers, Rule of Five

**Build System** ✅
- CMake integration, library linking (SDL_ttf), cross-platform compatibility

### 🚀 **READY FOR UDACITY SUBMISSION**
**Project Status**: **COMPLETE** - Exceeds all requirements with professional-grade implementation