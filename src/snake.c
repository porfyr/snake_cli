#include "game_arch.h"


static int  snake_check_food_collision(const Snake *p_snake, size_t food_r, size_t food_c);
static void snake_sync_map(const Snake *p_snake);


static void choose_init_direction(Snake snake, int *direction_r, int *direction_c) {
    // Тут мало б повертати напрямок протилежний до найближчого краю
    // Перед тим приймало // Map *p_map, size_t snake_row, size_t snake_col
    *direction_r = 0;
    *direction_c = -1;
    // int* right_direction = (int *)calloc(2, sizeof(int));
    // right_direction[0] = 0;   // 1 вверх  -1 вниз
    // right_direction[1] = -1; // -1 вліво   1 вправо
    // return right_direction;
}


Snake* snake_new(Map *p_map)
{
    Snake *p_snake = (Snake *)calloc(1, sizeof(Snake *));
    udp_log("\n\n\n______________________________\n\nЕкспериментальна ініціалізація");
    p_snake->p_map = p_map;
    int direction_r = 0, direction_c = 0;
    choose_init_direction(*p_snake, &direction_r, &direction_c);
    snake_add_part(p_snake, direction_r, direction_c);
    snake_map_set_food(p_snake);

    return p_snake;
}


int snake_add_part(Snake *p_snake, int direction_r, int direction_c)
{
    Snake_part sp;
    sp.direction[0] = direction_r;
    sp.direction[1] = direction_c;
    if (p_snake->sp_head == NULL) {
        sp.coords[0] = (rand() % p_snake->p_map->height-2) + 2;
        sp.coords[1] = (rand() % p_snake->p_map->width-2) + 2;
        sp.length = INIT_LENGTH;
    } else {
        int cur_direction[2] = {p_snake->sp_head->data.direction[0], p_snake->sp_head->data.direction[1]};
        // dd{r/c} - difference direction row/column
        int ddr = cur_direction[0] + direction_r;
        int ddc = cur_direction[1] + direction_c;
        if (ddr == 2 || ddr == 0 || ddr == -2 || ddc == 2 || ddc == 0 || ddc == -2) {
            return -1;
        } else {
            for (int i = 0; i < 3; ++i) {
                // snake_map_set_food(p_snake);
            }
            sp.coords[0] = p_snake->sp_head->data.coords[0];
            sp.coords[1] = p_snake->sp_head->data.coords[1];
            sp.length = 0;
        }
    }
    p_snake->sp_head = sp_push(p_snake->sp_head, sp);

    return 0;
}


static int snake_check_food_collision(const Snake *p_snake, size_t food_r, size_t food_c)
{
    // udp_log("достукався в провірку колізії");
    int is_collision = 0;

    Node *current_sp = p_snake->sp_head;
    while (current_sp != NULL) {
        for (
                int i = 0, r = current_sp->data.coords[0], c = current_sp->data.coords[1];
                i < current_sp->data.length;
                ++i,
                r -= current_sp->data.direction[0],
                c -= current_sp->data.direction[1]
            ) {
                if (food_r == r && food_c == c)
                    is_collision = 1;
        }
        current_sp = NULL;
    }
    // if (is_collision) udp_log("провірку колізії не пройдено");

    return is_collision;
}


void snake_map_set_food(const Snake *p_snake)
{
    srand(time(NULL));
    int rand_row, rand_col;
    int done = 0;
    while (!done) {
        rand_row = (rand() % p_snake->p_map->height-1) + 1;
        rand_col = (rand() % p_snake->p_map->width-1) + 1;
        if (!snake_check_food_collision(p_snake, rand_row, rand_col))
            done = 1;
    }
    p_snake->p_map->food_coords[0] = rand_row;
    p_snake->p_map->food_coords[1] = rand_col;
    p_snake->p_map->buffer[rand_row][rand_col] = '*';
}


void snake_move_step(Snake *p_snake)
{
    Node* current_sp = p_snake->sp_head;
    current_sp->data.coords[0] += current_sp->data.direction[0];
    current_sp->data.coords[1] += current_sp->data.direction[1];
    if (p_snake->p_map->buffer[current_sp->data.coords[0]][current_sp->data.coords[1]] == '*') {
        udp_log("схавав");
        snake_map_set_food(p_snake);
    }

    int is_first_part = 1;
    Node *prev_sp = NULL;
    while (current_sp != NULL) {
        if (is_first_part) {
            if (current_sp->next != NULL) {
                current_sp->data.length ++;
            }
            is_first_part = 0;
        }
        // Якщо цей кусок не перший і останній
        else if (current_sp->next == NULL) {
            // Якщо цей кусок має довжину 1 й не перший
            if (current_sp->data.length == 1) {

                p_snake->p_map->buffer[current_sp->data.coords[0]][current_sp->data.coords[1]] = ' ';
                free(current_sp);
                current_sp = NULL;
                prev_sp->next = NULL;
                break;
            }
            else {
                current_sp->data.length --;
            }
        }

        // рух до наступного куска, якщо
        if (current_sp != NULL) {
            prev_sp = current_sp;
            current_sp = current_sp->next;
        }
    }
    snake_sync_map(p_snake);
}


static void snake_sync_map(const Snake *p_snake)
{
    Node *current_sp = p_snake->sp_head;
    while (current_sp != NULL) {
        for (
                int i = 0, r = current_sp->data.coords[0], c = current_sp->data.coords[1];
                i < current_sp->data.length;
                ++i,
                r -= current_sp->data.direction[0],
                c -= current_sp->data.direction[1]
            ) {
                p_snake->p_map->buffer[r][c] = '#';
        }

        // Якщо цей кусок останній й пофіг чи перший (прибирання за хвостиком)
        if (current_sp->next == NULL) {
            size_t r = current_sp->data.coords[0];
            size_t c = current_sp->data.coords[1];
            int r_diff = current_sp->data.direction[0] * current_sp->data.length;
            int c_diff = current_sp->data.direction[1] * current_sp->data.length;
            p_snake->p_map->buffer[r-r_diff][c-c_diff] = ' ';
        }

        current_sp = current_sp->next;
    }
}


void snake_free(Snake *p_snake)
{
    sp_free_list(p_snake->sp_head);
}