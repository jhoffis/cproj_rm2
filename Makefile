TEST_DIR = test
BUILD_DIR = build
PROJECT_TITLE = OpenGLApp

# Targets
default: all

# Create build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
create_test_dir:
	@mkdir -p $(TEST_DIR)

all: $(BUILD_DIR)
	cd $(BUILD_DIR) && make && ./$(PROJECT_TITLE)

# Debug build
debug: $(BUILD_DIR)
	@echo "Building in Debug mode..."
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ..

# Release build
release: $(BUILD_DIR)
	@echo "Building in Release mode..."
	cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release -DRUN_TESTS:BOOLEAN=FALSE -DGAME_TITLE:STRING=$(PROJECT_TITLE) ..

test: create_test_dir
	cd $(TEST_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_TESTS:BOOLEAN=TRUE -DGAME_TITLE:STRING=$(PROJECT_TITLE) .. && make && ./$(PROJECT_TITLE)

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(TEST_DIR)
	@echo "Cleaned build and test directories!"
