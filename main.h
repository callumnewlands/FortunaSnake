/* COMP2215 Task 7 */

#include <stdlib.h>
#include <util/delay.h>
#include "lcd/lcd.h"
#include "colours.h"

#ifndef MAIN_H
#define MAIN_H

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

#define PADDING 15 /* in pixels */
#define TOP_PADDING PADDING * 2
#define BOARD_WIDTH 21 /* in pixels */
#define BOARD_HEIGHT 15 /* in pixels */
#define SQUARE_SIZE ( min((LCDWIDTH -  2 * PADDING) / BOARD_HEIGHT, (LCDHEIGHT -  TOP_PADDING - PADDING) / BOARD_WIDTH) )
#define BASE_SPEED 200 /* Starting time between movements in ms */
#define BACK_COLOUR DARK_GREY
#define GAME_OVER_TIME 10000 /* Time after game over before the game resets to the title in ms */
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