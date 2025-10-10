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