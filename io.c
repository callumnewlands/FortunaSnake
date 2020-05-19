/* Author: Callum Newlands
 * License: This work is licenced under GNU GPLv3.0 - see the attached LICENSE.txt notice
 * Adapted from ruota.c which has the following copyright notice:
 *    |
 *    |  Copyright Peter Dannegger (danni@specs.de)
 *    |  http://www.mikrocontroller.net/articles/Entprellung 
 *    |  http://www.mikrocontroller.net/articles/Drehgeber
 *    |
 *    |  Slightly adapted by Klaus-Peter Zauner for FortunaOS, March 2015

*/

#include "io.h"
#include "main.h"

volatile uint8_t switch_state;   /* debounced and inverted key state: bit = 1: key pressed */
volatile uint8_t switch_press;   /* key press detect */
volatile uint8_t switch_rpt;     /* key long press and repeat */

void scan_switches();

void init_inputs(){
    DDRE &= ~_BV(SWC); // set as input
    PORTE |= _BV(SWC); // enable pullup

    DDRC &= ~COMPASS_SWITCHES; // set as inputs
    PORTC |= COMPASS_SWITCHES; // enable pull-ups

    /* Timer 0 for switch scan interrupt: */

    TCCR0A = _BV(WGM01);  /* CTC Mode, DS Table 14-7 */
    TCCR0B = _BV(CS01)
           | _BV(CS00);   /* Prescaler: F_CPU / 64, DS Table 14-8 */

    OCR0A = (uint8_t)(F_CPU / (64.0 * 1000) - 1); // 1000Hz = 1ms

    TIMSK0 |= _BV(OCIE0A);  /* Enable timer interrupt, DS 14.8.6  */
}

void init_outputs() {

    init_lcd();

    DDRB  |=  _BV(PB7);   /* LED pin out */
    PORTB &= ~_BV(PB7);   /* LED off */
}

 ISR( TIMER0_COMPA_vect ) {
    scan_switches();
    if (up_pressed()) {
        key_press(Up);
    }
    if (down_pressed()) {
        key_press(Down);
    }
    if (left_pressed()) {
        key_press(Left);
    }
    if (right_pressed()) {
        key_press(Right);
    }
 }


void scan_switches() {
    static uint8_t ct0, ct1, rpt;
    uint8_t i;

    cli();
    /* 
        Overlay port E for central button of switch wheel
    */ 
    i = switch_state ^ ~( (PINC | _BV(SWC))	\
                     & (PINE|~_BV(SWC)));  /* switch has changed */
    ct0 = ~( ct0 & i );                      /* reset or count ct0 */
    ct1 = ct0 ^ (ct1 & i);                   /* reset or count ct1 */
    i &= ct0 & ct1;                          /* count until roll over ? */
    switch_state ^= i;                       /* then toggle debounced state */
    switch_press |= switch_state & i;        /* 0->1: key press detect */

    if( (switch_state & ALL_SWITCHES) == 0 ) {    /* check repeat function */
        rpt = REPEAT_START;                 /* start delay */
    }
    
    if( --rpt == 0 ){
        rpt = REPEAT_NEXT;                   /* repeat delay */
        switch_rpt |= switch_state & ALL_SWITCHES;
    }

    sei();
}


/*
   Check if a key has been pressed
   Each pressed key is reported only once.
*/
uint8_t get_switch_press( uint8_t switch_mask ) {
  cli();                         /* read and clear atomic! */
  switch_mask &= switch_press;         /* read key(s) */
  switch_press ^= switch_mask;         /* clear key(s) */
  sei();
  return switch_mask;
}

bool centre_pressed() {
    return get_switch_press(_BV(SWC));
}

bool up_pressed() {
    return get_switch_press(_BV(SWN));
}

bool down_pressed() {
    return get_switch_press(_BV(SWS));
}

bool left_pressed() {
    return get_switch_press(_BV(SWW));
}

bool right_pressed() {
    return get_switch_press(_BV(SWE));
}