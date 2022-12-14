#include "menu.h"
#include "TFT/TFTMaster.h"

#define WIDTH 240
#define HEIGHT 320

struct menuSelect{
    int blockN;
};

void draw_menu(void){
    //SNAKE
    tft_drawRect(0, 0, WIDTH/2, HEIGHT/2, ILI9340_WHITE);
    tft_drawBitmap(0 + 5, 0 + 5, snake_logo, WIDTH / 2 - 5 * 2, HEIGHT / 2 - 5 * 2, ILI9340_WHITE);
    //SAT CLOCK
    tft_drawRect(WIDTH / 2, 0, WIDTH / 2, HEIGHT / 2, ILI9340_WHITE);
    tft_drawBitmap(WIDTH / 2 + 5, 0 + 5, sat_clk_logo, WIDTH / 2 - 5 * 2, HEIGHT / 2 - 5 * 2, ILI9340_WHITE);
    //SETTTNGS
    tft_drawRect(0, HEIGHT / 2, WIDTH / 2, HEIGHT / 2, ILI9340_WHITE);
    tft_drawBitmap(0 + 5, HEIGHT / 2 + 5, settings_logo, WIDTH / 2 - 5 * 2, HEIGHT / 2 - 5 * 2, ILI9340_WHITE);
    //MAP
    tft_drawRect(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2, ILI9340_WHITE);
    tft_drawBitmap(WIDTH / 2 + 5, HEIGHT / 2 + 5, map_logo, WIDTH / 2 - 5 * 2, HEIGHT / 2 - 5 * 2, ILI9340_WHITE);
}

void draw_setting(void){
    //baby
    tft_drawRect(0, 0, WIDTH / 2, HEIGHT / 2, ILI9340_WHITE);
    tft_drawBitmap(0 + 5, 0 + 5, baby_snake_logo, WIDTH / 2 - 5 * 2, HEIGHT / 2 - 5 * 2, ILI9340_GREEN);
    //ez
    tft_drawRect(WIDTH / 2, 0, WIDTH / 2, HEIGHT / 2, ILI9340_WHITE);
    tft_drawBitmap(WIDTH / 2 + 5, 0 + 5, ez_snake_logo, WIDTH / 2 - 5 * 2, HEIGHT / 2 - 5 * 2, ILI9340_GREEN);
    //normal
    tft_drawRect(0, HEIGHT / 2, WIDTH / 2, HEIGHT / 2, ILI9340_WHITE);
    tft_drawBitmap(5, HEIGHT / 2 + 5, normal_snake_logo, WIDTH / 2 - 5 * 2, HEIGHT / 2 - 5 * 2, ILI9340_GREEN);
    //hard
    tft_drawRect(WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2, ILI9340_WHITE);
    tft_drawBitmap(WIDTH / 2+5, HEIGHT / 2 + 5, hard_snake_logo, WIDTH / 2 - 5 * 2, HEIGHT / 2 - 5 * 2, ILI9340_GREEN);
}

void draw_map(void){
    tft_drawBitmap(0, 0, map, WIDTH, HEIGHT, ILI9340_WHITE);
    //tft_fillCircle(94,298,5,ILI9340_BLUE);
}
void removeAllSelector(){
    for(int i=1;i<5;i++){
        tft_drawRect(0 + i, 0 + i, WIDTH / 2 - i * 2, HEIGHT / 2 - i * 2, ILI9340_BLACK);
        tft_drawRect(WIDTH / 2 + i, 0 + i, WIDTH / 2 - i * 2, HEIGHT / 2 - i * 2, ILI9340_BLACK);
        tft_drawRect(0 + i, HEIGHT / 2 + i, WIDTH / 2 - i * 2, HEIGHT / 2 - i * 2, ILI9340_BLACK);
        tft_drawRect(WIDTH / 2 + i, HEIGHT / 2 + i, WIDTH / 2 - i * 2, HEIGHT / 2 - i * 2, ILI9340_BLACK);
    }

}

void draw_Selector(int blockN){

    switch (blockN)
    {
    case 0:
        for(int i=1;i<5;i++){
            tft_drawRect(0 + i, 0 + i, WIDTH / 2 - i * 2, HEIGHT / 2 - i * 2, ILI9340_BLUE);
        }
        break;
    case 1:
        for(int i=1;i<5;i++){
            tft_drawRect(WIDTH / 2 + i, 0 + i, WIDTH / 2 - i*2, HEIGHT / 2 - i*2, ILI9340_BLUE);
        }
        break;
    case 2:
        for(int i=1;i<5;i++){
            tft_drawRect(0 + i, HEIGHT / 2 + i, WIDTH / 2 - i*2, HEIGHT / 2 - i*2, ILI9340_BLUE);
        }
        break;
    case 3:
        for(int i=1;i<5;i++){
            tft_drawRect(WIDTH/2 + i, HEIGHT / 2 + i, WIDTH / 2 - i*2, HEIGHT / 2 - i*2, ILI9340_BLUE);
        }
        break;

    default:
        break;
    }
}

void remove_frame(void)
{
    tft_drawRect(0, 0, WIDTH, HEIGHT, ILI9340_BLACK);
}