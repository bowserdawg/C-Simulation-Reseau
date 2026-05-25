# Crédits à Claude pour ce MakeFile

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
CFLAGS    := -Wall -Wextra -Wpedantic -std=c2x -I$(INC_DIR)
LDFLAGS   :=   # Add linker flags here (e.g. -lm)
 
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
 
# ==============================================================================
# Targets
# ==============================================================================
 
.PHONY: all clean purge help
 
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
 
# Include auto-generated dependency rules (silently skip if missing) ----------
-include $(DEPS)
 
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
	@echo "  clean   Remove the executable from the root directory"
	@echo "  purge   Remove the root executable AND the entire build/ directory"
	@echo "  help    Show this message"
	@echo ""
