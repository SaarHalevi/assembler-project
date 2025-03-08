CC = gcc
CFLAGS = -ansi -pedantic -Wall
INCLUDES = -I./include   # Add this to tell gcc where to find header files
SRC_DIR = ./src          # Specify the directory where the source files are

# List of object files
OBJ = $(SRC_DIR)/pre_assembler.o $(SRC_DIR)/general.o $(SRC_DIR)/front_end.o \
      $(SRC_DIR)/first_pass.o $(SRC_DIR)/second_pass.o $(SRC_DIR)/back_end.o \
      $(SRC_DIR)/assembler.o

# Default target: all
all: assembler

# Linking object files to create the executable
assembler: $(OBJ)
	$(CC) $(CFLAGS) $(INCLUDES) -o assembler $(OBJ)

# Compilation rules for .c files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(SRC_DIR)/*.o assembler
