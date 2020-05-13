#include "main.h"
#include "io.h"
#include "gameRender.h"

FATFS FatFs;
FIL File;
snake s;
point apple;
point exploding_pos;
point prev_tail_pos;
direction moving;
float speed;
direction_queue key_buffer;
int length;
highscore highscores[NO_OF_HIGHSCORES];

void display_menu();
void reset();
void step();
void redraw();
void save_score();
void read_highscores();
void add_key_to_buffer(direction k);
direction get_key_from_buffer();

void main(void)
{
 	/* 8MHz clock, no prescaling */
    CLKPR = _BV(CLKPCE);
    CLKPR = 0;

	init_inputs();
	init_outputs();
	
	sei();

	for (;;) {
		display.background = BLACK;
		clear_screen();
		display_menu();
		while(!centre_pressed());

		clear_screen();
		reset();
		redraw(s, length, highscores[0].score, apple, prev_tail_pos, exploding_pos);

		while (s.head != NULL) {
			step();
			redraw(s, length, highscores[0].score, apple, prev_tail_pos, exploding_pos);
			_delay_ms(BASE_SPEED / speed);
		}

		if (length > highscores[NO_OF_HIGHSCORES - 1].score) {
			save_score();
		}

		display_game_over(length, highscores);
		int i;
		const int delay = 10;
		for (i = 0; i < GAME_OVER_TIME; i += delay) {
			if (centre_pressed()) break;
			_delay_ms(delay);
		}
	}
}

highscore parse_line_to_hsc(char *line, int length) {
	highscore hs = (highscore) highscore_init;
	
	strncpy(hs.name, line, 3);
	const int n = length - 3;
	char tmp[n];
	
	strncpy(tmp, &line[3], n - 1);
	hs.score=atoi(tmp);
	
	return hs;
}

int f_lines(FIL *fp) {
	int i = 0;
	const int line_length = 32;
	char line[line_length];
	while(f_gets(line, sizeof line, fp)) {
		i++;
	}
	f_lseek(fp, 0);
	return i;
}

void read_highscores() {
	f_mount(&FatFs, "", 0);
	if (f_open(&File, "snake.txt", FA_READ | FA_OPEN_EXISTING) != FR_OK) {
		display_string("Error: Can't read file! \n");
		return;
	}
	const int line_length = 10;
	char line[line_length];
	const int scores_on_file = f_lines(&File);
	highscore hss[scores_on_file];
	int i = 0;
	while(f_gets(line, sizeof line, &File)) {
		hss[i] = parse_line_to_hsc(line, line_length);
		i++;
	}
	f_close(&File);
	
	int n;
	for (n = 0; n < NO_OF_HIGHSCORES; n++) {
		highscores[n] = (highscore) highscore_init;
	}
	for (n = 0; n < min(scores_on_file, NO_OF_HIGHSCORES); n++) {
		int tmp_index = 0;
		int i;
		for (i = 0; i < scores_on_file; i++) {
			if (hss[i].score >= hss[tmp_index].score) {
				tmp_index = i;
			}
		}
		highscores[n] = (highscore) {.score=hss[tmp_index].score};
		strcpy(highscores[n].name, hss[tmp_index].name); 
		hss[tmp_index].score = -1;
		

	}
	
}

void save_score() {
	f_mount(&FatFs, "", 0);
	if (f_open(&File, "snake.txt", FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {
			f_lseek(&File, f_size(&File));
			f_printf(&File, "AAA %d\n", length);
			f_close(&File);
	} else {
			display_string("Error: Can't write file! \n");
	}
}

void move_snake() {
	if (moving == None) {
		return;
	}
	prev_tail_pos = (point) {(*s.tail).x, (*s.tail).y };
	snake_segment *current = s.tail;
	while (current != NULL && current != s.head) {
		snake_segment *prev = (*current).prev;
		(*current).x = (*prev).x;
		(*current).y = (*prev).y;
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

/* Does not decrement length */
void remove_tail_segment() {
	snake_segment *new_tail = (*s.tail).prev;
	if (new_tail != NULL) {
		(*new_tail).next = NULL;
	} else {
		s.head = NULL;
	} 	
	prev_tail_pos = exploding_pos;
	exploding_pos.x = (*s.tail).x;
	exploding_pos.y = (*s.tail).y;
	free(s.tail);
	s.tail = new_tail;
}


void step() {

	if (moving == None) {
		if (s.head != NULL && s.tail != NULL) {
			remove_tail_segment();
		} 
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
	prev_tail_pos = (point) {-1,-1};
	exploding_pos = (point) {-1,-1};
}

void reset() {
	display.background = BLACK;
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
	read_highscores();
	
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
	if (moving == None) return;

	add_key_to_buffer(d);
}
