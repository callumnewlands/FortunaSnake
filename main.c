#include "main.h"

#define min(X, Y)  ((X) < (Y) ? (X) : (Y))

#define PADDING 20
#define BOARD_WIDTH 20
#define BOARD_HEIGHT 15
#define SQUARE_SIZE ( min((LCDWIDTH -  2 * PADDING) / BOARD_HEIGHT, (LCDHEIGHT -  2 * PADDING) / BOARD_WIDTH) )

// move segment to position of prev?

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

void add_segment() {
	snake_segment *ss = (snake_segment *) malloc(sizeof(snake_segment));
	(*ss).x = (*s.tail).x;
	(*ss).y = (*s.tail).y;
	(*ss).prev = s.tail;
	(*ss).next = NULL;
	(*s.tail).next = ss;
}

void draw_segment(snake_segment *ss, bool is_head) {
	const int left = PADDING + (*ss).x * SQUARE_SIZE;
	const int top = PADDING + (*ss).y * SQUARE_SIZE;
	rectangle segment = {
		left, left + SQUARE_SIZE, 
		top,  top + SQUARE_SIZE};
	fill_rectangle(segment, is_head ? DARK_GREEN : GREEN);
}

void redraw() {
	snake_segment *next = s.head;
	draw_segment(next, true);
	next = (*next).next;
	while (next != NULL) {
		draw_segment(next, false);
		next = (*next).next;
	}
}

void free_all_segments() {
	snake_segment *current = s.head;
	while (current != NULL) {
		snake_segment *next = (*current).next;
		free(current);
		current = next;
	}
}

void reset_snake() {
	free_all_segments();

	const int y = (BOARD_HEIGHT / 2);
	snake_segment *s0 = (snake_segment *) malloc(sizeof(snake_segment));
	snake_segment *s1 = (snake_segment *) malloc(sizeof(snake_segment));
	snake_segment *s2 = (snake_segment *) malloc(sizeof(snake_segment));
	(*s0).x = 4;
	(*s0).y = y;
	(*s0).prev = NULL;
	(*s0).next = s1;

	(*s1).x = 3;
	(*s1).y = y;
	(*s1).prev = s0;
	(*s1).next = s2;

	(*s2).x = 2;
	(*s2).y = y;
	(*s2).prev = s1;
	(*s2).next = NULL;

	s = (snake) {.head=s0, .tail=s2};
}

void reset_board() {
	rectangle clear = {PADDING, BOARD_WIDTH * SQUARE_SIZE, PADDING, BOARD_HEIGHT * SQUARE_SIZE};
	fill_rectangle(clear, DARK_GRAY);
	reset_snake();
}
