CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -static-libgcc -static-libstdc++ -static -lpthread

SRC_DIR = src
BUILD_DIR = build
BIN = $(BUILD_DIR)/stupid-db

SRCS = $(shell find $(SRC_DIR) -name "*.cpp")

run:
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS) -o $(BIN)
	@echo "=== Compilation ended ==="
	@echo "=== Running ==="
	./$(BIN)

clean:
	rm -rf $(BUILD_DIR)
	@echo "=== Cleared ==="

.PHONY: run clean
