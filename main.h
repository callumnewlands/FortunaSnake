/* COMP2215 Task 7 */

#include <stdlib.h>
#include <util/delay.h>
#include "lcd/lcd.h"
#include "colours.h"
#include "io.h"

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

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

typedef enum {Up, Down, Left, Right} direction;
