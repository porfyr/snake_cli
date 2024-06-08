#include <string.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "game_arch.h"


Map* map_new(pthread_mutex_t *p_mutex)
{
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // w.ws_col, w.ws_row-1
    size_t height = w.ws_row-1;
    size_t width = w.ws_col;

    Map *p_map = calloc(1, sizeof(*p_map));

    p_map->height = height;
    p_map->width = width;
    p_map->buffer = (char **)calloc(height, sizeof(char *));
    p_map->p_mutex = p_mutex;
    if (p_map->buffer == NULL)
        perror("Помилка виділення пам'яті під буфер");
    for (size_t i = 0; i < height; ++i) {
        p_map->buffer[i] = (char *)calloc(width, sizeof(char));
        if (p_map->buffer[i] == NULL)
            perror("Помилка виділення пам'яті під рядок буферу");
    }

    return p_map;
}


static void map_fill(Map *p_map, char chr)
{
    for (size_t r = 0; r < p_map->height; ++r) {
        // // Щось через наступний рядок при очищенні крашилось
        // memset(&p_map->buffer[r], chr, p_map->width * sizeof(char));
        for (size_t c = 0; c < p_map->width; ++c) {
            p_map->buffer[r][c] = chr;
        }
    }
}


void map_fill_with_border(Map *p_map)
{
    map_fill(p_map, ' ');
    for (size_t r = 0; r < p_map->height; ++r) {
        p_map->buffer[r][0] = '|';
        p_map->buffer[r][p_map->width-1] = '|';
    }
    for (size_t c = 0; c < p_map->width; ++c) {
        p_map->buffer[0][c] = '-';
        p_map->buffer[p_map->height-1][c] = '-';
    }
    p_map->buffer[0][0] = '+';
    p_map->buffer[0][p_map->width-1] = '+';
    p_map->buffer[p_map->height-1][0] = '+';
    p_map->buffer[p_map->height-1][p_map->width-1] = '+';
}


void map_render(const Map *p_map)
{
    for (size_t r = 0; r < p_map->height; ++r) {
        for (size_t c = 0; c < p_map->width; ++c) {
            printf("%c", p_map->buffer[r][c]);
        }
        printf("\n");
    }
}


void map_free(Map *p_map)
{
    for (size_t i = 0; i < p_map->height; ++i) {
        udp_log("Звільняєм %ld / %ld", i, p_map->height);
        free(p_map->buffer[i]);
    }
    free(p_map->buffer);
    free(p_map);
}