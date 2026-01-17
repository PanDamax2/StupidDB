# -------- Konfiguracja wspólna --------
CXXFLAGS ?= -I/mingw64/include -std=c++17 -Wall -Wextra
SRC_DIR  := src
BUILD_DIR:= build
DLL_FILES_DIR := dll_files
SRCS     := $(shell find $(SRC_DIR) -name "*.cpp")

# Wykrywanie OS (Windows_NT ustawiane przez cmd/PowerShell/MinGW)
ifeq ($(OS),Windows_NT)
  DEFAULT_TARGET := windows
else
  DEFAULT_TARGET := linux
endif

# Pozwól nadpisać TARGET z linii poleceń: make TARGET=windows
TARGET ?= $(DEFAULT_TARGET)

# Sufiks EXE na Windows
ifeq ($(TARGET),windows)
  EXEEXT := .exe
else
  EXEEXT :=
endif

BIN := $(BUILD_DIR)/stupid-db$(EXEEXT)


# -------- Ustawienia per target --------
ifeq ($(TARGET),linux)
  CXX ?= g++
  LDLIBS := -lpthread -lsodium
endif

ifeq ($(TARGET),windows)
  # Uwaga: to działa zarówno przy cross-compile z Linuksa,
  # jak i natywnie w MSYS2/MinGW (tam też CXX to x86_64-w64-mingw32-g++).
  CXX ?= x86_64-w64-mingw32-g++
  # Winsock + (opcjonalnie) pthread w MinGW + libsodium
  LDLIBS := -lsodium -lwinpthread -lws2_32
  # statyczne libgcc/libstdc++ (można wyłączyć: make TARGET=windows STATIC=0)
  LDFLAGS += -L/mingw64/lib -static-libgcc -static-libstdc++ -static -DWIN32_LEAN_AND_MEAN -DNOMINMAX
endif

ifeq ($(TARGET),windows)
  CP_DLLS := @cp -r $(DLL_FILES_DIR)/* $(BUILD_DIR)/
else
  CPP_DLLS :=
endif

# -------- Reguły --------
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
	$(CXX) $(CXXFLAGS) $(SRCS) $(LDFLAGS) $(LDLIBS) -o $(BIN)

run: all
	./$(BIN)

clean:
	rm -rf $(BUILD_DIR)
