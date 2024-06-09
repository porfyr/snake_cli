#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "game_arch.h"


static int  ch_reader(int ch_code, int* ch_i, char (*ch_arr)[10]);
static int  getch();
static void game_over(Snake *p_snake);

void* map_runtime(void* vp_snake)
{
    void* time_to_quit = NULL;

    Snake *p_snake = (Snake*) vp_snake;
    
    pthread_t t_controls;
    pthread_create(&t_controls , NULL, &controls, p_snake);

    map_fill_with_border(p_snake->p_map);
    snake_map_set_food(p_snake);

    while (p_snake->still_playing) {
        p_snake->still_playing = snake_move_step(p_snake);
        map_render(p_snake->p_map);
        usleep(FRAME_DURATION); // miliseconds
        pthread_mutex_unlock(p_snake->controls_mutex);
        printf("\x1b[d");
    }
    
    pthread_cancel(t_controls);
    // pthread_join(t_controls, NULL);
    game_over(p_snake);

    map_free(p_snake->p_map);
    snake_free(p_snake);

    return time_to_quit;
}

void* controls(void* vp_snake)
{
    void* time_to_quit = NULL;

    Snake *p_snake = (Snake*) vp_snake;
    
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
                p_snake->still_playing = 0;
                p_snake->quited = 1;
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

static void game_over(Snake *p_snake)
{
    printf("\x1b[d");
    printf("\x1b[2J");
    if (p_snake->quited)
        printf("gg\n");
    else
        printf("game over\n");
}



// // spacial characters to eraze terminal or move cursor
// printf("\x1b[2J");
// printf("\x1b[d");
