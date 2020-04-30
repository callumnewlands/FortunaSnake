/* 
   |      |     | Signal on |                       |
   | Port | Pin | Schematic | Function              |
   |------+-----+-----------+-----------------------|
   | E    |   4 | ROTA      | Rotary Encoder A      |
   | E    |   5 | ROTB      | Rotary Encoder B      |
   | E    |   7 | SWC       | Switch wheel "Centre" |
   |------+-----+-----------+-----------------------|
   | C    |   2 | SWN       | Switch wheel "North"  |
   | C    |   3 | SWE       | Switch wheel "East"   |
   | C    |   4 | SWS       | Switch wheel "South"  |
   | C    |   5 | SWW       | Switch wheel "West"   |
   |------+-----+-----------+-----------------------|
*/

#include <stdio.h>
#include "lcd/lcd.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "stdbool.h"

#ifndef IO_H
#define IO_H

#define SWN     PC2
#define SWE     PC3
#define SWS     PC4
#define SWW     PC5
#define SWC     PE7

#define COMPASS_SWITCHES (_BV(SWW)|_BV(SWS)|_BV(SWE)|_BV(SWN))
#define ALL_SWITCHES (_BV(SWC) | COMPASS_SWITCHES)

#define REPEAT_START    60      /* after 600ms */
#define REPEAT_NEXT     10      /* every 100ms */

void init_inputs();
void init_outputs();
bool centre_pressed();
bool left_pressed();
bool right_pressed();
bool up_pressed();
bool down_pressed();

#endif /* IO_H */