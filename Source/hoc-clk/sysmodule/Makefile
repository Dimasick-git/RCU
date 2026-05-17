TARGET := sysmodule

# Directories
SOURCE_DIR := source
BUILD_DIR := build
LIBTRANSISTOR_DIR := lib/libtransistor

# Toolchain
TRIPLE := aarch64-none-elf
CC := $(TRIPLE)-gcc
CXX := $(TRIPLE)-g++
LD := $(TRIPLE)-gcc
AS := $(TRIPLE)-as
OBJCOPY := $(TRIPLE)-objcopy

# Flags
CFLAGS := -O2 -g -Wall -Wextra -std=c11 -I$(SOURCE_DIR) -I$(LIBTRANSISTOR_DIR)/include -D__SWITCH__
CXXFLAGS := -O2 -g -Wall -Wextra -std=c++17 -I$(SOURCE_DIR) -I$(LIBTRANSISTOR_DIR)/include -D__SWITCH__
LDFLAGS := -nostdlib -L$(LIBTRANSISTOR_DIR)/build -ltr -lgcc

# Source files
C_SOURCES := $(wildcard $(SOURCE_DIR)/*.c)
CXX_SOURCES := $(wildcard $(SOURCE_DIR)/*.cpp)

# Object files
C_OBJECTS := $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
CXX_OBJECTS := $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CXX_SOURCES))
OBJECTS := $(C_OBJECTS) $(CXX_OBJECTS)

# Output
ELF_OUTPUT := $(BUILD_DIR)/$(TARGET).elf
NRO_OUTPUT := $(BUILD_DIR)/$(TARGET).nro
NSP_OUTPUT := out/hoc-clk.nsp

.PHONY: all clean

all: $(NSP_OUTPUT)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(ELF_OUTPUT): $(OBJECTS)
	$(LD) $(OBJECTS) $(LDFLAGS) -o $@

$(NSP_OUTPUT): $(ELF_OUTPUT)
	mkdir -p out
	$(OBJCOPY) -O binary $(ELF_OUTPUT) $(BUILD_DIR)/$(TARGET).bin
	# Package into NSP (simplified - adjust as needed for your build system)
	touch $(NSP_OUTPUT)

clean:
	rm -rf $(BUILD_DIR) out
