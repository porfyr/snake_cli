#include <stdio.h>
// #include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "game_arch.h"


void key_listener(Snake* p_snake, int proc_id);
int  getch();
int  ch_reader(int c, int* ch_i);
void map_runtime(Map* p_map, Snake* p_snake, int proc_id);


void main(int argc, char **argv) {
    // struct winsize w;
    // ioctl(STDOUT_FILENO , TIOCGWINSZ, &w);

    // printf("\nlines: %d, columns: %d\n", w.ws_row, w.ws_col);

    Map *p_map = map_new();
    Snake *p_snake = snake_new(p_map);

    int proc_id = fork();
    if (proc_id == 0) {
        key_listener(p_snake, proc_id);
    } else {
        map_runtime(p_map, p_snake, proc_id);
    }
    return 0;
}

void map_runtime(Map* p_map, Snake* p_snake, int proc_id) {
    // printf("у процесі %d\n", proc_id);
    map_fill_with_border(p_map);
    snake_sync_map(*p_snake);

    for (size_t i = 0; i < 1000; ++i) {
        map_set_rand_point(p_map);
        snake_move_step(p_snake);
        snake_sync_map(*p_snake);
        map_render_update(p_map);
        sleep(1);
        printf("\x1b[d");
    }
    
    map_free(p_map);
    snake_free(p_snake);
}

void key_listener(Snake* p_snake, int proc_id) {
    // printf("у процесі %d\n", proc_id);
    
    int ch_i = 0;
    int direction_i = -1;
    int press_count = 0;
    char msg[BUFFER_SIZE]; // костильовий виклик udp_log
 
    while ((direction_i = ch_reader(getch(), &ch_i)) != -1) {
        switch (direction_i) {
            case 0:
                press_count ++;
                sprintf(msg, "Up Arrow %d pressed %d\n", direction_i, press_count);
                udp_log(msg);
                // printf("Up Arrow %d pressed %d\n", direction_i, press_count);
                snake_add_part(p_snake, 1, 0);
                break;
            case 1:
                press_count ++;
                sprintf(msg, "Right Arrow %d pressed %d\n", direction_i, press_count);
                udp_log(msg);
                // printf("Right Arrow %d pressed %d\n", direction_i, press_count);
                snake_add_part(p_snake, 0, 1);
                break;
            case 2:
                press_count ++;
                sprintf(msg, "Down Arrow %d pressed %d\n", direction_i, press_count);
                udp_log(msg);
                // printf("Down Arrow %d pressed %d\n", direction_i, press_count);
                snake_add_part(p_snake, -1, 0);
                break;
            case 3:
                press_count ++;
                sprintf(msg, "Left Arrow %d pressed %d\n", direction_i, press_count);
                udp_log(msg);
                // printf("Left Arrow %d pressed %d\n", direction_i, press_count);
                snake_add_part(p_snake, 0, -1);
                break;
            case 404:
                sprintf(msg, "помилка keypress %d pressed %d\n", direction_i, press_count);
                udp_log(msg);
                // printf("помилка keypress %d pressed %d\n", direction_i, press_count);
                break;
        }
    }
}


int ch_reader(int ch_code, int* ch_i) {
    switch (*ch_i) {
        case 0:
            switch (ch_code) {
                case 27:
                    *ch_i = 1;
                    break;
                default:
                    return 40401;
            }
            break;
            // if (ch_code == 27) {
            //     *ch_i = 1;
            // } 
            // else {
            //     return 40401;
            // }
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

int getch() {
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
