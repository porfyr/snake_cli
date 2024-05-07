#include "game_arch.h"


static void choose_init_direction(Snake snake, int* direction_r, int* direction_c)
{
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
    udp_log("Експериментальна ініціалізація\n");
    p_snake->p_map = p_map;
    int direction_r = 0, direction_c = 0;
    choose_init_direction(*p_snake, &direction_r, &direction_c);
    snake_add_part(p_snake, direction_r, direction_c);
    
    udp_log("Вертаємо вказівник на змію (майже)\n");
    char msg[BUFFER_SIZE];
    sprintf(msg, "перша коорд. (щас довжина) змії %ld\n", p_snake->sp_head->data.length);
    udp_log(msg);
    // printf("перша коорд. (щас довжина) змії %ld\n", p_snake->sp_head->data.length);
    return p_snake;
}

int snake_add_part(Snake *p_snake, int direction_r, int direction_c)
{
    udp_log("Викликано snake_add_part");
    Snake_part sp;
    sp.direction[0] = direction_r;
    sp.direction[1] = direction_c;
    if (p_snake->sp_head == NULL) {
        sp.coords[0] = (rand() % p_snake->p_map->height-1) + 1;
        sp.coords[1] = (rand() % p_snake->p_map->width-1) + 1;
        sp.length = INIT_LENGTH;
    } else {
        int cur_direction[2] = {p_snake->sp_head->data.direction[0], p_snake->sp_head->data.direction[1]};
        int ddr = cur_direction[0] + direction_r;
        int ddc = cur_direction[1] + direction_c;
        if (ddr == 2 || ddr == 0 || ddr == -2 || ddc == 2 || ddc == 0 || ddc == -2) {
            // printf("Поворот неумісний %d %d : %d %d\n", cur_direction[0], cur_direction[1], direction_r, direction_c);
            char msg[BUFFER_SIZE];
            sprintf(msg, "поворот неумісний %d %d : %d %d\n", cur_direction[0], cur_direction[1], direction_r, direction_c);
            udp_log(msg);
            return -1;
        } else {
            // printf("Поворот умісний %d %d : %d %d\n", cur_direction[0], cur_direction[1], direction_r, direction_c);
            sp.coords[0] = p_snake->sp_head->data.coords[0] + direction_r;
            sp.coords[1] = p_snake->sp_head->data.coords[1] + direction_c;
            sp.length = 1;
            // p_snake->sp_head = sp_push(p_snake->sp_head, sp);
        }
    }
    p_snake->sp_head = sp_push(p_snake->sp_head, sp);
    sp_print_list(p_snake->sp_head);
    return 0;
}


void snake_move_step(Snake *p_snake) {
    p_snake->sp_head->data.coords[0] += p_snake->sp_head->data.direction[0];
    p_snake->sp_head->data.coords[1] += p_snake->sp_head->data.direction[1];
    if (p_snake->sp_head->next != NULL) {
        p_snake->sp_head->data.length ++;
    }
    Node* current = p_snake->sp_head;
    int is_single_part = sp_get_length(p_snake->sp_head) == 1;
    do {
        if (current->next == NULL) {
            if (current->data.length == 1) {
                udp_log("Вичерпалась частинка змії");
                free(current);
            } else if (current->next != NULL && is_single_part) {
                is_single_part = 0;
            }
                // sp_shrink();
            if (!is_single_part) {
                char msg[BUFFER_SIZE];
                sprintf(msg, "Зменшуєм частинку на 1, %d", is_single_part);
                udp_log(msg);
                // printf("Зменшуєм частинку на 1, %d\n", is_single_part);
                current->data.length --;
            }
            // size_t coords[2] = {current->data.coords[0], current->data.coords[1]};
            size_t r = current->data.coords[0];
            size_t c = current->data.coords[1];
            int r_diff = current->data.direction[0] * current->data.length;
            int c_diff = current->data.direction[1] * current->data.length;
            // printf("прибирання %ld %ld :: %d %d\n", r, c, r_diff, c_diff);
            p_snake->p_map->buffer[r-r_diff][c-c_diff] = ' ';
        }
    } while (current->next != NULL);
}


void snake_sync_map(Snake snake) {
    size_t parts_count = sp_get_length(snake.sp_head);
    // printf("try_sync parts_count %ld\n", parts_count);
    for (size_t sp_i = 0; sp_i < parts_count; ++sp_i) {
        Snake_part *sp = get_sp_by_index(snake.sp_head, sp_i);
        int direction[2] = {sp->direction[0], sp->direction[1]};
        size_t coords[2] = {sp->coords[0], sp->coords[1]};
        // printf("try sync direction: %d %d, coords: %ld %ld\n", direction[0], direction[1], coords[0], coords[1]);
        for (size_t i = 0, r = coords[0], c = coords[1]; i < sp->length; ++i) {
            snake.p_map->buffer[r][c] = '#';
            r -= direction[0];
            c -= direction[1];
            //printf("матика в sync %ld %ld :: %d %ld\n", r, c, coords[0] + sp->length * direction[0], coords[1] + sp->length * direction[1]);
        }
    }
}



void snake_free(Snake* p_snake) {
    sp_free_list(p_snake->sp_head);
}