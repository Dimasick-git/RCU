# Makefile for sysmodule

# --- Project settings ---
TARGET := hoc-clk
TITLE_ID := 00FF0000636C6BFF

# --- Directories ---
SRC_DIR := source
BUILD_DIR := build
OUT_DIR := out

# --- Include paths ---
INCLUDES := -I$(SRC_DIR)

# --- Source files ---
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
SRC_FILES += $(wildcard $(SRC_DIR)/*.c)

# --- Object files ---
OBJ_FILES := $(SRC_FILES:%=$(BUILD_DIR)/%.o)

# --- Compiler flags ---
CXXFLAGS := -O2 -Wall -Wextra $(INCLUDES)
CFLAGS := -O2 -Wall -Wextra $(INCLUDES)

# --- Linker flags ---
LDFLAGS := -specs=switch.specs

# --- Targets ---
.PHONY: all clean

all: $(OUT_DIR)/$(TARGET).nsp

$(OUT_DIR)/$(TARGET).nsp: $(OBJ_FILES)
	@mkdir -p $(OUT_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR) $(OUT_DIR)
