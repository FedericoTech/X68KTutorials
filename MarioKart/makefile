# Variables
SRC_DIR = .
OBJ_DIR = .mariko_objs
BIN_DIR = MarioKart_mariko
LIB_A_NAME = $(OBJ_DIR)/lib1.a
LIB_A_OBJS = $(filter-out $(OBJ_DIR)/main.o, $(OBJS))
EXEC = $(BIN_DIR)/MarioKart.x

# Libraries to link against
LIBS = -ldos -liocs -lsignal
LIB_DIRS = 

# Automatically find all source files
SRCS = $(wildcard *.c)

# Create a list of object files in the OBJ_DIR
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

# Default target
MarioKart_mariko: $(EXEC)

# Rule to create the executable
$(EXEC): $(OBJ_DIR)/lib1.a $(OBJ_DIR)/main.o 
	gcc -o $@ $^ $(LIB_DIRS) $(LIBS)

$(LIB_A_NAME):	$(LIB_A_OBJS)
	run68 ar /b $(OBJ_DIR)/lib1.a $(LIB_A_OBJS)

# Rule to compile source files into object files
$(OBJ_DIR)/%.o: %.c
	if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	gcc -c $< -o $@

# Rule to clean up the build
cleanMarioKart_mariko:
	del /Q /F $(OBJ_DIR)\*.o
	del /Q /F $(OBJ_DIR)\*.a
	del /Q /F $(BIN_DIR)\*.x