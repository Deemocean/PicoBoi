
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
#define w 1.0f   // whole note
#define h 0.5f   // half note
#define q 0.25f  // quarter note
#define e 0.125f // eighth note
#define s .0625f // sixteenth note

#define arraylen 36


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

float volume=0.99;

// float notes[32] = {82.41, 200, 82.41, 200, 98, 200, 82.41, 200, 73.42, 200, 65.41, 200, 61.74, 200, 82.41, 200, 82.41, 200, 98, 200, 82.41, 200, 73.42, 200, 65.41, 200, 73.42, 200, 65.41, 200, 61.74, 200};
// float duration[32] = {pause, 2, pause, 1, pause, 1, pause, 1, pause, 1, pause, 2, pause, 2, pause, 2, pause, 1, pause, 1, pause, 1, pause, 1, pause, 1, pause, 1, pause, 1, pause, 2};
float ttls[arraylen] = {C4, C4, G4, G4, A4, A4, G4, F4, F4, E4, E4, D4, D4, C4};
float ttlsduration[arraylen] = {
    q,
    q,
    q,
    q,
    q,
    q,
    h,
    q,
    q,
    q,
    q,
    q,
    q,
    h,
};

float notes[arraylen] = {C4, E4, G4, G4, A4, E4, G4, G4, A4, E4, G4, F4, E4, D4, C4, E4, G4, F4, E4, D4, E4, F4, E4, F4, D4, C4, E4, G4, F4, E4, D4, C4, F4, D4, E4, C4};
float duration[arraylen] = {q, q, h, q, q, q, h, q, q, q, h, q, q, w, q, q, h, q, q, h, q, q, q, q, w, q, q, h, q, q, h, q, q, q, q, w};
int count = 0;
int ind = 0;
int state = 1;
float dur = pause;
float vol = 0;
