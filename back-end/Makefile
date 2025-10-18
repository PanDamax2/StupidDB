CFLAGS := -Wall -g -static-libgcc -static-libstdc++ -static

main:
	@mkdir -p bin
	@$(CXX) $(CFLAGS) src/*.cpp -o bin/stupid-db

clean:
	@echo Cleaning...
	@rm -r bin