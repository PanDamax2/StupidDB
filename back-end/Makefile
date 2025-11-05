CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -static-libgcc -static-libstdc++ -static -lpthread
SRC_DIR = src
BUILD_DIR = build
BIN = $(BUILD_DIR)/projekt

SRCS = $(shell find $(SRC_DIR) -name "*.cpp")

all: $(BIN)

$(BIN):
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS) -o $(BIN)
	@echo "Compilation ended: $(BIN)"

run: all
	./$(BIN)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean