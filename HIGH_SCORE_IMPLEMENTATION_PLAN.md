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
- [ ] Player can enter name using keyboard before game starts
- [ ] Name input supports alphanumeric characters (max 20 chars)
- [ ] Input validates against empty names
- [ ] Player can use Backspace to edit and Enter to confirm

### User Story 2: High Score Persistence
**As a** player
**I want to** see my score saved when I achieve a high score
**So that** I can track my progress over time

**Acceptance Criteria:**
- [ ] Game saves score to `scores.txt` when game ends
- [ ] File stores player name, score, and timestamp
- [ ] Top 10 scores are maintained in sorted order
- [ ] File persists between game sessions

### User Story 3: High Score Display
**As a** player
**I want to** view the current high scores
**So that** I can see how I rank against previous games

**Acceptance Criteria:**
- [ ] High scores displayed after game over
- [ ] Shows rank, name, score, and date
- [ ] Highlights if current score made the top 10
- [ ] Option to view scores without playing

## Technical Implementation Plan

### Sprint 1: Foundation & File I/O (Session 1)

#### Task 1.1: Create HighScoreManager Class
**Rubric Criteria**: Object-Oriented Programming, Memory Management
**Files**: `src/highscore_manager.h`, `src/highscore_manager.cpp`

**Implementation Checklist:**
- [ ] Create class with proper access specifiers (private data, public interface)
- [ ] Use member initialization lists in constructor
- [ ] Implement RAII for file resource management
- [ ] Add const methods for data access
- [ ] Use `std::vector<ScoreEntry>` for score storage
- [ ] Implement smart pointers for any dynamic allocations

**ScoreEntry Structure:**
```cpp
struct ScoreEntry {
    std::string playerName;
    int score;
    std::string timestamp;

    // Constructor with member initialization list
    ScoreEntry(const std::string& name, int s, const std::string& time);
};
```

**Core Methods:**
- [ ] `LoadScores()` - Read from file with exception handling
- [ ] `SaveScore(const std::string& name, int score)` - Write to file
- [ ] `GetTopScores(size_t count = 10)` - Return sorted scores
- [ ] `IsHighScore(int score)` - Check if score qualifies

#### Task 1.2: File I/O Implementation
**Rubric Criteria**: Loops, Functions, and I/O
**File Format**: `scores.txt` (CSV or structured text)

**Implementation Checklist:**
- [ ] Use `std::ifstream`/`std::ofstream` for file operations
- [ ] Implement proper exception handling (`try-catch` blocks)
- [ ] Use various control structures (for loops, while loops, if/else)
- [ ] Add file existence checking and creation
- [ ] Implement data parsing and formatting functions

**File Format Example:**
```
PlayerName,Score,Timestamp
Alice,150,2024-09-27_14:30:15
Bob,120,2024-09-27_13:45:22
```

### Sprint 2: Input System Enhancement (Session 2)

#### Task 2.1: Extend Controller for Text Input
**Rubric Criteria**: Object-Oriented Programming, Loops, Functions, and I/O
**Files**: `src/controller.h`, `src/controller.cpp`

**Implementation Checklist:**
- [ ] Add text input handling methods with function overloading
- [ ] Implement input validation functions
- [ ] Use switch statements for key handling
- [ ] Add member variables with proper access specifiers
- [ ] Use const references for string parameters

**New Methods:**
- [ ] `HandleTextInput(std::string& inputText, bool& inputComplete)`
- [ ] `HandleTextInput(SDL_Event& event, std::string& inputText)` (overloaded)
- [ ] `ValidatePlayerName(const std::string& name)`
- [ ] `ProcessTextEvent(const SDL_Event& event)`

#### Task 2.2: Game State Management
**Rubric Criteria**: Object-Oriented Programming, Memory Management
**Files**: `src/game.h`, `src/game.cpp`

**Implementation Checklist:**
- [ ] Add game state enumeration with clear naming
- [ ] Use private game state member with public getter
- [ ] Implement state transition methods
- [ ] Add player name storage using `std::string`
- [ ] Use references to avoid unnecessary copying

**Game States:**
```cpp
enum class GameState {
    ENTER_NAME,
    PLAYING,
    GAME_OVER,
    SHOW_SCORES
};
```

### Sprint 3: UI Enhancement & Integration (Session 3)

#### Task 3.1: Text Rendering Capabilities
**Rubric Criteria**: Object-Oriented Programming, Memory Management
**Files**: `src/renderer.h`, `src/renderer.cpp`

**Implementation Checklist:**
- [ ] Extend Renderer class with text rendering methods
- [ ] Use const references for string parameters
- [ ] Implement proper resource management for text surfaces
- [ ] Add private helper methods for text operations
- [ ] Use RAII for SDL_Surface cleanup

