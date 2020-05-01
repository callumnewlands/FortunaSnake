#include <stdlib.h>
#include "lcd/lcd.h"
#include "main.h"
#include "colours.h"
#include <string.h>
#include <stdio.h>

void display_menu();
void display_game_over(int);
void redraw(snake, int, point, point, point);