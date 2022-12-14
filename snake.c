#include "snake.h"
#include "TFT/TFTMaster.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "stdio.h"

#include "hardware/regs/rosc.h"
#include "hardware/regs/addressmap.h"
#include "hardware/structs/vreg_and_chip_reset.h"

#define WIDTH 240
#define HEIGHT 320

#define SNAKE_RAD 5



// return random number by ROSC between 0-2^N
// N should be in [0,32]
unsigned int ROrand(int N)
{
    static volatile uint32_t *randbit_reg = (uint32_t *)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);
    unsigned int random = 0;
    for (int i = 0; i < N; i++)
    {
        unsigned int random_bit = 0x1 & (*randbit_reg);
        random = random << 1 | random_bit;
    }
    return random;
}

//draw new head
void draw_snake(snake *mysnake) 
{
    tft_fillCircle(mysnake->x_coor[0], mysnake->y_coor[0], SNAKE_RAD, ILI9340_WHITE);

}

//remove tail
void remove_snake(snake *mysnake)
{
    tft_fillCircle(mysnake->x_coor[mysnake->snake_len - 1], mysnake->y_coor[mysnake->snake_len - 1], SNAKE_RAD, ILI9340_BLACK);
}

int is_over(snake *mysnake, int new_head_x, int new_head_y){

    //hitting the wall
    if (new_head_x <= 0 || new_head_x >= WIDTH|| new_head_y<=0||new_head_y>=HEIGHT){
        return 1;
    }

    //hitting itself

    for (int i = 1; i < mysnake->snake_len; i++)
    {
        float dist = sqrt(pow(mysnake->x_coor[0] - mysnake->x_coor[i], 2) + pow(mysnake->y_coor[0] - mysnake->y_coor[i], 2));
        if(dist<SNAKE_RAD){
            return 1;
        }
    }
    //over MAX_SNAKE_LEN

    if(mysnake->snake_len>= MAX_SNAKE_LEN){
        return 1;
    }


    return 0;

}

int hit_food(snake *mysnake){

    float food_dis = sqrt(pow(mysnake->x_coor[0] - mysnake->food_loc[0], 2) + pow(mysnake->y_coor[0] - mysnake->food_loc[1], 2));

    if (food_dis < SNAKE_RAD*2){
        return 1;
    }else{
        return 0;
    }
}

void spawn_food(snake *mysnake){
    mysnake->food_loc[0] = ROrand(7)+100; //100-228
    mysnake->food_loc[1] = ROrand(8)+50; //50-306
}

void draw_food(snake *mysnake){
    tft_fillCircle(mysnake->food_loc[0], mysnake->food_loc[1], SNAKE_RAD / 2, ILI9340_YELLOW);
}
void remove_food(snake *mysnake){

    int inside_body=0;
    //if inside snake's body
    for (int i = 0; i < mysnake->snake_len; i++)
    {
        float food_dis = sqrt(pow(mysnake->x_coor[i] - mysnake->food_loc[0], 2) + pow(mysnake->y_coor[i] - mysnake->food_loc[1], 2));
        if(food_dis<SNAKE_RAD){
            tft_fillCircle(mysnake->food_loc[0], mysnake->food_loc[1], SNAKE_RAD / 2, ILI9340_BLACK);
            tft_fillCircle(mysnake->x_coor[i], mysnake->y_coor[i], SNAKE_RAD, ILI9340_WHITE);
            inside_body=1;
        }
    }

    if(!inside_body){
        tft_fillCircle(mysnake->food_loc[0], mysnake->food_loc[1], SNAKE_RAD / 2, ILI9340_BLACK);
    }
    

}

void spawn_snake(snake *mysnake){
    mysnake->snake_len = 3;


    mysnake->direction = 0;

    mysnake->x_coor[0] = WIDTH / 2;
    mysnake->y_coor[0] = HEIGHT/ 2;

    mysnake->x_coor[1] = WIDTH / 2;
    mysnake->y_coor[1] = HEIGHT / 2 + 2 * SNAKE_RAD;

    mysnake->x_coor[2] = WIDTH / 2;
    mysnake->y_coor[2] = HEIGHT / 2 + 4 * SNAKE_RAD;


    spawn_food(mysnake);

}
//return 0 if game over, 1 if in progress

