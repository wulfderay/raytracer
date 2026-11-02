
CXX       := g++
CXXFLAGS  := -std=c++17 -Wall -Wextra -O2 -pthread
CPPFLAGS  := -Isource
LDFLAGS   := -pthread

TARGET    := raytracer
BUILD_DIR := build
SRC_DIR   := source

SOURCES := \
	$(SRC_DIR)/box.cpp \
	$(SRC_DIR)/bvh.cpp \
	$(SRC_DIR)/camera.cpp \
	$(SRC_DIR)/material.cpp \
	$(SRC_DIR)/raytracer.cpp \
	$(SRC_DIR)/renderer.cpp \
	$(SRC_DIR)/stdafx.cpp \
	$(SRC_DIR)/threads.cpp \
	$(SRC_DIR)/transformation.cpp

OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS    := $(OBJECTS:.o=.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
