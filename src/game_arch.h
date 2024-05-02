#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

// #include <string.h>
#include <time.h>
// #include <errno.h>

// const short unsigned int INIT_LENGTH;
#define INIT_LENGTH 4
#define PORT 12345

typedef struct {
    size_t height;
    size_t width;
    char **buffer;
} Map;

typedef struct {
    int direction[2];  // [0]: vertical (row) [1]: horizonal (collumn)
    size_t coords[2]; //
    size_t length;
} Snake_part;

typedef struct Node {
    Snake_part data;
    struct Node* next;
} Node;

typedef struct {
    Map *p_map;
    struct Node *sp_head;   // Linked list queue
} Snake;


//// Map functions
Map* map_new();
void map_fill(Map *p_map, char chr);
void map_fill_with_border(Map* p_map);
void map_set_rand_point(Map* p_map);
void map_render_update(Map* p_map);
void map_free(Map* p_map);

//// Snake manipulation functions
int    snake_add_part(Snake* p_snake, int dir_r, int dir_c);
Snake* snake_new(Map* p_map);
void   snake_move_step(Snake *p_snake);
void   snake_sync_map(Snake snake);
void   snake_free(Snake* p_snake);

//// linked list functions in "snake_parts_ll.c"
Node*        sp_push(Node* head, Snake_part data);
Snake_part*  get_sp_by_index(Node* head, size_t i);
Node*        sp_shrink(Node* head);
size_t       sp_get_length(Node* head);
void         sp_print_list(Node* head);
void         sp_free_list(Node* head);

//// UDP logger
int udp_log(const char* msg);


//// Render






