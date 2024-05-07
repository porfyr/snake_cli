CC = gcc
CFLAGS = -Wall -Wno-unused-variable -Wno-unused-function -Wno-main # -lcurses # -Wextra -g
TARGET = snake_c
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
TESTING_DIR = ./testing

SRCS = $(wildcard $(SRC_DIR)/*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(BIN_DIR) $(BIN_DIR)/$(TARGET) $(OBJ_DIR)

$(BIN_DIR):
	mkdir $(BIN_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

run: $(BIN_DIR)/$(TARGET)
	./$(BIN_DIR)/$(TARGET)

clear:
	rm -f $(OBJ_DIR)/*.o
	rm -rf $(BIN_DIR)/*

clean-objects:
	rm -f $(OBJ_DIR)/*.o

clear-bins:
	rm -f $(BIN_DIR)/

#test:
#	./$(TARGET) # Припустимо, що my_program - це програма, яка потребує запуску для тестування

server:
	$(CC) $(TESTING_DIR)/server.c -o $(BIN_DIR)/server_bin

server-run:
	$(CC) $(TESTING_DIR)/server.c -o $(BIN_DIR)/server_bin && $(BIN_DIR)/server_bin

help:
	@echo "Available targets:"
	@echo "  all        	: Compile the project"
	@echo "  clear      	: Remove object files and executable"
	@echo "  clear-objects 	: Remove object files"
	@echo "  clear-target  	: Remove executable"
#	@echo "  testing       	: Run tests"
