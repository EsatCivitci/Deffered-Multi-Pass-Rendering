# Compiler and flags
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iheaders -D__STDC_VERSION__=201710L `pkg-config --cflags freetype2`

# Optional: Disable HDR in stb_image if you don’t need it
# CXXFLAGS += -DSTBI_NO_HDR

# Libraries to link against
LIBS     = -lGLEW -lglfw -lGL `pkg-config --libs freetype2`

# Source and object files
SRCS     = src/main.cpp src/Shader.cpp src/Utils.cpp src/Camera.cpp src/CubeMap.cpp src/Armadillo.cpp src/Quad.cpp
OBJS     = $(SRCS:.cpp=.o)

# Target executable name
TARGET   = main

# Default target
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LIBS)

# Compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(TARGET) $(OBJS)

