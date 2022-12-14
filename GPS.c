#include "GPS.h"
#include "TFT/TFTMaster.h"
#include "math.h"

#define PI 3.1415926

// GNRMC,060155.000,A,4226.1410,N,07629.1449,W,0.02,46.89,061222,,,A*55
void buffer_shift(GPS *myGPS, uint8_t new_char)
{
    uint8_t temp_dog[DOG_SIZE];
    for (int i=0;i<DOG_SIZE-1;i++){
        temp_dog[i]=myGPS->watch_dog[i+1];
    }
    temp_dog[DOG_SIZE-1]=new_char;
    
    //full copy
    for (int i=0;i<DOG_SIZE;i++){
        myGPS->watch_dog[i]=temp_dog[i];
    }
}

int check_GNRMC(GPS *myGPS)
{
    uint8_t c0 = myGPS->watch_dog[0];
    uint8_t c1 = myGPS->watch_dog[1];
    uint8_t c2 = myGPS->watch_dog[2];
    uint8_t c3 = myGPS->watch_dog[3];
    uint8_t c4 = myGPS->watch_dog[4];

    if (c0 == 'G' && c1 == 'N' && c2 == 'R' && c3 == 'M' && c4 == 'C'){
        return 1;
    }else{
        return 0;
    }
}

void get_time(GPS *myGPS){


    for (int i = 0; i < 2; i++)
    {
        hh[i] = myGPS->watch_dog[i + GNRMC_LEN + 1];
        mm[i] = myGPS->watch_dog[i + GNRMC_LEN + 1 + 2];
        ss[i] = myGPS->watch_dog[i + GNRMC_LEN + 1 + 4];
    }
    // convert char to int
    sscanf(hh, "%d", &myGPS->EST_hh);
    sscanf(mm, "%d", &myGPS->EST_mm);
    sscanf(ss, "%d", &myGPS->EST_ss);

    // UTC to EST
    if (myGPS->EST_hh<5){
        myGPS->EST_hh = 24 + myGPS->EST_hh - 5;
    }else{
        myGPS->EST_hh = myGPS->EST_hh - 5;
    }

}

void get_fix_status(GPS *myGPS){
    for (int i = 0; i < 2; i++)
    {
        hh[i] = myGPS->watch_dog[i + GNRMC_LEN + 1];
        mm[i] = myGPS->watch_dog[i + GNRMC_LEN + 1 + 2];
        ss[i] = myGPS->watch_dog[i + GNRMC_LEN + 1 + 4];
    }

    if(fix=='A'){
        myGPS->FIX = 1;
    }else{
        myGPS->FIX =0;
    }
}

float dm2rad(float d, float m)
{
    return (d + m / 60.0) * PI / 180.0;
}

// GNRMC,060155.000,A,4(19)226.1410,N,0(31)762(34)9.1449,W,0.02,46.89,061222,,,A*55
void get_loc(GPS *myGPS)
{

    for (int i = 0; i < 2; i++){
        Lat_dd[i] = myGPS->watch_dog[GNRMC_LEN + UTC_LEN + 1+3];
    }

    for (int i = 0; i < 7; i++)
    {
        Lat_mm_mmmm[i] = myGPS->watch_dog[GNRMC_LEN + UTC_LEN + 1 + 3+2];
    }

    for (int i = 0; i < 3; i++)
    {
        Long_ddd[i] = myGPS->watch_dog[31];
    }

    for (int i = 0; i < 7; i++)
    {
        Long_mm_mmmm[i] = myGPS->watch_dog[34];
    }

    float lat_dd_temp;
    float lat_mm_mmmm_temp;

    float long_ddd_temp;
    float long_mm_mmmm_temp;

    sscanf(Lat_dd, "%f", &lat_dd_temp);
    sscanf(Lat_mm_mmmm, "%f", &lat_mm_mmmm_temp);

    sscanf(Long_ddd, "%f", &long_ddd_temp);
    sscanf(Long_mm_mmmm, "%f", &long_mm_mmmm_temp);

    // myGPS->Lat = lat_dd_temp + lat_mm_mmmm_temp/60.0;
    // myGPS->Long = long_ddd_temp + long_mm_mmmm_temp / 60.0;

    myGPS->Lat = dm2rad(lat_dd_temp, lat_mm_mmmm_temp);
    myGPS->Long = dm2rad(long_ddd_temp, long_mm_mmmm_temp);
}

void draw_me(GPS *myGPS){
    int x = (int)(10847.947929854385 * (myGPS->Long + 76.48373) + 117.59175555964036);
    int y = (int)(14868.50664436081 * (myGPS->Lat - 42.444493) + 158.64696589539855);

    tft_fillCircle(x, y, 5, ILI9340_BLUE);
}

void draw_time_fix(GPS *myGPS){
    //fix
    tft_setCursor(70, 70);
    tft_setTextColor(ILI9340_WHITE);
    tft_setTextSize(2);
    tft_writeString("FIX:");

    if(myGPS->FIX){
        tft_fillCircle(150, 75, 10, ILI9340_GREEN);
    }else{
        tft_fillCircle(150, 75, 10, ILI9340_RED);
    }





    tft_setCursor(45, 120);
    tft_setTextColor(ILI9340_WHITE);
    tft_setTextSize(2);
    tft_writeString("Time is (EST): ");

    tft_setCursor(60, 200);
    tft_setTextColor2(ILI9340_WHITE, ILI9340_BLACK);
    tft_setTextSize(3);

    uint8_t time_str[8];

    sprintf(time_str, "%d:%d:%d ", myGPS->EST_hh, myGPS->EST_mm, myGPS->EST_ss);
    tft_writeString(time_str);
}
