// #include <stdlib.h>
// #include <string.h>
// #include <time.h>
// #include <errno.h>

#include "game_arch.h"


// static
static void sp_copy(Snake_part source, Snake_part* target) {
    target->direction[0] = source.direction[0];
    target->direction[1] = source.direction[1];
    target->coords[0] = source.coords[0];
    target->coords[1] = source.coords[1];
    target->length = source.length;
}


// Function to create a new node
Node* create_node(Snake_part data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        udp_log("Memory allocation failed\n");
        exit(1);
    }
    sp_copy(data, &new_node->data);
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}


// Function to insert a node at the beginning of the linked list
Node* sp_push(Node* head, Snake_part data) {
    Node* newNode = create_node(data);
    newNode->next = head;
    udp_log("sp_push повертає новий вузол, direction (r c): %d, %d", newNode->data.direction[0], newNode->data.direction[1]);
    // printf("sp_push повертає новий вузол %d, %d\n", newNode->data.direction[0], newNode->data.direction[1]);
    return newNode;
}


Snake_part* get_sp_by_index(Node *head, size_t i) {
    Node *current = head;
    size_t curr_i = 0;
    while (current != NULL) {
        Node *temp = current;
        if (curr_i == i)
            return &current->data;
        ++ curr_i;
    }
    return NULL;
}


// paste output into head carriers
Node* sp_shrink(Node* head) {
    Node *new_head = head->next;
    free(head);
    return new_head;
}


// Отримати кількість кусків змії
size_t sp_get_length(Node *head) {
    size_t length = 0;
    Node *current = head;
    while (current != NULL) {
        ++ length;
        current = current->next;
    }
    return length;
}


static void sp_print(Snake_part sp, int i) {
    udp_log("№%d dir r, c: %d %d \t coords r: %ld \tc: %ld \tlen: %ld",
            i, sp.direction[0], sp.direction[1], sp.coords[0], sp.coords[1], sp.length);
}

void sp_print_list(Node* head) {
    Node *current = head;
    // udp_log("друк інфи про куски змії");
    int i = 0;
    while (current != NULL) {
        sp_print(current->data, i);
        ++i;
        current = current->next;
    }
}


void sp_free_list(Node* head) {
    Node *current = head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
}
