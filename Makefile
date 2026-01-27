BACKEND_DIR := back-end
FRONTEND_CLI_DIR := front-end-cli

.PHONY: make run_frontend_cli run_backend clean

make:
	$(MAKE) -C $(BACKEND_DIR)
	$(MAKE) -C $(FRONTEND_CLI_DIR)

run_front-end-cli:
	$(MAKE) -C $(FRONTEND_CLI_DIR) run

run_back-end-cli:
	$(MAKE) -C $(BACKEND_DIR) run

clean:
	$(MAKE) -C $(BACKEND_DIR) clean
	$(MAKE) -C $(FRONTEND_CLI_DIR) clean