#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "game_arch.h"


static int ch_reader(int ch_code, int* ch_i, char (*ch_arr)[10]);
static int getch();
static int game_over();

void* map_runtime(void* vp_snake)
{
    void* time_to_quit = NULL;

    Snake *p_snake = (Snake*) vp_snake;
    
    pthread_mutex_t *p_map_mutex = p_snake->p_map->p_mutex;
    pthread_mutex_lock(p_map_mutex);
    map_fill_with_border(p_snake->p_map);
    snake_map_set_food(p_snake);
    pthread_mutex_unlock(p_map_mutex);

    udp_log("map inited");

    for (int continue_loop = 1; continue_loop; ) {
        pthread_mutex_lock(p_map_mutex); // maybe it is useless
        continue_loop = snake_move_step(p_snake);
        pthread_mutex_unlock(p_map_mutex);
        map_render(p_snake->p_map);
        usleep(FRAME_DURATION); // miliseconds
        pthread_mutex_unlock(p_snake->controls_mutex);
        printf("\x1b[d");
    }
    
    game_over(p_snake);

    map_free(p_snake->p_map);
    snake_free(p_snake);

    udp_log("Все позвільняв");

    return 0;
}

void* controls(void* vp_snake)
{
    void* time_to_quit = NULL;

    Snake *p_snake = (Snake*) vp_snake;
    pthread_mutex_t *p_map_mutex = p_snake->p_map->p_mutex;
    
    char ch_arr[10];
    int ch_i = 0;
    int direction_i = -1;
    int press_count = 0;
 
    while ((direction_i = ch_reader(getch(), &ch_i, &ch_arr)) != -1) {
        switch (direction_i) {
            case 0:     // Up
                press_count ++;
                snake_add_part(p_snake, -1, 0);
                break;
            case 1:     // Right
                press_count ++;
                snake_add_part(p_snake, 0, 1);
                break;
            case 2:     // Down
                press_count ++;
                snake_add_part(p_snake, 1, 0);
                break;
            case 3:     // Left
                press_count ++;
                snake_add_part(p_snake, 0, -1);
                break;
            case 4:     // got 'q', must quit
                return time_to_quit;
                break;
            case 404:
                udp_log("помилка keypress %d pressed %d\n", direction_i, press_count);
                break;
        }
    }
    return time_to_quit;
}


static int ch_reader(int ch_code, int* ch_i, char (*ch_arr)[10]) //char * ch_arr)   // if need longer commands
{
    switch (*ch_i) {
        case 0:
            switch (ch_code) {
                case 27:
                    *ch_i = 1;
                    break;
                case 113:   // 'q'
                    return 4;
                default:
                    return 40401;
            }
            break;
        case 1:
            if (*ch_i == 1 && ch_code == 91) {
                *ch_i = 2;
            } else {
                *ch_i = 0;
                return 40402;
            }
            break;
        case 2:
            *ch_i = 0;
            switch ((char)ch_code) {
                case 'A':
                    return 0;
                case 'C':
                    return 1;
                case 'B':
                    return 2;
                case 'D':
                    return 3;
                default:
                    return 40405;
            }
    }
    return 40400;
}

static int getch()
{
   struct termios oldtc;
   struct termios newtc;
   int ch;
   tcgetattr(STDIN_FILENO, &oldtc);
   newtc = oldtc;
   newtc.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(STDIN_FILENO, TCSANOW, &newtc);
   ch = getchar();
   tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
   return ch;
}

static int game_over(Snake *p_snake)
{
    printf("\x1b[d");
    printf("\x1b[2J");
    printf("game over");
}



// // spacial characters to eraze terminal or move cursor
// printf("\x1b[2J");
// printf("\x1b[d");
