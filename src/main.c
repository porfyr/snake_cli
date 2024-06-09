#include <stdio.h>
#include <pthread.h>


#include "game_arch.h"



int main(int argc, char **argv) {

    pthread_mutex_t controls_mutex;
    pthread_t t_map_runtime;
    pthread_mutex_init(&controls_mutex, NULL);

    Map *p_map = map_new();
    Snake *p_snake = snake_new(p_map, &controls_mutex);

    pthread_create(&t_map_runtime, NULL, &map_runtime, p_snake);

    pthread_mutex_destroy(&controls_mutex);
    pthread_join(t_map_runtime, NULL);

    return 0;
}