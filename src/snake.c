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
    udp_log("\n\n\n______________________________\n\nЕкспериментальна ініціалізація");
    p_snake->p_map = p_map;
    int direction_r = 0, direction_c = 0;
    choose_init_direction(*p_snake, &direction_r, &direction_c);
    snake_add_part(p_snake, direction_r, direction_c);
    
    // udp_log("Вертаємо вказівник на змію (майже)");
    // udp_log("перша коорд. (щас довжина) змії %ld\n", p_snake->sp_head->data.length);
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
        p_snake->p_map->buffer[p_snake->sp_head->data.coords[0]+direction_r][p_snake->sp_head->data.coords[1]+direction_c] = '%';
        
        int cur_direction[2] = {p_snake->sp_head->data.direction[0], p_snake->sp_head->data.direction[1]};
        int ddr = cur_direction[0] + direction_r;
        int ddc = cur_direction[1] + direction_c;
        if (ddr == 2 || ddr == 0 || ddr == -2 || ddc == 2 || ddc == 0 || ddc == -2) {
            udp_log("поворот неумісний"); // %d %d : %d %d\n", cur_direction[0], cur_direction[1], direction_r, direction_c);
            return -1;
        } else {
            udp_log("Додаєм новий кусок");
            p_snake->p_map->buffer[p_snake->sp_head->data.coords[0] + direction_r*2][p_snake->sp_head->data.coords[1] + direction_c*2] = '@';
            p_snake->p_map->buffer[10][10] = '@';
            for (int i = 0; i < 10; ++i) {
                map_set_rand_point(p_snake->p_map);
            }
            // printf("Поворот умісний %d %d : %d %d\n", cur_direction[0], cur_direction[1], direction_r, direction_c);
            sp.coords[0] = p_snake->sp_head->data.coords[0] + direction_r;
            sp.coords[1] = p_snake->sp_head->data.coords[1] + direction_c;
            sp.length = 1;
            // p_snake->sp_head = sp_push(p_snake->sp_head, sp);
        }
    }
    p_snake->sp_head = sp_push(p_snake->sp_head, sp);
    p_snake->p_map->buffer[10][10] = '@';
    udp_log("%c", p_snake->p_map->buffer[10][10]);
    udp_log("(add) p_snake %d p_map %d", p_snake, p_snake->p_map);
    udp_log("Друк з (add)");
    sp_print_list(p_snake->sp_head);
    return 0;
}


void snake_move_step(Snake *p_snake) {
    udp_log("(move) p_map %d", p_snake->p_map);
    Node* current_sp = p_snake->sp_head;
    current_sp->data.coords[0] += current_sp->data.direction[0];
    current_sp->data.coords[1] += current_sp->data.direction[1];
    // if (p_snake->sp_head->next != NULL) {
    //     p_snake->sp_head->data.length ++;
    // }
    int is_first_part = 1;

    int i = 0;
    // Node* prev_sp;   // Якщо не працюватиме вичерпання куска змії
    while (current_sp != NULL) {
        udp_log("поворухнувся кусок %d", i++);
        // Якщо цей кусок перший
        if (is_first_part) {
            // Якщо цей кусок має наступні
            if (current_sp->next != NULL) {
                // current_sp->data.coords[0] += current_sp->data.direction[0];
                // current_sp->data.coords[1] += current_sp->data.direction[1];
                current_sp->data.length ++;
            }
            // Якщо цей кусок перший і останній
            else {
                // current_sp->data.coords[0] += current_sp->data.direction[0];
                // current_sp->data.coords[1] += current_sp->data.direction[1];
                ;
            }
            is_first_part = 0;
        }
        // Якщо цей кусок не перший і останній
        else if (current_sp->next == NULL) {
            // Якщо цей кусок має довжину 1 й не перший
            if (current_sp->data.length == 1) {
                udp_log("Вичерпалась частинка змії");

                free(current_sp);
                current_sp = NULL;
                // prev_sp->next = NULL;
                // break;
            }
            // /*
            // Якщо цей крок перший чи не останній
            else {
                udp_log("Зменшуєм частинку на 1");

                current_sp->data.length --;
            }
            // */
        }

        // Якщо цей кусок останній й пофіг чи перший (прибирання за хвостиком)
        if (current_sp->next == NULL) {
            size_t r = current_sp->data.coords[0];
            size_t c = current_sp->data.coords[1];
            int r_diff = current_sp->data.direction[0] * current_sp->data.length;
            int c_diff = current_sp->data.direction[1] * current_sp->data.length;
            p_snake->p_map->buffer[r-r_diff][c-c_diff] = ' ';
        }

        // рух до наступного куска
        if (current_sp != NULL) {
            // prev_sp = current_sp;    // Якщо не працюватиме вичерпання куска змії
            current_sp = current_sp->next;
        }
    }
}


void snake_sync_map(Snake snake) {
    udp_log("(sync) p_map %d", snake.p_map);
    udp_log("друк змії з (sync)");
    sp_print_list(snake.sp_head);
    Node* current_sp = snake.sp_head;
    int i = 0;
    while (current_sp != NULL) {
        udp_log("синхронізувалась частинка %d", i++);
        for (
                int i = 0, r = current_sp->data.coords[0], c = current_sp->data.coords[1];
                i < current_sp->data.length;
                ++i
            )
        {
            snake.p_map->buffer[r][c] = '#';
            r -= current_sp->data.direction[0];
            c -= current_sp->data.direction[1];
        }
        current_sp = current_sp->next;
    }
}


void snake_free(Snake* p_snake) {
    sp_free_list(p_snake->sp_head);
}