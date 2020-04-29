/* COMP2215 Task 7 */

#include <stdlib.h>
#include <util/delay.h>
#include "lcd/lcd.h"
#include "colours.h"
#include "io.h"

typedef struct snake_segment {
    int x, y;
    struct snake_segment *prev;
    struct snake_segment *next;
} snake_segment;

typedef struct {
    snake_segment head;
    snake_segment tail;
} snake;

void reset_board();
void redraw();