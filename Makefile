# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall

# Linker flags
LDFLAGS = -lboost_filesystem -lboost_system -lpugixml -licuuc -licudata -licui18n

# Output executable
TARGET = redatamconverter

# Source files
SRCS = $(wildcard *.cpp)

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET) *~

# Phony targets
.PHONY: all clean

# Format
format:
	clang-format -i *.cpp *.h
