/* COMP2215 Task 7 */

#include <stdlib.h>
#include <util/delay.h>
#include <string.h>
#include "lcd/lcd.h"
#include "colours.h"

#ifndef MAIN_H
#define MAIN_H

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

#define KEY_BUFFER_SIZE 10

typedef struct { int x,y; } point;

typedef struct snake_segment {
    int x, y;
    struct snake_segment *prev;
    struct snake_segment *next;
} snake_segment;

typedef struct {
    snake_segment *head;
    snake_segment *tail;
} snake;

typedef enum {Up, Down, Left, Right, None} direction;

typedef struct {
    direction contents[KEY_BUFFER_SIZE];
    int head; 
    int tail; 
} direction_queue;

#define direction_queue_init {.head=0, .tail=0};

void key_press(direction);

#endif /* MAIN_H */