**New Methods:**
- [ ] `RenderText(const std::string& text, int x, int y, SDL_Color color)`
- [ ] `RenderNameInput(const std::string& currentInput)`
- [ ] `RenderHighScores(const std::vector<ScoreEntry>& scores)`
- [ ] `RenderGameOverScreen(int score, bool isHighScore)`

#### Task 3.2: Game Loop State Integration
**Rubric Criteria**: Loops, Functions, and I/O, Object-Oriented Programming
**Files**: `src/game.cpp`, `src/main.cpp`

**Implementation Checklist:**
- [ ] Modify main game loop with state-specific logic
- [ ] Use various control structures (while, for, if/else, switch)
- [ ] Implement function decomposition for complex operations
- [ ] Add proper method overriding where appropriate
- [ ] Use member initialization lists for new data members

**State-Specific Updates:**
- [ ] `UpdateEnterName()` - Handle name input state
- [ ] `UpdatePlaying()` - Existing game logic
- [ ] `UpdateGameOver()` - High score processing
- [ ] `UpdateShowScores()` - Score display logic

### Sprint 4: Advanced Features & Polish (Session 4)

#### Task 4.1: Smart Pointer Integration
**Rubric Criteria**: Memory Management
**Files**: All relevant files

**Implementation Checklist:**
- [ ] Use `std::unique_ptr` for HighScoreManager ownership
- [ ] Implement `std::shared_ptr` for score data sharing
- [ ] Add move semantics where beneficial
- [ ] Implement Rule of Five where needed
- [ ] Use `std::weak_ptr` if circular dependencies exist

#### Task 4.2: Enhanced User Experience
**Rubric Criteria**: Loops, Functions, and I/O
**Files**: Various

**Implementation Checklist:**
- [ ] Add input validation with detailed error handling
- [ ] Implement score highlighting for new high scores
- [ ] Add timestamp formatting functions
- [ ] Create utility functions for string manipulation
- [ ] Implement configuration options via file I/O

## Rubric Criteria Mapping

### Loops, Functions, and I/O (Target: 2+ criteria)
- ✅ **File I/O**: High score persistence (`scores.txt`)
- ✅ **Control Structures**: Various loops and conditionals in input handling
- ✅ **Function Overloading**: Text input methods with different signatures
- ✅ **Enhanced User Input**: Name entry beyond basic movement

### Object-Oriented Programming (Target: 3+ criteria)
- ✅ **Access Specifiers**: All classes use explicit public/private/protected
- ✅ **Member Initialization Lists**: All constructors properly initialize members
- ✅ **Inheritance**: Potential Entity base class for polymorphism
- ✅ **Encapsulation**: Private data with public getter/setter methods
- ✅ **Templates**: Template functions for score operations if needed

### Memory Management (Target: 3+ criteria)
- ✅ **References**: Const references in function parameters
- ✅ **Smart Pointers**: unique_ptr, shared_ptr for resource management
- ✅ **RAII**: Automatic resource cleanup in destructors
- ✅ **Move Semantics**: Move constructors for performance-critical classes
- ✅ **Rule of Five**: Complete implementation for resource-managing classes

### Concurrency (Target: 2+ criteria)
- 🔄 **Async File I/O**: Use `std::async` for non-blocking file operations
- 🔄 **Thread Safety**: Mutex protection for score file access
- 🔄 **Promise/Future**: Communication between file I/O and game threads

## Definition of Done

### Code Quality Checklist
- [ ] All classes follow single responsibility principle
- [ ] Proper exception handling for all file operations
- [ ] Const correctness throughout implementation
- [ ] No memory leaks (verified with tools if available)
- [ ] Consistent naming conventions
- [ ] All public methods documented

### Testing Checklist
- [ ] File creation and reading works correctly
- [ ] Invalid input handling (empty names, file access errors)
- [ ] Score sorting and persistence verification
- [ ] Game state transitions work properly
- [ ] Memory management verified (no leaks)

### Integration Checklist
- [ ] Builds successfully with existing CMake configuration
- [ ] Maintains 60 FPS performance target
- [ ] No breaking changes to existing interfaces
- [ ] Cross-platform compatibility preserved

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
- [ ] All user stories completed with acceptance criteria met
- [ ] Minimum 5 rubric criteria addressed across categories
- [ ] No performance degradation from baseline
- [ ] Successful file persistence across game sessions
- [ ] Clean, maintainable code following established patterns

---

**Start Development**: Begin with Sprint 1, Task 1.1 (HighScoreManager class creation)
**Review Points**: After each sprint completion
**Final Review**: Comprehensive testing and rubric criteria validation