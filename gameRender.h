#include <stdlib.h>
#include "lcd/lcd.h"
#include "main.h"
#include "colours.h"
#include <string.h>
#include <stdio.h>

void display_menu();
void display_game_over(int, highscore*);
void redraw(snake, int, int, point, point, point);