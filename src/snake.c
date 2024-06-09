#include "game_arch.h"



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


Snake* snake_new(Map *p_map, pthread_mutex_t *controls_mutex)
{
    Snake *p_snake = (Snake *)malloc(sizeof(Snake));
    udp_log("\n\n\n______________________________\n\nЕкспериментальна ініціалізація");
    p_snake->p_map = p_map;
    int direction_r = 0, direction_c = 0;
    p_snake->controls_mutex = controls_mutex;
    choose_init_direction(*p_snake, &direction_r, &direction_c);
    snake_add_part(p_snake, direction_r, direction_c);
    snake_map_set_food(p_snake);

    return p_snake;
}


int snake_add_part(Snake *p_snake, int direction_r, int direction_c)
{
    pthread_mutex_lock(p_snake->controls_mutex);
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

static void snake_increase(Snake *p_snake)
{
    Node *current_sp = p_snake->sp_head;
    while (current_sp != NULL) {
        if (current_sp->next == NULL) {
            ++ current_sp->data.length;
        }
        current_sp = current_sp->next;
    }
}

static int check_way(const Snake *p_snake)
{   
    size_t r = p_snake->sp_head->data.coords[0];
    size_t c = p_snake->sp_head->data.coords[1];
    switch (p_snake->p_map->buffer[r][c]) {
        case '|':
        case '-':
        case '#':
            return 0;
        default:
            return 1;
    }
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


int snake_move_step(Snake *p_snake)
{
    // 1 if all is good, 0 if not so
    int result = 1;
    Node* current_sp = p_snake->sp_head;
    current_sp->data.coords[0] += current_sp->data.direction[0];
    current_sp->data.coords[1] += current_sp->data.direction[1];
    result = check_way(p_snake);

    int is_first_part = 1;
    Node *prev_sp = NULL;
    while (current_sp != NULL) {
        if (is_first_part) {
            if (current_sp->next != NULL) {
                current_sp->data.length ++;
            }
            is_first_part = 0;
        }
        // If this part isn't first and is last
        else if (current_sp->next == NULL) {
            // If this part has length 1 and isn't first
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

        // move to next part if this iteration didn't destroy it
        if (current_sp != NULL) {
            prev_sp = current_sp;
            current_sp = current_sp->next;
        }
    }
    snake_sync_map(p_snake);
    return result;
}


void snake_free(Snake *p_snake)
{
    sp_free_list(p_snake->sp_head);
}