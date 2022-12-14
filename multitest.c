
/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define CLKDIV 125.0f
#define pause 0.1f
#define w 1.0f     //whole note
#define h 0.5f     //half note
#define q 0.25f    //quarter note
#define e 0.125f   //eighth note
#define s .0625f   //sixteenth note

#define arraylen 36
#define volume .99f

#define C4 261.63f
#define C4S 277.18f
#define D4 293.66f
#define D4S 311.13f
#define E4 329.63f
#define F4 349.23f
#define F4S 369.99f
#define G4 392.0f
#define G4S 415.3f
#define A4 440.0f
#define A4S 466.16f
#define B4 493.88f

// float notes[32] = {82.41, 200, 82.41, 200, 98, 200, 82.41, 200, 73.42, 200, 65.41, 200, 61.74, 200, 82.41, 200, 82.41, 200, 98, 200, 82.41, 200, 73.42, 200, 65.41, 200, 73.42, 200, 65.41, 200, 61.74, 200};
// float duration[32] = {pause, 2, pause, 1, pause, 1, pause, 1, pause, 1, pause, 2, pause, 2, pause, 2, pause, 1, pause, 1, pause, 1, pause, 1, pause, 1, pause, 1, pause, 1, pause, 2};
float ttls[arraylen] = {C4, C4, G4, G4, A4, A4, G4, F4, F4, E4, E4, D4, D4, C4};
float ttlsduration[arraylen] = {q,q,q,q,q,q,h,q,q,q,q,q,q,h,};

float notes[arraylen] = {C4,E4,G4,G4,A4,E4,G4,G4,A4,E4,G4,F4,E4,D4,      C4,E4,G4,F4,E4,D4,E4,F4,E4,F4,D4,    C4,E4,G4,F4,E4,D4,C4,F4,D4,E4,C4};
float duration[arraylen] = {q,q,h,q,q,q,h,q,q,q,h,q,q,w,     q,q,h,q,q,h,q,q,q,q,w    ,q,q,h,q,q,h,q,q,q,q,w};
int count = 0;
int ind = 0;
int state = 1;   
float dur = pause;
float vol = 0;
bool music_cb(struct repeating_timer *t)
{
    if (count < (45000 * dur))
    {
        count++;
    }
    else
    {
        count = 0;
        if(state == 0){
            state =1;
            dur = pause; 
            vol = 0;
        } else{
            state = 0;
            dur = duration[ind];
            vol = 1000000 / (notes[ind] * (1/volume));
        }
        uint slice_num = pwm_gpio_to_slice_num(2);
        
        pwm_set_wrap(slice_num, (1000000 / notes[ind]));
        pwm_set_chan_level(slice_num, PWM_CHAN_A, vol);
        if( state == 0){
            if (ind == (arraylen - 1))
            {
            ind = 0;
            }
            else
            {
                ind++;
            }

        }
        
    }
    return true;
}

int main()
{

    stdio_init_all();
    // Tell GPIO 0 it is allocated to the PWM
    gpio_set_function(2, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
    uint slice_num = pwm_gpio_to_slice_num(2);

    pwm_set_clkdiv(slice_num, CLKDIV);

    pwm_set_wrap(slice_num, 1000000 / 261.6);

    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);

    pwm_set_enabled(slice_num, true);

    // Create a repeating timer that calls repeating_timer_callback.
    // If the delay is > 0 then this is the delay between the previous callback ending and the next starting.
    // If the delay is negative (see below) then the next call to the callback will be exactly x us after the
    // start of the call to the last callback
    struct repeating_timer timer;

    // Negative delay so means we will call repeating_timer_callback, and call it again
    // 25us (40kHz) later regardless of how long the callback took to execute
    add_repeating_timer_us(-25, repeating_timer_callback, NULL, &timer);
    while (1)
    {
    }
    return 0;
}