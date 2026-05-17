# Makefile for RCU

DEVKITPRO := /opt/devkitpro
DEVKITA64 := $(DEVKITPRO)/devkitA64
DEVKITARM := $(DEVKITPRO)/devkitARM

# You may need to adjust these
LIBNX := $(DEVKITPRO)/libnx

# TITLE_ID: 00FF0000636C6BFF
TITLE := RCU
TITLE_ID := 00FF0000636C6BFF
AUTHOR := "RCU Team"

# Sources
SOURCES := $(wildcard src/**/*.cpp) $(wildcard src/**/*.c) $(wildcard src/*.cpp) $(wildcard src/*.c)
OBJECTS := $(SOURCES:.cpp=.o) $(SOURCES:.c=.o)

# Include paths
INCLUDES := -I$(LIBNX)/include -ISource/hoc-clk/source -ISource/common

# Flags
CFLAGS := -Wall -Wextra -O2 -march=armv8-a+simd -mtune=cortex-a57 -mtp=soft -fPIE $(INCLUDES)
CXXFLAGS := -Wall -Wextra -O2 -march=armv8-a+simd -mtune=cortex-a57 -mtp=soft -fPIE -std=c++17 $(INCLUDES)
LDFLAGS := -specs=$(DEVKITPRO)/libnx/switch.specs -g

# Target
TARGET := $(TITLE).nso
BUILD := build

all: $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD) $(OBJECTS)

.PHONY: all clean