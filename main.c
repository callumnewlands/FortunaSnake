#include "main.h"
#include "io.h"

#define PADDING 15
#define TOP_PADDING PADDING * 2
#define BOARD_WIDTH 21
#define BOARD_HEIGHT 15
#define SQUARE_SIZE ( min((LCDWIDTH -  2 * PADDING) / BOARD_HEIGHT, (LCDHEIGHT -  TOP_PADDING - PADDING) / BOARD_WIDTH) )
#define BASE_SPEED 200 /* Starting time between movements in ms */
#define BACK_COLOUR DARK_GREY

snake s;
point apple;
point prev_tail_pos;
direction moving;
float speed;
direction_queue key_buffer;
int length;

// TODO Death Screen, Score Counter, Highscore storage

// char* direction_strings[] = {"Up", "Down", "Left", "Right", "None"}; 

void display_menu();
void reset();
void step();
void redraw();
void add_key_to_buffer(direction k);
direction get_key_from_buffer();

void main(void)
{
 	/* 8MHz clock, no prescaling (DS, p. 48) */
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

	init_inputs();
	init_outputs();
	
	sei();

	clear_screen();
	display_menu();
	while(!centre_pressed());

	clear_screen();
	reset();
	redraw();

	for (;;)
	{
		step();
		redraw();
		_delay_ms(BASE_SPEED / speed);
	}
}

void display_menu() {
	display_string("\n\n\n   _____             _        ");
	display_string("\n  / ____|           | |       ");
	display_string("\n | (___  _ __   __ _| | _____ ");
	display_string("\n  \\___ \\| '_ \\ / _` | |/ / _ \\");
	display_string("\n  ____) | | | | (_| |   <  __/");
	display_string("\n |_____/|_| |_|\\__,_|_|\\_\\___|\n\n");
	display_string("\n Press Centre To Start...");
}

/* Start from tail, and don't use this for the head*/
void move_segment(snake_segment *ss) {
	
	snake_segment *prev = (*ss).prev;
	(*ss).x = (*prev).x;
	(*ss).y = (*prev).y;
}

void move_snake() {
	if (moving == None) {
		return;
	}
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

bool is_part_of_snake(point p) {
	snake_segment *current = s.head;
	while (current != NULL) {
		if ((*current).x == p.x && (*current).y == p.y) {
			return true;
		}
		current = (*current).next;
	}
	return false;
}

bool crashing() {
	point new_pos = get_next_head_pos();
	return (moving == Up && new_pos.y < 0) ||
		(moving == Down && new_pos.y >= BOARD_HEIGHT) ||
		(moving == Right && new_pos.x >= BOARD_WIDTH) ||
		(moving == Left && new_pos.x < 0) ||
		is_part_of_snake(new_pos) || moving == None;
}

void move_apple() {
	point new_point;
	do {
		new_point = (point) {rand() % BOARD_WIDTH, rand() % BOARD_HEIGHT};
	} while (is_part_of_snake(new_point));
	apple = new_point;
}

void add_segment() {
	snake_segment *ss = (snake_segment *) malloc(sizeof(snake_segment));
	(*ss).x = (*s.tail).x;
	(*ss).y = (*s.tail).y;
	(*ss).prev = s.tail;
	(*ss).next = NULL;
	(*s.tail).next = ss;
	s.tail = ss;
	length++;
}

void step() {

	if (moving == None) {
		return;
	}

	direction change = get_key_from_buffer();
	if ((change == Up && moving != Down) || 
		(change == Down && moving != Up) ||
		(change == Left && moving != Right) ||
		(change == Right && moving != Left)) {
			moving = change;	
	}

	if (eating_apple()) {
		add_segment();
		move_apple();
		speed += .07f;
	}

	if (crashing()) {
		moving = None;
	}

	move_snake();
}

void draw_cell(int x, int y, int16_t col) {
	const int left = PADDING + x * SQUARE_SIZE;
	const int top = TOP_PADDING + y * SQUARE_SIZE;
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

void draw_ui() {
	fill_rectangle((rectangle){0, LCDWIDTH, 0, TOP_PADDING - 1}, BLACK);
	char text[42];
	sprintf(text, "length = %d               highscore = ?", length);
	display_string_xy(text, PADDING * 2, TOP_PADDING / 2 - 1);
}

void redraw() {
	draw_apple();
	draw_snake();
	draw_ui();
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
	length = 3;
}

void reset() {
	rectangle clear = {PADDING, PADDING + BOARD_WIDTH * SQUARE_SIZE, TOP_PADDING, TOP_PADDING + BOARD_HEIGHT * SQUARE_SIZE};
	fill_rectangle(clear, BACK_COLOUR);
	reset_snake();
	apple = (point) {max(BOARD_WIDTH - 5, (*s.head).x + 1), BOARD_HEIGHT / 2};
	moving = Right;
    speed = 1.0f;
	key_buffer = (direction_queue)direction_queue_init;
	int i;
	for (i = 0; i < KEY_BUFFER_SIZE; i++) {
		key_buffer.contents[i] = None;
	}
}

void add_key_to_buffer(direction k) {

	if (((key_buffer.tail + 1) % KEY_BUFFER_SIZE) == key_buffer.head) 
		key_buffer.head++;

	key_buffer.contents[key_buffer.tail++] = k;
	key_buffer.tail %= KEY_BUFFER_SIZE;
}

direction get_key_from_buffer() {
	if (key_buffer.head == key_buffer.tail) return None; // Empty Buffer

	const int ptr = key_buffer.head++;
	key_buffer.head %= KEY_BUFFER_SIZE;
	const direction d = key_buffer.contents[ptr];
	key_buffer.contents[ptr] = None;
	return d;
}

void key_press(direction d) {

	PINB |= _BV(PINB7);   /* toggle LED */

	if (moving == None) return;

	add_key_to_buffer(d);

	// if ((d == Up && moving != Down) || 
	// 	(d == Down && moving != Up) ||
	// 	(d == Left && moving != Right) ||
	// 	(d == Right && moving != Left)) {
	// 		moving = d;	
	// }
}
