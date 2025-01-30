
ifeq ($(OS),Windows_NT)
    TEST_DIR = game\test
    BUILD_DIR = game\build
else
    TEST_DIR = game/test
    BUILD_DIR = game/build
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
	@cd $(BUILD_DIR) && cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ../..
else
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ../..
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
	@if exist "$(TEST_DIR)\pics" (xcopy /E /I /Y "$(TEST_DIR)\pics" "test_pics_temp\" > nul)
	@if exist "$(TEST_DIR)\.gdbinit" (copy "$(TEST_DIR)\.gdbinit" "test_gdbinit_temp" > nul)
	@if exist "$(BUILD_DIR)\pics" (xcopy /E /I /Y "$(BUILD_DIR)\pics" "pics_temp\" > nul)
	@if exist "$(BUILD_DIR)\audio" (xcopy /E /I /Y "$(BUILD_DIR)\audio" "audio_temp\" > nul)
	@if exist "$(BUILD_DIR)\models" (xcopy /E /I /Y "$(BUILD_DIR)\models" "models_temp\" > nul)
	@if exist "$(BUILD_DIR)\mods" (xcopy /E /I /Y "$(BUILD_DIR)\mods" "mods_temp\" > nul)
	@if exist "$(BUILD_DIR)\shaders" (xcopy /E /I /Y "$(BUILD_DIR)\shaders" "shaders_temp\" > nul)
	@if exist "$(BUILD_DIR)\.gdbinit" (copy "$(BUILD_DIR)\.gdbinit" ".gdbinit_temp" > nul)
	@if exist "$(BUILD_DIR)\$(PROJECT_TITLE).exe" (copy "$(BUILD_DIR)\$(PROJECT_TITLE).exe" "proj_temp.exe" > nul)
	@rd /s /q "$(BUILD_DIR)" 2>nul || exit 0
	@rd /s /q "$(TEST_DIR)" 2>nul || exit 0
	@$(MAKE) $(BUILD_DIR)
	@$(MAKE) create_test_dir
	@if exist "test_pics_temp" (move /Y "test_pics_temp" "$(TEST_DIR)\pics" > nul)
	@if exist "test_gdbinit_temp" (move /Y "test_gdbinit_temp" "$(TEST_DIR)\.gdbinit" > nul)
	@if exist "pics_temp" (move /Y "pics_temp" "$(BUILD_DIR)\pics" > nul)
	@if exist "audio_temp" (move /Y "audio_temp" "$(BUILD_DIR)\audio" > nul)
	@if exist "models_temp" (move /Y "models_temp" "$(BUILD_DIR)\models" > nul)
	@if exist "mods_temp" (move /Y "mods_temp" "$(BUILD_DIR)\mods" > nul)
	@if exist "shaders_temp" (move /Y "shaders_temp" "$(BUILD_DIR)\shaders" > nul)
	@if exist ".gdbinit_temp" (move /Y ".gdbinit_temp" "$(BUILD_DIR)\.gdbinit" > nul)
	@if exist "proj_temp.exe" (move /Y "proj_temp.exe" "$(BUILD_DIR)\$(PROJECT_TITLE).exe" > nul)
else
	@if [ -d "$(TEST_DIR)/pics" ]; then cp -r "$(TEST_DIR)/pics" ./test_pics_temp; fi
	@if [ -f "$(TEST_DIR)/.gdbinit" ]; then cp "$(TEST_DIR)/.gdbinit" ./test_gdbinit_temp; fi
	@if [ -d "$(BUILD_DIR)/pics" ]; then cp -r "$(BUILD_DIR)/pics" ./pics_temp; fi
	@if [ -d "$(BUILD_DIR)/audio" ]; then cp -r "$(BUILD_DIR)/audio" ./audio_temp; fi
	@if [ -d "$(BUILD_DIR)/models" ]; then cp -r "$(BUILD_DIR)/models" ./models_temp; fi
	@if [ -d "$(BUILD_DIR)/mods" ]; then cp -r "$(BUILD_DIR)/mods" ./mods_temp; fi
	@if [ -d "$(BUILD_DIR)/shaders" ]; then cp -r "$(BUILD_DIR)/shaders" ./shaders_temp; fi
	@if [ -f "$(BUILD_DIR)/.gdbinit" ]; then cp "$(BUILD_DIR)/.gdbinit" ./.gdbinit_temp; fi
	@if [ -f "$(BUILD_DIR)/$(PROJECT_TITLE)" ]; then cp "$(BUILD_DIR)/$(PROJECT_TITLE)" ./proj_temp; fi
	@rm -rf $(BUILD_DIR)
	@rm -rf $(TEST_DIR)
	@$(MAKE) $(BUILD_DIR)
	@$(MAKE) create_test_dir
	@if [ -d "test_pics_temp" ]; then mv test_pics_temp "$(TEST_DIR)/pics"; fi
	@if [ -f "test_gdbinit_temp" ]; then mv test_gdbinit_temp "$(TEST_DIR)/.gdbinit"; fi
	@if [ -d "pics_temp" ]; then mv pics_temp "$(BUILD_DIR)/pics"; fi
	@if [ -d "audio_temp" ]; then mv audio_temp "$(BUILD_DIR)/audio"; fi
	@if [ -d "models_temp" ]; then mv models_temp "$(BUILD_DIR)/models"; fi
	@if [ -d "mods_temp" ]; then mv mods_temp "$(BUILD_DIR)/mods"; fi
	@if [ -d "shaders_temp" ]; then mv shaders_temp "$(BUILD_DIR)/shaders"; fi
	@if [ -f ".gdbinit_temp" ]; then mv .gdbinit_temp "$(BUILD_DIR)/.gdbinit"; fi
	@if [ -f "proj_temp" ]; then mv proj_temp "$(BUILD_DIR)/$(PROJECT_TITLE)"; fi
endif
	@echo "Cleaned build and test directories while preserving pics, audio, models folders, and .gdbinit files!"

