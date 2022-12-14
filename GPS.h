
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define DOG_SIZE 50
#define UTC_LEN 10
#define GNRMC_LEN 5

static uint8_t hh[2];
static uint8_t mm[2];
static uint8_t ss[2];

static uint8_t fix;

static uint8_t Lat_dd[2];
static uint8_t Lat_mm_mmmm[7];

static uint8_t Long_ddd[3];
static uint8_t Long_mm_mmmm[7];

typedef struct GPS
{
    uint8_t watch_dog[DOG_SIZE];
    uint8_t UTC_time[UTC_LEN];

    int FIX;

    int EST_hh;
    int EST_mm;
    int EST_ss;

    float Lat;
    float Long;



} GPS;

void buffer_shift(GPS *myGPS, uint8_t new_char);
int check_GNRMC(GPS *myGPS);
void get_time(GPS *myGPS);
void get_fix_status(GPS *myGPS);
void draw_time_fix(GPS *myGPS);
void draw_me(GPS *myGPS);
void get_loc(GPS *myGPS);