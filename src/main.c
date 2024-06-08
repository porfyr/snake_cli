#include <stdio.h>
#include <pthread.h>


#include "game_arch.h"



pthread_mutex_t mutex;

int main(int argc, char **argv) {

    pthread_mutex_t mutex;
    pthread_t t_map_runtime, t_key_listener;
    pthread_mutex_init(&mutex, NULL);

    Map *p_map = map_new(&mutex);
    Snake *p_snake = snake_new(p_map);

    pthread_create(&t_map_runtime, NULL, &map_runtime, p_snake);
    pthread_create(&t_key_listener , NULL, &key_listener, p_snake);

    pthread_join(t_map_runtime, NULL);
    pthread_join(t_key_listener, NULL);

    return 0;
}