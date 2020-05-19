/* Author: Callum Newlands
 * Licence: This work is licenced under GNU GPLv3.0 - see the attached LICENCE.txt notice
 */

#include <stdlib.h>
#include "lcd/lcd.h"
#include "main.h"
#include "colours.h"
#include <string.h>
#include <stdio.h>

void display_menu();
void display_game_over(int, highscore*);
void redraw(snake, int, int, point, point, point);