CC = gcc
CFLAGS = -Wall -Wno-unused-variable -Wno-unused-function -Wno-main # -lcurses # -Wextra -g
TARGET = snake_c
SRC_DIR = ./src
OBJ_DIR = ./obj

SRCS = $(wildcard $(SRC_DIR)/*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET) $(OBJ_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

run: $(TARGET)
	./$(TARGET)

clear:
	rm -f $(OBJ_DIR)/*.o $(TARGET)

clean-objects:
	rm -f $(OBJ_DIR)/*.o

clear-target:
	rm -f $(TARGET)

test:
	./$(TARGET) # Припустимо, що my_program - це програма, яка потребує запуску для тестування

help:
	@echo "Available targets:"
	@echo "  all        : Compile the project"
	@echo "  clear      : Remove object files and executable"
	@echo "  clear-objects : Remove object files"
	@echo "  clear-target  : Remove executable"
	@echo "  test       : Run tests"
