CXXFLAGS ?= -std=c++17 -Wall -Wextra
SRC_DIR  := src
BUILD_DIR:= build
DLL_FILES_DIR := dll_files
SRCS     := $(shell find $(SRC_DIR) -name "*.cpp")

ifeq ($(OS),Windows_NT)
  DEFAULT_TARGET := windows
else
  DEFAULT_TARGET := linux
endif

TARGET ?= $(DEFAULT_TARGET)

ifeq ($(TARGET),windows)
  EXEEXT := .exe
else
  EXEEXT :=
endif

BIN := $(BUILD_DIR)/stupid-db$(EXEEXT)


ifeq ($(TARGET),linux)
  CXX ?= g++
  LDLIBS := -lpthread -lsodium
  LDFLAGS_AFTER_OUTPUT :=
endif

ifeq ($(TARGET),windows)
  CXX ?= x86_64-w64-mingw32-g++
  LDLIBS := -lsodium -lwinpthread
  LDFLAGS += -static-libgcc -static-libstdc++ -static -DWIN32_LEAN_AND_MEAN -DNOMINMAX
  LDFLAGS_AFTER_OUTPUT := -lws2_32 -lmswsock -lwsock32 -lshlwapi
endif

ifeq ($(TARGET),windows)
  CP_DLLS := @cp -r $(DLL_FILES_DIR)/* $(BUILD_DIR)/
else
  CPP_DLLS :=
endif

.PHONY: all linux windows run clean

all: $(TARGET)

linux:
	@$(MAKE) TARGET=linux build

windows:
	@$(MAKE) TARGET=windows build

build: $(BIN)
	@echo "Compilation ended: $(BIN)"

$(BIN):
	@mkdir -p $(BUILD_DIR)
	$(CP_DLLS)
  # wiem że to głupie ale działa XD
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS) $(LDLIBS) -o $(BIN) $(LDFLAGS_AFTER_OUTPUT)
run: all
	./$(BIN)

clean:
	rm -rf $(BUILD_DIR)