#include "gameRender.h"

void display_menu() {
	display_string("\n\n\n\n\n\n\n\n             _____             _        ");
	display_string("\n            / ____|           | |       ");
	display_string("\n           | (___  _ __   __ _| | _____ ");
	display_string("\n            \\___ \\| '_ \\ / _` | |/ / _ \\");
	display_string("\n            ____) | | | | (_| |   <  __/");
	display_string("\n           |_____/|_| |_|\\__,_|_|\\_\\___|\n\n");
	display_string("\n             Press Centre To Start...");
}

void display_game_over(int length) {
	display.background = RED;
	clear_screen();
	display_string("\n\n\n\n\n\n\n\n  _____                         ____                 ");
	display_string("\n / ____|                       / __ \\                ");
	display_string("\n| |  __  __ _ _ __ ___   ___  | |  | |_   _____ _ __ ");
	display_string("\n| | |_ |/ _` | '_ ` _ \\ / _ \\ | |  | \\ \\ / / _ \\ '__|");
	display_string("\n| |__| | (_| | | | | | |  __/ | |__| |\\ V /  __/ |   ");
	display_string("\n \\_____|\\__,_|_| |_| |_|\\___|  \\____/  \\_/ \\___|_|   ");
	display_string("\n=====================================================");

	char text[32];
	sprintf(text, "\n\n                      Length = %d", length);
	display_string(text);
	display_string("\n\n\n               Press Centre to Restart...");
}


void draw_cell(int x, int y, int16_t col) {
	if (x < 0 || y < 0) return;

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

void draw_snake(snake s, point prev_tail_pos, point exploding_pos) {

	draw_cell(prev_tail_pos.x, prev_tail_pos.y, BACK_COLOUR);
	draw_cell(exploding_pos.x, exploding_pos.y, YELLOW);

	if (s.head == NULL) return;

	snake_segment *next = s.head;
	draw_segment(next, DARK_GREEN);
	next = (*next).next;
	while (next != NULL) {
		draw_segment(next, GREEN);
		next = (*next).next;
	}
}

void draw_apple(point apple) {
	draw_cell(apple.x, apple.y, RED);
}

void draw_ui(int length) {
	fill_rectangle((rectangle){0, LCDWIDTH, 0, TOP_PADDING - 1}, BLACK);
	char text[42];
	sprintf(text, "length = %d               highscore = ?", length);
	display_string_xy(text, PADDING * 2, TOP_PADDING / 2 - 1);
}

void redraw(snake s, int length, point apple, point prev_tail_pos, point exploding_pos) {
	draw_apple(apple);
	draw_snake(s, prev_tail_pos, exploding_pos);
	draw_ui(length);
}