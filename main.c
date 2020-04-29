#include "main.h"

#define PADDING 20
#define BOARD_WIDTH 22
#define BOARD_HEIGHT 15
#define SQUARE_SIZE ( min((LCDWIDTH -  2 * PADDING) / BOARD_HEIGHT, (LCDHEIGHT -  2 * PADDING) / BOARD_WIDTH) )
#define BASE_SPEED 300 /* Starting time between movements in ms */
#define BACK_COLOUR DARK_GREY

snake s;
point apple;
point prev_tail_pos;
direction moving;
float speed = 1.0f;

void reset();
void redraw();
void add_segment();
void step();

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
	reset();
	redraw();

	for (;;)
	{
   		PINB |= _BV(PINB7);   /* toggle LED */
		step();
		redraw();
		_delay_ms(speed * BASE_SPEED);
	}
}

/* Start from tail, and don't use this for the head*/
void move_segment(snake_segment *ss) {
	
	snake_segment *prev = (*ss).prev;
	(*ss).x = (*prev).x;
	(*ss).y = (*prev).y;
}

void move_snake() {
	prev_tail_pos = (point) {(*s.tail).x, (*s.tail).y };
	snake_segment *current = s.tail;
	while (current != NULL && current != s.head) {
		move_segment(current);
		current = (*current).prev;
	}
	switch (moving) {
		case Up: (*s.head).y--; break;	
		case Down: (*s.head).y++; break;
		case Left: (*s.head).x--; break;
		case Right: (*s.head).x++; break;
		default: break; 
	}
}

bool eating_apple() {
	return (*s.head).x == apple.x && (*s.head).y == apple.y;
}

point get_next_head_pos() {
	int x = (*s.head).x;
	int y = (*s.head).y;
	switch (moving) {
		case Up: y = (*s.head).y - 1; break;	
		case Down: y = (*s.head).y + 1; break;
		case Left: x = (*s.head).x - 1; break;
		case Right: x = (*s.head).x + 1; break;
		default: break; 
	}
	return (point) {x, y};
}

bool crashing() {
	//TODO
}

void move_apple() {
	// TODO
}

void step() {

	if (up_pressed() && moving != Down) {
		moving = Up;
	} 
	if (down_pressed() && moving != Up) {
		moving = Down;
	} 
	if (left_pressed() && moving != Right) {
		moving = Left;
	} 
	if (right_pressed()&& moving != Left) {
		moving = Right;
	}

	if (eating_apple()) {
		add_segment();
		move_apple();
	}

	if (crashing()) {
		// TODO
	}

	move_snake();
}

void add_segment() {
	snake_segment *ss = (snake_segment *) malloc(sizeof(snake_segment));
	(*ss).x = (*s.tail).x;
	(*ss).y = (*s.tail).y;
	(*ss).prev = s.tail;
	(*ss).next = NULL;
	(*s.tail).next = ss;
	s.tail = ss;
}

void draw_cell(int x, int y, int16_t col) {
	const int left = PADDING + x * SQUARE_SIZE;
	const int top = PADDING + y * SQUARE_SIZE;
	rectangle segment = {
		left, left + SQUARE_SIZE, 
		top,  top + SQUARE_SIZE};
	fill_rectangle(segment, col);
}

void draw_segment(snake_segment *ss, int16_t col) {
	draw_cell((*ss).x, (*ss).y, col);
}

void draw_snake() {

	draw_cell(prev_tail_pos.x, prev_tail_pos.y, BACK_COLOUR);

	snake_segment *next = s.head;
	draw_segment(next, DARK_GREEN);
	next = (*next).next;
	while (next != NULL) {
		draw_segment(next, GREEN);
		next = (*next).next;
	}
}

void draw_apple() {
	draw_cell(apple.x, apple.y, RED);
}

void redraw() {
	draw_apple();
	draw_snake();
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

void reset() {
	rectangle clear = {PADDING, BOARD_WIDTH * SQUARE_SIZE, PADDING, BOARD_HEIGHT * SQUARE_SIZE};
	fill_rectangle(clear, BACK_COLOUR);
	reset_snake();
	apple = (point) {max(BOARD_WIDTH - 5, (*s.head).x + 1), BOARD_HEIGHT / 2};
	moving = Right;
}
