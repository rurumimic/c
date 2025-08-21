.PHONY: all help clean debug release test format clang-format 

all: debug

help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all             Build debug (default)"
	@echo "  debug           Configure & build in debug mode"
	@echo "  release         Configure & build in release mode"
	@echo "  test            Configure, build, and run tests"
	@echo "  format          Run clang-format on source files"
	@echo "  clang-format    Create a .clang-format file"
	@echo "  clean           Remove build directory"
	@echo "  help            Show this help message"
	@echo ""
	@echo "Options:"
	@echo "  VERBOSE=1       Enable verbose mode"

clean:
	@echo "[INFO] Clean build directory:"
	rm -rf build

debug:
	@echo "[INFO] Configure with preset: configure-debug"
	cmake --preset configure-debug
	@echo "[INFO] Build with preset: build-debug"
ifeq ($(VERBOSE), 1)
	cmake --build --preset build-debug --verbose
else
	cmake --build --preset build-debug
endif

release:
	@echo "[INFO] Configure with preset: configure-release"
	cmake --preset configure-release
	@echo "[INFO] Build with preset: build-release"
ifeq ($(VERBOSE), 1)
	cmake --build --preset build-release --verbose
else
	cmake --build --preset build-release
endif

test:
	@echo "[INFO] Configure with preset: configure-test"
	cmake --preset configure-test
	@echo "[INFO] Build with preset: build-unit-tests"
	cmake --build --preset build-unit-tests
	@echo "[INFO] Run tests with preset: run-unit-tests"
ifeq ($(VERBOSE), 1)
	ctest --preset run-unit-tests --output-on-failure --verbose
else
	ctest --preset run-unit-tests --output-on-failure
endif

format:
	cmake --build --preset build-debug --target format

clang-format:
	cmake --preset configure-debug
	cmake --build --preset build-debug --target clang-format
