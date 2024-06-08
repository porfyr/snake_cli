#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


#include <string.h>
#include <time.h>
// #include <errno.h>

// const short unsigned int INIT_LENGTH;
#define INIT_LENGTH 4
#define BUFFER_SIZE 255
#define PORT 12345

typedef struct {
    size_t height;
    size_t width;
    char **buffer;
    pthread_mutex_t *p_mutex;
    size_t food_coords[2];
} Map;

typedef struct {
    int direction[2];  // [0]: vertical (row) [1]: horizonal (collumn)
    size_t coords[2]; //
    size_t length;
} Snake_part;

typedef struct Node {
    Snake_part data;
    struct Node *next;
} Node;

typedef struct {
    Map*  p_map;
    Node *sp_head;   // Linked list queue
    int scores;
} Snake;


//// two thread functions 
void* map_runtime(void* vp_snake);
void* key_listener(void* vp_snake);

//// Map functions
Map* map_new(pthread_mutex_t *p_mutex);
void map_fill_with_border(Map *p_map);
void map_render(const Map *p_map);
void map_free(Map* p_map);

//// Snake manipulation functions
int    snake_add_part(Snake *p_snake, int dir_r, int dir_c);
Snake* snake_new(Map *p_map);
void   snake_map_set_food(const Snake *p_snake);
void   snake_move_step(Snake *p_snake);
void   snake_free(Snake *p_snake);

//// linked list functions in "snake_parts_ll.c"
Node*        sp_push(Node* head, Snake_part data);
Snake_part*  get_sp_by_index(Node* head, size_t i);
Node*        sp_shrink(Node* head);
size_t       sp_get_length(Node* head);
void         sp_print_list(Node* head);
void         sp_free_list(Node* head);

//// UDP logger
// char* format(const char* str);
int udp_log(char *msg, ...);


//// Render






