#include "main.h"

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))

#define PADDING 20
#define BOARD_WIDTH 20
#define BOARD_HEIGHT 15
#define SQUARE_SIZE ( min((LCDWIDTH -  2 * PADDING) / BOARD_HEIGHT, (LCDHEIGHT -  2 * PADDING) / BOARD_WIDTH) )

snake s;

void main(void)
{
 	/* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

	init_inputs();
	init_outputs();
	
	sei();

	clear_screen();
	display_string_xy("Press Center to Start", 60, 30);
	while(!centre_pressed());

	clear_screen();
	reset_board();
	redraw();

	for (;;)
	{
	}
}

void draw_segment(snake_segment *ss) {
	const int left = PADDING + (*ss).x * SQUARE_SIZE;
	const int top = PADDING + (*ss).y * SQUARE_SIZE;
	rectangle segment = {
		left, left + SQUARE_SIZE, 
		top,  top + SQUARE_SIZE};
	fill_rectangle(segment, GREEN);
}

void redraw() {
	snake_segment *next = &s.head;
	while (next != NULL) {
		draw_segment(next);
		next = (*next).next;
	}
}

void reset_snake() {
	const int y = (BOARD_HEIGHT / 2);
	snake_segment s0, s1, s2;
	s0 = (snake_segment) {.x = 4, .y = y, .prev=NULL, .next=&s1 };
	s1 = (snake_segment) {.x = 3, .y = y, .prev=&s0, .next=&s2 };
	s2 = (snake_segment) {.x = 2, .y = y, .prev=&s1, .next=NULL };

	s = (snake) {.head=s0, .tail=s2};
}

void reset_board() {
	rectangle clear = {PADDING, BOARD_WIDTH * SQUARE_SIZE, PADDING, BOARD_HEIGHT * SQUARE_SIZE};
	fill_rectangle(clear, DARK_GRAY);
	reset_snake();
}
