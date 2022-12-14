#define MAX_SNAKE_LEN 100
#include "stdio.h"



typedef struct snake
{
    int x_coor[MAX_SNAKE_LEN];
    int y_coor[MAX_SNAKE_LEN];
    int direction;
    int snake_len;

    int food_loc[2];

} snake;


void draw_boundary(void);
void spawn_snake(snake *mysnake);
void draw_snake(snake *mysnake);
void remove_snake(snake *mysnake);
int update_snake(snake *mysnake);

void spawn_food(snake *mysnake);
void remove_food(snake *mysnake);

void draw_gameover(snake *mysnake);