int update_snake(snake *mysnake){

    int new_head_x;
    int new_head_y;

    int temp_x_coor[mysnake->snake_len];
    int temp_y_coor[mysnake->snake_len];

    switch (mysnake->direction)
    {
    //up
    case 0:
        new_head_x=mysnake->x_coor[0];
        new_head_y=mysnake->y_coor[0]-SNAKE_RAD*2;
        printf("up");
        break;
    //right
    case 1:
        new_head_x = mysnake->x_coor[0] + SNAKE_RAD * 2;
        new_head_y = mysnake->y_coor[0];
        printf("right");
        break;
        //down
    case 2:
        new_head_x = mysnake->x_coor[0];
        new_head_y = mysnake->y_coor[0] + SNAKE_RAD * 2;
        printf("down");
        break;
    // left
    case 3:
        new_head_x = mysnake->x_coor[0] - SNAKE_RAD * 2;
        new_head_y=mysnake->y_coor[0];
        printf("left");
        break;
    //up-right
    case 4:
        new_head_x = mysnake->x_coor[0] + (SNAKE_RAD * 2)/sqrt(2);
        new_head_y = mysnake->y_coor[0] - (SNAKE_RAD * 2)/sqrt(2);
        printf("up-right");
        break;
    // down-right
    case 5:
        new_head_x = mysnake->x_coor[0] + (SNAKE_RAD * 2) / sqrt(2);
        new_head_y = mysnake->y_coor[0] + (SNAKE_RAD * 2) / sqrt(2);
        printf("down-right");
        break;
    // down-left
    case 6:
        new_head_x = mysnake->x_coor[0] - (SNAKE_RAD * 2) / sqrt(2);
        new_head_y = mysnake->y_coor[0] + (SNAKE_RAD * 2) / sqrt(2);
        printf("down-left");
        break;
    // up-left
    case 7:
        new_head_x = mysnake->x_coor[0] - (SNAKE_RAD * 2) / sqrt(2);
        new_head_y = mysnake->y_coor[0] - (SNAKE_RAD * 2) / sqrt(2);
        printf("up-left");
        break;

    default:
        break;
    }

    if(is_over(mysnake, new_head_x, new_head_y)){
        return 0;
    }


    if(hit_food(mysnake)){
        remove_food(mysnake);
        for (int i = 0; i < mysnake->snake_len; i++)
        {
            temp_x_coor[i + 1] = mysnake->x_coor[i];
            temp_y_coor[i + 1] = mysnake->y_coor[i];
        }
        mysnake->snake_len++;
        spawn_food(mysnake);
        draw_food(mysnake);
    }else{
        for (int i = 0; i < mysnake->snake_len - 1; i++)
        {
            temp_x_coor[i + 1] = mysnake->x_coor[i];
            temp_y_coor[i + 1] = mysnake->y_coor[i];
        }
        draw_food(mysnake);
    }

    temp_x_coor[0] = new_head_x;
    temp_y_coor[0] = new_head_y;

    for (int i = 0; i < mysnake->snake_len; i++)
    {
        mysnake->x_coor[i]=temp_x_coor[i];
        mysnake->y_coor[i] = temp_y_coor[i];
    }

    return 1;

}

void draw_boundary(void)
{
    tft_drawRect(0, 0, WIDTH, HEIGHT, ILI9340_WHITE);

}

void draw_gameover(snake *mysnake){
    //background
    tft_fillRect(0+50, 0+50, WIDTH-50*2, HEIGHT-50*2, ILI9340_WHITE);
    //GG!
    tft_setCursor(60, 70);
    tft_setTextColor(ILI9340_BLACK);
    tft_setTextSize(8);
    tft_writeString("GG!");
    //max len
    tft_setCursor(60, 150);
    tft_setTextColor(ILI9340_BLACK);
    tft_setTextSize(2);
    tft_writeString("Max Length: ");

    tft_setCursor(105, 200);
    tft_setTextColor(ILI9340_RED);
    tft_setTextSize(5);
    int max_snake_len = mysnake->snake_len;
    char str[12];
    sprintf(str, "%d", max_snake_len);
    tft_writeString(str);
}

