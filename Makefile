
ifeq ($(OS),Windows_NT)
    TEST_DIR = game\test
    BUILD_DIR = game\build
	RELEASE_DIR = game\release
else
    TEST_DIR = game/test
    BUILD_DIR = game/build
	RELEASE_DIR = game/release
endif
PROJECT_TITLE = OpenGLApp

# Targets
default: all

# Create build directory if it doesn't exist
$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	@mkdir $(BUILD_DIR) 2>nul || exit 0
else
	@mkdir -p $(BUILD_DIR)
endif

create_test_dir:
ifeq ($(OS),Windows_NT)
	@mkdir $(TEST_DIR) 2>nul || exit 0
else
	@mkdir -p $(TEST_DIR)
endif

create_release_dir:
ifeq ($(OS),Windows_NT)
	@mkdir $(RELEASE_DIR) 2>nul || exit 0
else
	@mkdir -p $(RELEASE_DIR)
endif

all: $(BUILD_DIR)
ifeq ($(OS),Windows_NT)
	@cd $(BUILD_DIR) && make --no-print-directory && $(PROJECT_TITLE).exe
else
	@cd $(BUILD_DIR) && make --no-print-directory && ./$(PROJECT_TITLE)
endif

r: create_release_dir
ifeq ($(OS),Windows_NT)
	@cd $(RELEASE_DIR) && make --no-print-directory && $(PROJECT_TITLE).exe
else
	@cd $(RELEASE_DIR) && make --no-print-directory && ./$(PROJECT_TITLE)
endif

db: $(BUILD_DIR)
	@cd $(BUILD_DIR) && make --no-print-directory && gdb --tui -q -ex "run" --args ./$(PROJECT_TITLE)

# Debug build
debug: $(BUILD_DIR)
	@echo "Building in Debug mode..."
ifeq ($(OS),Windows_NT)
	@cd $(BUILD_DIR) && cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ../..
else
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ../..
endif


# Release build
release: create_release_dir
	@echo "Building in Release mode..."
ifeq ($(OS),Windows_NT)
	@cd $(RELEASE_DIR) && cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ..
else
	@cd $(RELEASE_DIR) && cmake -DCMAKE_BUILD_TYPE=Release -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ..
endif


test: create_test_dir
ifeq ($(OS),Windows_NT)
	@cd $(TEST_DIR) && cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=TRUE -DGAME_TITLE:STRING=$(PROJECT_TITLE) .. && make -s && $(PROJECT_TITLE).exe
else
	@cd $(TEST_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=TRUE -DGAME_TITLE:STRING=$(PROJECT_TITLE) .. && make -s && ./$(PROJECT_TITLE)
endif

cleantest:
ifeq ($(OS),Windows_NT)
	@if exist "$(TEST_DIR)\.gdbinit" (copy "$(TEST_DIR)\.gdbinit" "test_gdbinit_temp" > nul)
	@rd /s /q "$(TEST_DIR)" 2>nul || exit 0
	@$(MAKE) create_test_dir
	@if exist "test_gdbinit_temp" (move /Y "test_gdbinit_temp" "$(TEST_DIR)\.gdbinit" > nul)
else
	@if [ -f "$(TEST_DIR)/.gdbinit" ]; then cp "$(TEST_DIR)/.gdbinit" ./test_gdbinit_temp; fi
	@rm -rf $(TEST_DIR)
	@$(MAKE) create_test_dir
	@if [ -f "test_gdbinit_temp" ]; then mv test_gdbinit_temp "$(TEST_DIR)/.gdbinit"; fi
endif

cleanr:
ifeq ($(OS),Windows_NT)
	@rd /s /q "$(RELEASE_DIR)" 2>nul || exit 0
else
	@rm -rf $(RELEASE_DIR)
endif
	@echo "Cleaned release directory!"

clean:
ifeq ($(OS),Windows_NT)
	@if exist "$(BUILD_DIR)\.gdbinit" (copy "$(BUILD_DIR)\.gdbinit" ".gdbinit_temp" > nul)
	@if exist "$(BUILD_DIR)\$(PROJECT_TITLE).exe" (copy "$(BUILD_DIR)\$(PROJECT_TITLE).exe" "proj_temp.exe" > nul)
	@rd /s /q "$(BUILD_DIR)" 2>nul || exit 0
	@$(MAKE) $(BUILD_DIR)
	@if exist ".gdbinit_temp" (move /Y ".gdbinit_temp" "$(BUILD_DIR)\.gdbinit" > nul)
	@if exist "proj_temp.exe" (move /Y "proj_temp.exe" "$(BUILD_DIR)\$(PROJECT_TITLE).exe" > nul)
else
	@if [ -f "$(BUILD_DIR)/.gdbinit" ]; then cp "$(BUILD_DIR)/.gdbinit" ./.gdbinit_temp; fi
	@if [ -f "$(BUILD_DIR)/T_TITLE)" ]; then cp "$(BUILD_DIR)/$(PROJECT_TITLE)" ./proj_temp; fi
	@rm -rf $(BUILD_DIR)
	@$(MAKE) $(BUILD_DIR)
	@if [ -f ".gdbinit_temp" ]; then mv .gdbinit_temp "$(BUILD_DIR)/.gdbinit"; fi
	@if [ -f "proj_temp" ]; then mv proj_temp "$(BUILD_DIR)/$(PROJECT_TITLE)"; fi
endif
	@echo "Cleaned build directory!"

