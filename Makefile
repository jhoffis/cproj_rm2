TEST_DIR = test
BUILD_DIR = build
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

all: $(BUILD_DIR)
ifeq ($(OS),Windows_NT)
	@cd $(BUILD_DIR) && make --no-print-directory && $(PROJECT_TITLE).exe
else
	@cd $(BUILD_DIR) && make --no-print-directory && ./$(PROJECT_TITLE)
endif

db: $(BUILD_DIR)
	@cd $(BUILD_DIR) && make --no-print-directory && gdb --tui -q -ex "run" --args ./$(PROJECT_TITLE)

# Debug build
debug: $(BUILD_DIR)
	@echo "Building in Debug mode..."
ifeq ($(OS),Windows_NT)
	@cd $(BUILD_DIR) && cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ..
else
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ..
endif


# Release build
release: $(BUILD_DIR)
	@echo "Building in Release mode..."
ifeq ($(OS),Windows_NT)
	@cd $(BUILD_DIR) && cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ..
else
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ..
endif


test: create_test_dir
ifeq ($(OS),Windows_NT)
	@cd $(TEST_DIR) && cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=TRUE -DGAME_TITLE:STRING=$(PROJECT_TITLE) .. && make -s && $(PROJECT_TITLE).exe
else
	@cd $(TEST_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=TRUE -DGAME_TITLE:STRING=$(PROJECT_TITLE) .. && make -s && ./$(PROJECT_TITLE)
endif

clean:
ifeq ($(OS),Windows_NT)
	@if exist "$(BUILD_DIR)\pics" (xcopy /E /I /Y "$(BUILD_DIR)\pics" "pics_temp\" > nul)
	@if exist "$(TEST_DIR)\pics" (xcopy /E /I /Y "$(TEST_DIR)\pics" "test_pics_temp\" > nul)
	@rd /s /q "$(BUILD_DIR)" 2>nul || exit 0
	@rd /s /q "$(TEST_DIR)" 2>nul || exit 0
	@if exist "pics_temp" (mkdir "$(BUILD_DIR)" & xcopy /E /I /Y "pics_temp" "$(BUILD_DIR)\pics\" > nul & rd /s /q "pics_temp")
	@if exist "test_pics_temp" (mkdir "$(TEST_DIR)" & xcopy /E /I /Y "test_pics_temp" "$(TEST_DIR)\pics\" > nul & rd /s /q "test_pics_temp")
else
	@if [ -d "$(BUILD_DIR)/pics" ]; then cp -r "$(BUILD_DIR)/pics" ./pics_temp; fi
	@if [ -d "$(TEST_DIR)/pics" ]; then cp -r "$(TEST_DIR)/pics" ./test_pics_temp; fi
	@rm -rf $(BUILD_DIR)
	@rm -rf $(TEST_DIR)
	@if [ -d "pics_temp" ]; then mkdir -p "$(BUILD_DIR)" && mv pics_temp "$(BUILD_DIR)/pics"; fi
	@if [ -d "test_pics_temp" ]; then mkdir -p "$(TEST_DIR)" && mv test_pics_temp "$(TEST_DIR)/pics"; fi
endif
	@echo "Cleaned build and test directories while preserving pics folders!"
