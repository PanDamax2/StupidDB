# Kompilator i flaga
CC = g++  # Lub g++ dla C++
CFLAGS = -Wall -g -Iinclude  # Flagi: ostrzeżenia, debug, ścieżka do headers

# Nazwa projektu i pliki źródłowe
TARGET = build/projekt.exe
SOURCES = src/main.cpp src/utils.cpp  
OBJECTS = $(patsubst src/%.cpp,build/%.o,$(SOURCES))

all: $(TARGET)

# Budowanie 
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Kompilacja 
build/%.o: src/%.cpp
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Czyszczenie
clean:
	rm -f $(OBJECTS) $(TARGET)

# PHONY 
.PHONY: all clean



# ----------------------------------------

CFLAGS := -Wall -g

main:
	@mkdir -p bin
	@$(CXX) $(CFLAGS) src/*.cpp -o bin/stupid-db

clean:
	@echo Cleaning...
	@rm -r bin

install:
	@echo Installing...
	@cp bin/stupid-db /usr/bin

uninstall:
	@echo Uninstalling...
	@rm /usr/bin/stupid-db