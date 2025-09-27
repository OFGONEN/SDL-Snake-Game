# Makefile for Snake Game C++ Project
# Provides convenient commands for building and cleaning the project

# Default target
.DEFAULT_GOAL := build

# Variables
BUILD_DIR = build
CMAKE_FLAGS =
MAKE_FLAGS = -j$(shell nproc)

# Clean target - removes build directory
.PHONY: clean
clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)
	@echo "Clean complete."

# Build target - creates build directory and compiles project
.PHONY: build
build:
	@echo "Creating build directory..."
	mkdir -p $(BUILD_DIR)
	@echo "Configuring project with CMake..."
	cd $(BUILD_DIR) && cmake $(CMAKE_FLAGS) ..
	@echo "Building project..."
	cd $(BUILD_DIR) && make $(MAKE_FLAGS)
	@echo "Build complete. Executable: $(BUILD_DIR)/SnakeGame"

# Rebuild target - clean and build
.PHONY: rebuild
rebuild: clean build

# Run target - build and execute the game
.PHONY: run
run: build
	@echo "Running Snake Game..."
	./$(BUILD_DIR)/SnakeGame

# Install target - build and copy executable to /usr/local/bin (requires sudo)
.PHONY: install
install: build
	@echo "Installing SnakeGame to /usr/local/bin..."
	sudo cp $(BUILD_DIR)/SnakeGame /usr/local/bin/
	@echo "Installation complete. Run with: SnakeGame"

# Debug build target - build with debug flags
.PHONY: debug
debug: CMAKE_FLAGS += -DCMAKE_BUILD_TYPE=Debug
debug: build

# Release build target - build with optimization flags
.PHONY: release
release: CMAKE_FLAGS += -DCMAKE_BUILD_TYPE=Release
release: build

# Help target - display available commands
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  clean     - Remove build directory"
	@echo "  build     - Configure and build project (default)"
	@echo "  rebuild   - Clean and build project"
	@echo "  run       - Build and run the game"
	@echo "  debug     - Build with debug information"
	@echo "  release   - Build with optimizations"
	@echo "  install   - Install executable to /usr/local/bin"
	@echo "  help      - Show this help message"