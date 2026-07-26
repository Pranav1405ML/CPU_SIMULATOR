# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Icpu -Iassembler -Iinstruction

# Source files
SRC = \
	main.c \
	cpu/cpu.c \
	cpu/alu.c \
	assembler/assembler.c

# Output executable
TARGET = cpu

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Run the simulator
run: $(TARGET)
	./$(TARGET)

# Remove generated files
clean:
	rm -f $(TARGET) $(TARGET).exe

# Rebuild from scratch
rebuild: clean all

.PHONY: all run clean rebuild


# mingw32-make    
# mingw32-make run
