# ==============================================================================
# Project Configuration
# ==============================================================================
 
# Name of the final executable
TARGET    := main
 
# Directories
SRC_DIR   := src
BUILD_DIR := build
INC_DIR   := include
DEP_DIR   := $(BUILD_DIR)/deps
 
# Compiler & flags
CC        := gcc
CFLAGS    := -Wall -Wextra -Wpedantic -g -std=c23 -I$(INC_DIR)
LDFLAGS   :=                          # Add linker flags here (e.g. -lm)
 
# ------------------------------------------------------------------------------
# Test configuration
# ------------------------------------------------------------------------------
 
TEST_DIR        := tests
TEST_BUILD_DIR  := $(BUILD_DIR)/tests
TEST_DEP_DIR    := $(BUILD_DIR)/deps/tests
TEST_TARGET     := $(TEST_BUILD_DIR)/test_runner
 
# Flags for test compilation (debug info, no optimisation, coverage-friendly)
TEST_CFLAGS     := -Wall -Wextra -Wpedantic -std=c23 -I$(INC_DIR) -g -O0 -DDEBUG
TEST_LDFLAGS    :=                          # Add test-specific linker flags here
 
# ==============================================================================
# Derived variables  (no need to touch these)
# ==============================================================================
 
# All .c sources found recursively under src/
SRCS      := $(shell find $(SRC_DIR) -name '*.c')
 
# Corresponding .o files under build/
OBJS      := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
 
# Corresponding .d dependency files under build/deps/
DEPS      := $(patsubst $(SRC_DIR)/%.c, $(DEP_DIR)/%.d, $(SRCS))
 
# Final binary locations
TARGET_BUILD := $(BUILD_DIR)/$(TARGET)
TARGET_ROOT  := ./$(TARGET)
 
# ------------------------------------------------------------------------------
# Test derived variables
# ------------------------------------------------------------------------------
 
# All .c sources found recursively under tests/
TEST_SRCS     := $(shell find $(TEST_DIR) -name '*.c')
 
# Their .o files under build/tests/
TEST_OBJS     := $(patsubst $(TEST_DIR)/%.c, $(TEST_BUILD_DIR)/%.o, $(TEST_SRCS))
 
# Their .d dependency files under build/deps/tests/
TEST_DEPS     := $(patsubst $(TEST_DIR)/%.c, $(TEST_DEP_DIR)/%.d, $(TEST_SRCS))
 
# src/ objects minus main.o — tests supply their own entry point
SRC_OBJS_NO_MAIN := $(filter-out $(BUILD_DIR)/main.o, $(OBJS))
 
# ==============================================================================
# Targets
# ==============================================================================
 
.PHONY: all test clean purge help
 
# Default target ---------------------------------------------------------------
all: $(TARGET_BUILD)
 
# Link -------------------------------------------------------------------------
$(TARGET_BUILD): $(OBJS) | $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	cp $@ $(TARGET_ROOT)
	@echo ""
	@echo "  Built  ->  $(TARGET_BUILD)"
	@echo "  Copied ->  $(TARGET_ROOT)"
 
# Compile each .c -> .o, generating a .d file alongside ----------------------
# The -MMD -MP flags make gcc emit a .d file that lists every header the
# translation unit includes, so Make can detect header changes automatically.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR) $(DEP_DIR)
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*.d)
	$(CC) $(CFLAGS) -MMD -MP -MF $(DEP_DIR)/$*.d -c $< -o $@
 
# Directory creation -----------------------------------------------------------
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
 
$(DEP_DIR):
	mkdir -p $(DEP_DIR)
 
$(TEST_BUILD_DIR):
	mkdir -p $(TEST_BUILD_DIR)
 
$(TEST_DEP_DIR):
	mkdir -p $(TEST_DEP_DIR)
 
# Include auto-generated dependency rules (silently skip if missing) ----------
-include $(DEPS)
-include $(TEST_DEPS)
 
# ==============================================================================
# Test target
# ==============================================================================
 
# Build and immediately run the test binary.
# Depends on the src/ objects (excluding main.o) so they are rebuilt first if
# any source file changed — tests always exercise the latest compiled code.
test: $(TEST_TARGET)
	@echo ""
	@echo "  Running tests..."
	@echo ""
	@$(TEST_TARGET); \
	  STATUS=$$?; \
	  echo ""; \
	  if [ $$STATUS -eq 0 ]; then \
	    echo "  All tests passed."; \
	  else \
	    echo "  Tests FAILED (exit code $$STATUS)."; \
	  fi; \
	  exit $$STATUS
 
# Link test objects + src objects (no main.o) into the test runner ------------
$(TEST_TARGET): $(TEST_OBJS) $(SRC_OBJS_NO_MAIN) | $(TEST_BUILD_DIR)
	$(CC) $^ -o $@ $(TEST_LDFLAGS)
	@echo ""
	@echo "  Built  ->  $(TEST_TARGET)"
 
# Compile each tests/*.c -> build/tests/*.o ------------------------------------
$(TEST_BUILD_DIR)/%.o: $(TEST_DIR)/%.c | $(TEST_BUILD_DIR) $(TEST_DEP_DIR)
	@mkdir -p $(dir $@) $(dir $(TEST_DEP_DIR)/$*.d)
	$(CC) $(TEST_CFLAGS) -MMD -MP -MF $(TEST_DEP_DIR)/$*.d -c $< -o $@
 
# ==============================================================================
# Maintenance targets
# ==============================================================================
 
# clean  – remove only the copy of the executable from the root directory
clean:
	@rm -f $(TARGET_ROOT)
	@echo "  Removed $(TARGET_ROOT)"
 
# purge  – remove every build artefact (objects, deps, binaries) from build/
#          AND the root-level copy
purge: clean
	@rm -rf $(BUILD_DIR)
	@echo "  Removed $(BUILD_DIR)/"
 
# ==============================================================================
# Help
# ==============================================================================
 
help:
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "  all     Build the project (default)"
	@echo "  test    Compile and run all tests in tests/"
	@echo "  clean   Remove the executable from the root directory"
	@echo "  purge   Remove the root executable AND the entire build/ directory"
	@echo "  help    Show this message"
	@echo ""
