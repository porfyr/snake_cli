#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "game_arch.h"


static int ch_reader(int ch_code, int* ch_i, char (*ch_arr)[10]);
static int getch();

void* map_runtime(void* vp_snake) {
    Snake *p_snake = (Snake*) vp_snake;
    pthread_mutex_t *p_mutex = p_snake->p_map->p_mutex;
    pthread_mutex_lock(p_mutex);
    map_fill_with_border(p_snake->p_map);
    snake_map_set_food(p_snake);
    pthread_mutex_unlock(p_mutex);

    for (size_t i = 0; i < 1000; ++i) {
        pthread_mutex_lock(p_mutex); // maybe it is useless
        snake_move_step(p_snake);
        pthread_mutex_unlock(p_mutex);
        map_render(p_snake->p_map);
        usleep(200000); // miliseconds
        printf("\x1b[d");
    }
    
    map_free(p_snake->p_map);
    snake_free(p_snake);

    return 0;
}

void* key_listener(void* vp_snake) {
    Snake *p_snake = (Snake*) vp_snake;
    pthread_mutex_t *p_mutex = p_snake->p_map->p_mutex;
    
    char ch_arr[10];
    int ch_i = 0;
    int direction_i = -1;
    int press_count = 0;
 
    while ((direction_i = ch_reader(getch(), &ch_i, &ch_arr)) != -1) {
        switch (direction_i) {
            case 0:
                press_count ++;
                snake_add_part(p_snake, -1, 0);
                break;
            case 1:
                press_count ++;
                snake_add_part(p_snake, 0, 1);
                break;
            case 2:
                press_count ++;
                snake_add_part(p_snake, 1, 0);
                break;
            case 3:
                press_count ++;
                snake_add_part(p_snake, 0, -1);
                break;
            case 4:
                map_free(p_snake->p_map);
                snake_free(p_snake);
                // exit(1);
                break;
            case 404:
                udp_log("помилка keypress %d pressed %d\n", direction_i, press_count);
                break;
        }
    }
}


static int ch_reader(int ch_code, int* ch_i, char (*ch_arr)[10]) { //char * ch_arr) {
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

static int getch() {
   struct termios oldtc;
   struct termios newtc;
   int ch;
   tcgetattr(STDIN_FILENO, &oldtc);
   newtc = oldtc;
   newtc.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(STDIN_FILENO, TCSANOW, &newtc);
   ch=getchar();
   tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
   return ch;
}






    // printf("\x1b[2J");
    // printf("\x1b[d");
