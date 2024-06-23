# Main snake program

CC = gcc
CFLAGS = -Wall -Wno-unused-variable -Wno-unused-function -Wno-main #-std=c99 # -lcurses # -Wextra -g
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
TARGET = $(BIN_DIR)/snake_c

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(OBJ_DIR) $(BIN_DIR) $(TARGET)

$(BIN_DIR):
	mkdir $(BIN_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

run: $(TARGET)
	./$(TARGET)

clear:
	rm -f $(OBJ_DIR)/*.o
	rm -rf $(BIN_DIR)/*

clean-objects:
	rm -f $(OBJ_DIR)/*.o

clear-bins:
	rm -f $(BIN_DIR)/




# Another program client server for capturing logs from snake program

SERVER  = $(BIN_DIR)/server_bin
TESTING_DIR = ./testing

$(SERVER): $(TESTING_DIR)/server.c
	$(CC) $^ -o $@

server: $(SERVER)

server-run: $(SERVER)
	./$(SERVER)




# Plaugound for testing features of C language

PLAYGROUND_DIR = ../playground

file-name ?=
playground-run: 
	$(CC) $(PLAYGROUND_DIR)/$(file-name) -o $(PLAYGROUND_DIR)/bin
	./$(PLAYGROUND_DIR)/bin

playground-clear:
	rm $(PLAYGROUND_DIR)/bin





help:
	@echo "Available targets:"
	@echo "  all        	: Compile the project"
	@echo "  clear      	: Remove object files and executable"
	@echo "  clear-objects 	: Remove object files"
	@echo "  clear-target  	: Remove executable"
#	@echo "  testing       	: Run tests"
