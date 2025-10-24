FILES = $(shell find src -name "*.cpp")
CFLAGS := -Wall -g -static-libgcc -static-libstdc++ -static


main:
	@mkdir -p bin
	@$(CXX) $(CFLAGS) $(FILES) -o bin/stupid-db

clean:
	@echo Cleaning...
	@rm -r bin