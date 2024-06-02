#include <stdio.h>
// #include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <sys/ioctl.h>

#include "game_arch.h"



// void* map_runtime(Snake* p_snake);
// void* key_listener(Snake* p_snake);
void* map_runtime(void* vp_snake);
void* key_listener(void* vp_snake);
int  getch();
int  ch_reader(int c, int* ch_i, char (*ch_arr)[10]);


pthread_mutex_t mutex;

int main(int argc, char **argv) {

    Map *p_map = map_new();
    Snake *p_snake = snake_new(p_map);

    pthread_t t_map_runtime, t_key_listener;
    pthread_mutex_init(&mutex, NULL);
    pthread_create(&t_map_runtime, NULL, &map_runtime, p_snake);
    pthread_create(&t_key_listener , NULL, &key_listener, p_snake);

    // int proc_id = fork();
    // if (proc_id == 0) {
    //     // udp_log("процес %d p_map %d", proc_id, p_snake->p_map);
    //     key_listener(p_snake);
    // } else {
    //     // udp_log("процес %d p_map %d", proc_id, p_snake->p_map);
    //     map_runtime(p_snake);
    // }
    pthread_join(t_map_runtime, NULL);
    pthread_join(t_key_listener, NULL);
    return 0;
}

void* map_runtime(void* vp_snake) {
    Snake *p_snake = (Snake*) vp_snake;
    udp_log("p_map %d", p_snake->p_map);
    pthread_mutex_lock(&mutex);
    map_fill_with_border(p_snake->p_map);
    snake_sync_map(*p_snake);
    pthread_mutex_unlock(&mutex);

    for (size_t i = 0; i < 1000; ++i) {
        map_set_rand_point(p_snake->p_map);
        snake_move_step(p_snake);
        pthread_mutex_lock(&mutex);
        snake_sync_map(*p_snake);
        pthread_mutex_unlock(&mutex);
        map_render(p_snake->p_map);
        usleep(500000);
        printf("\x1b[d");
    }
    
    map_free(p_snake->p_map);
    snake_free(p_snake);
}

void* key_listener(void* vp_snake) {
    Snake *p_snake = (Snake*) vp_snake;
    udp_log("p_snake %d", p_snake);
    
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
        // sp_print_list(p_snake->sp_head);
    }
}


int ch_reader(int ch_code, int* ch_i, char (*ch_arr)[10]) { //char * ch_arr) {
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
