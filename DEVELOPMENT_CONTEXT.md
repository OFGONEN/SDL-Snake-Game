# Development Context for AI-Assisted Implementation

## Overview
This document provides essential context for AI assistants working on the Snake Game C++ project. Read this file before implementing any features to ensure consistency with project goals and patterns.

## Required Reading
**ALWAYS read `@PROJECT_DOCUMENTATION.md` before implementing any feature.** This file contains:
- Complete project architecture
- Rubric-driven design goals
- Implementation patterns that MUST be followed

## Core Development Principles

### 1. **Mandatory Pattern Adherence**
Follow ALL patterns defined in the "Implementation Patterns" section of PROJECT_DOCUMENTATION.md:
- **Separation of Concerns**: Each class has single responsibility
- **Resource Management**: Use RAII, stack allocation, STL containers
- **Performance**: Maintain 60 FPS, efficient collision detection
- **Extensibility**: Modular architecture with clear interfaces
- **Cross-Platform**: SDL2 abstraction, CMake, standard C++17

### 2. **Rubric Compliance Requirements**
Every feature implementation MUST contribute to meeting Udacity C++ Nanodegree rubric criteria:

#### **Loops, Functions, and I/O** (Need 2+ criteria)
- Use variety of control structures (for, while, if/else, switch)
- Implement function overloading where appropriate
- Add file I/O operations (high scores, configuration, save/load)
- Support enhanced user input beyond basic movement

#### **Object-Oriented Programming** (Need 3+ criteria)
- Mark ALL data members as public/private/protected explicitly
- Use member initialization lists in ALL constructors
- Implement inheritance with virtual functions and override
- Create template functions or classes where beneficial
- Ensure proper encapsulation with getter/setter methods

#### **Memory Management** (Need 3+ criteria)
- Use const references for function parameters
- Implement smart pointers (unique_ptr, shared_ptr, weak_ptr)
- Add move semantics and Rule of Five where appropriate
- Follow RAII principles strictly
- Use references to avoid unnecessary copying

#### **Concurrency** (Need 2+ criteria)
- Implement multithreading with proper synchronization
- Use mutex/lock_guard for shared data protection
- Implement promise/future or condition variables
- Consider async file operations

### 3. **Implementation Guidelines**

#### **Code Quality Standards**
- Use `const` wherever possible
- Implement proper exception handling
- Add clear documentation for all public methods
- Follow consistent naming conventions
- Never add comments unless explicitly requested

#### **File Organization**
- Maintain existing file structure: `src/` directory with `.h/.cpp` pairs
- Keep class declarations in header files
- Implement methods in corresponding `.cpp` files
- Use include guards or `#pragma once`

#### **Build System**
- Use existing CMake configuration
- Ensure C++17 standard compliance
- Link SDL2 dynamically
- Maintain cross-platform compatibility

### 4. **Feature Implementation Process**

#### **Before Starting Any Implementation:**
1. Read `@PROJECT_DOCUMENTATION.md` completely
2. Identify which rubric criteria the feature will address
3. Plan implementation to follow existing patterns
4. Consider impact on all four rubric categories

#### **During Implementation:**
1. Maintain separation of concerns across classes
2. Use appropriate access specifiers for all members
3. Implement constructors with member initialization lists
4. Add smart pointers and references where beneficial
5. Consider concurrency implications

#### **After Implementation:**
1. Verify all rubric criteria are still being met
2. Test that existing patterns are maintained
3. Ensure no breaking changes to established interfaces
4. Validate build system still works correctly

### 5. **Class Responsibilities (DO NOT BREAK)**

#### **Game Class** (`src/game.h/cpp`)
- Core game logic and state management
- Main game loop implementation
- Food placement and collision detection
- Score tracking and timing control

#### **Snake Class** (`src/snake.h/cpp`)
- Player entity behavior and data
- Movement and growth mechanics
- Self-collision detection
- Body management with std::vector

#### **Controller Class** (`src/controller.h/cpp`)
- Input handling ONLY
- Keyboard event processing
- Direction change validation
- Game exit control

#### **Renderer Class** (`src/renderer.h/cpp`)
- Graphics and display ONLY
- SDL2 resource management
- Snake and food rendering
- Window and title updates

#### **main.cpp**
- Application entry point
- Object coordination and initialization
- Final statistics display

### 6. **Common Implementation Patterns**

#### **Adding New Features**
- Determine which existing class should handle the feature
- If no existing class fits, consider creating new class with single responsibility
- Maintain existing interfaces, extend through inheritance if needed
- Add configuration options through file I/O when appropriate

#### **Memory Management**
- Prefer stack allocation over dynamic allocation
- Use smart pointers for any dynamic resources
- Implement RAII for resource cleanup
- Use STL containers for dynamic collections

#### **Threading Considerations**
- Protect shared state with mutexes
- Use atomic variables for simple shared data
- Consider async operations for file I/O
- Implement proper thread synchronization

### 7. **Validation Checklist**

Before completing any feature implementation, verify:

- [ ] Follows all Implementation Patterns from PROJECT_DOCUMENTATION.md
- [ ] Maintains existing class responsibilities and interfaces
- [ ] Uses appropriate access specifiers
- [ ] Implements constructors with member initialization lists
- [ ] Uses const correctness throughout
- [ ] Includes smart pointers or references where appropriate
- [ ] Builds successfully with existing CMake configuration
- [ ] Maintains 60 FPS performance target
- [ ] Preserves cross-platform compatibility

## Emergency Guidelines

### **If Patterns Conflict with Requirements:**
1. Prioritize rubric compliance over convenience
2. Extend existing patterns rather than breaking them
3. Create new classes if existing ones cannot handle requirements
4. Document any necessary deviations with clear justification

### **If Build Fails:**
1. Check C++17 standard compliance
2. Verify SDL2 linkage
3. Ensure all headers are properly included
4. Check for missing member initialization lists

Remember: The goal is to implement features that meet rubric criteria while maintaining the established architectural patterns and code quality standards.