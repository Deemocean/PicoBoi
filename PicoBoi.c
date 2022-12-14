#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "hardware/pio.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"
// Include custom libraries
#include "pt_cornell_rp2040_v1.h"
#include "TFT/TFTMaster.h"
//#include "Adafruit_GPS-1.7.1/src/Adafruit_GPS.h"
#include "menu.h"
#include "snake.h"
#include "GPS.h"
#include "music.h"

#define BUTTON_X_PIN 8
#define BUTTON_Y_PIN 9

#define BUTTON_A_PIN  10
#define BUTTON_B_PIN  11
#define BUTTON_C_PIN  12
#define BUTTON_D_PIN  13

//#define UART_ID uart1
#define BAUD_RATE 9600
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

//#define UART_TX_PIN 4
#define UART_RX_PIN 21
// ms
#define DEBOUNCE_CD 200
#define PRESS_SOUND_CD 100

int SNAKE_FRAME_CD = 200;

#define CLOCK_FRAME_CD 500
#define MAP_FRAME_CD 500

int CD_X = 0;
int CD_Y = 0;
int CD_A = 0;
int CD_B = 0;
int CD_C = 0;
int CD_D = 0;

int CD_SNAKE_FRAME=0;
int CD_CLOCK_FRAME=0;
int CD_PRESS_SOUND=0;
int CD_MAP_FRAME=0;
//-1:menu 0:app0 -2:app0 gameover 1:app1 2:app2 3:app3
int STATE =-1;
//init semphares

int menu_init = 1;
//SNAKE
int App_0_init = 1;
int App_0_over_init = 1;
//CLOCK
int App_1_init = 1;
//settings
int App_2_init = 1;
//map
int App_3_init = 1;

int currentSelectorN = 0;
int drawSelectorN = 0;

int currentSettingN = 0;
int drawSettingN = 0;

static snake mysnake;
static GPS myGPS;




// RX interrupt handler
void on_uart_rx()
{
    //int i_char=0;
    //clean watch_dog
    while (uart_is_readable(uart1))
    {
        
        uint8_t new_char = uart_getc(uart1);

        buffer_shift(&myGPS, new_char);

        if (check_GNRMC(&myGPS))
        {
            
           get_time(&myGPS);
           get_fix_status(&myGPS);
           if(myGPS.FIX){
               get_loc(&myGPS);
           }

        }



    }

}



bool repeating_timer_callback_core_0(struct repeating_timer *t){

    if (CD_SNAKE_FRAME> 0)
    {
        CD_SNAKE_FRAME--;
    }

    if (CD_CLOCK_FRAME > 0)
    {
        CD_CLOCK_FRAME--;
    }

    if(STATE!=0){
        if (CD_PRESS_SOUND > 0)
        {
            CD_PRESS_SOUND--;
        }else{
            pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 0);
        }
    }

    if (CD_X > 0)
    {
        CD_X--;
    }
    if (CD_Y > 0)
    {
        CD_Y--;
    }
    if(CD_A>0){
        CD_A--;
    }
    if (CD_B > 0)
    {
        CD_B--;
    }
    if (CD_C > 0)
    {
        CD_C--;
    }
    if (CD_D > 0)
    {
        CD_D--;
    }

    if (STATE == 0)
    {
        volume=0.99;
        if (count < (1125 * dur))
        {
            count++;
        }
        else
        {
            count = 0;
            if (state == 0)
            {
                state = 1;
                dur = pause;
                vol = 0;
            }
            else
            {
                state = 0;
                dur = duration[ind];
                vol = 1000000 / (notes[ind] * (1 / volume));
            }
            uint slice_num = pwm_gpio_to_slice_num(2);

            pwm_set_wrap(slice_num, (1000000 / notes[ind]));
            pwm_set_chan_level(slice_num, PWM_CHAN_A, vol);
            if (state == 0)
            {
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

    // if(new_c){
    //     for (int i = 0; i < 5; i++)
    //     {
    //         printf("%c", myGPS.watch_dog[i]);
    //     }
    //     printf("\n------------------------------------------------\n");

    //     new_c=0;
    // }

    


    //printf("currentSelectorN: %d, drawSelectorN: %d\n", currentSelectorN, drawSelectorN);
    //printf("currentSettingN: %d, drawSettingN: %d\n", currentSettingN, drawSettingN);
    //printf("STATE: %d , menu_init: %d, App0_init: %d, currentSelectorN: %d \n", STATE, menu_init, App_0_init, currentSelectorN);
    //printf("SNAKE_CD: %d , SNAKE_X: %d, SNAKE_Y: %d, DIR: %d \n", CD_SNAKE_FRAME, mysnake.x, mysnake.y, mysnake.direction);
}

static PT_THREAD(pt_drawslr(struct pt *pt))
{

    PT_BEGIN(pt);
    if(STATE == -1){

        if(menu_init){
            tft_fillScreen(ILI9340_BLACK);
            draw_menu();
            draw_Selector(0);
            currentSelectorN = 0;
            menu_init = 0;
        }

        if (drawSelectorN != currentSelectorN)
        {
            removeAllSelector();
            draw_Selector(drawSelectorN);
            currentSelectorN = drawSelectorN;
        }
    }
    PT_END(pt);
   
}

static PT_THREAD(pt_app0(struct pt *pt))
{

    PT_BEGIN(pt);
    if (STATE == 0)
    {
        if (App_0_init)
        {
            tft_fillScreen(ILI9340_BLACK);
            draw_boundary();
            spawn_snake(&mysnake);
            draw_snake(&mysnake);
            App_0_init = 0;
        }

        if(CD_SNAKE_FRAME==0){

            remove_snake(&mysnake);
            if (!update_snake(&mysnake)){
                //game over
                STATE=-2;
            }
            draw_snake(&mysnake);
            CD_SNAKE_FRAME=SNAKE_FRAME_CD;
            
        }
    }else if(STATE==-2){
        //game over
        if (App_0_over_init)
        {
            draw_gameover(&mysnake);
            App_0_over_init = 0;
        }
    }
    PT_END(pt);
}

static PT_THREAD(pt_app1(struct pt *pt))
{

    PT_BEGIN(pt);
    if (STATE == 1)
    {
        
        if (App_1_init)
        {
            tft_fillScreen(ILI9340_BLACK);
            draw_boundary();

            App_1_init = 0;
        }

        if (CD_CLOCK_FRAME == 0)
        {

            draw_time_fix(&myGPS);
            
            CD_CLOCK_FRAME = CLOCK_FRAME_CD;
        }
    }

    PT_END(pt);
}

static PT_THREAD(pt_app2(struct pt *pt))
{

    PT_BEGIN(pt);
    if (STATE == 2)
    {
        if (App_2_init)
        {
            tft_fillScreen(ILI9340_BLACK);
            draw_setting();
            draw_Selector(0);
            App_2_init = 0;
        }

        if (drawSettingN != currentSettingN)
        {
            removeAllSelector();
            draw_Selector(drawSettingN);
            currentSettingN = drawSettingN;
        }

        
    }

    PT_END(pt);
}

static PT_THREAD(pt_app3(struct pt *pt))
{

    PT_BEGIN(pt);
    if (STATE == 3)
    {

        if (App_3_init)
        {
            tft_fillScreen(ILI9340_BLACK);
            draw_map();
            App_3_init = 0;
        }

        if(myGPS.FIX){
            draw_me(&myGPS);
        }
        // if (CD_CLOCK_FRAME == 0)
        // {

        //     draw_time_fix(&myGPS);

        //     CD_CLOCK_FRAME = CLOCK_FRAME_CD;
        // }
    }

    PT_END(pt);
}

void button_cb(uint gpio, uint32_t events)
{
        //exit to menu
        if (CD_Y == 0 && gpio == BUTTON_Y_PIN)
        {
            CD_Y = DEBOUNCE_CD;
            printf("Button: Y Back to menu\n");
            menu_init = 1; 
            App_0_init = 1;
            App_0_over_init = 1;
            App_1_init = 1;
            App_2_init=1;
            App_3_init = 1;

            //turn off any music
            pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 0);
            if(STATE!=0){
                // button press music
                pwm_set_wrap(pwm_gpio_to_slice_num(2), (1000000 / C4));
                pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 1000000 / (C4 * (1 / 0.99)));
                CD_PRESS_SOUND = PRESS_SOUND_CD;
            }

            STATE = -1;
        }


        //app0 snake
        if(STATE==0){
            if (CD_A == 0 && gpio == BUTTON_A_PIN)
            {
                if(CD_C>0&&CD_C>CD_B){
                    //A+C 0+1
                    mysnake.direction = 4;
                }
                else if (CD_B>0)
                {
                    //A+B 0+3
                    mysnake.direction = 7;
                }else{
                    mysnake.direction = 0;
                }

                CD_A = DEBOUNCE_CD;
            }
            if (CD_B == 0 && gpio == BUTTON_B_PIN)
            {
                if (CD_A > 0 && CD_A > CD_D)
                {
                    //B+A 3+0
                    mysnake.direction = 7;
                }
                else if (CD_D > 0)
                {
                    //B+D 3+2
                    mysnake.direction = 6;
                }else{
                    mysnake.direction = 3;
                }

                CD_B = DEBOUNCE_CD;
            }
            if (CD_C == 0 && gpio == BUTTON_C_PIN)
            {
                if (CD_A > 0 && CD_A > CD_D)
                {
                    //C+A 1+0
                    mysnake.direction = 4;
                }
                else if (CD_D > 0)
                {
                    //C+D
                    mysnake.direction = 5;
                }
                else
                {
                    mysnake.direction = 1;
                }
                CD_C = DEBOUNCE_CD;
            }
            if (CD_D == 0 && gpio == BUTTON_D_PIN)
            {
                if (CD_B > 0 && CD_B > CD_C)
                {
                    //D+B 2+3
                    mysnake.direction = 6;
                }
                else if (CD_C > 0)
                {
                    //D+C
                    mysnake.direction = 5;
                }
                else
                {
                    mysnake.direction = 2;
                }
                CD_D = DEBOUNCE_CD;
            }
        }
        //menu
        if(STATE==-1){
            if (CD_X == 0 && gpio == BUTTON_X_PIN)
            {
                printf("Button: X, Selecting Block: %d \n", currentSelectorN);
                CD_X = DEBOUNCE_CD;
                if (currentSelectorN == 0)
                {
                    // SNAKE
                    STATE = 0;
                }
                if (currentSelectorN == 1)
                {
                    // CLOCK
                    STATE = 1;
                }
                if (currentSelectorN == 2)
                {
                    // SETTING
                    STATE = 2;
                }
                if (currentSelectorN == 3)
                {
                    // MAP
                    STATE = 3;
                }
                if (STATE != 0)
                {
                    // button press music
                    pwm_set_wrap(pwm_gpio_to_slice_num(2), (1000000 / C4));
                    pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 1000000 / (C4 * (1 / 0.99)));
                    CD_PRESS_SOUND = PRESS_SOUND_CD;
                }
            }
            if (CD_A == 0 && gpio == BUTTON_A_PIN)
            {
                printf("Button: A\n");
                if(currentSelectorN==2){
                    drawSelectorN = 0;
                }
                if (currentSelectorN == 3)
                {
                    drawSelectorN = 1;
                }
                if (STATE != 0)
                {
                    // button press music
                    pwm_set_wrap(pwm_gpio_to_slice_num(2), (1000000 / C4));
                    pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 1000000 / (C4 * (1 / 0.99)));
                    CD_PRESS_SOUND = PRESS_SOUND_CD;
                }
                CD_A = DEBOUNCE_CD;
            }
            if (CD_B == 0 && gpio == BUTTON_B_PIN)
            {
                printf("Button: B\n");
                if (currentSelectorN == 1)
                {
                    drawSelectorN = 0;
                }
                if (currentSelectorN == 3)
                {
                    drawSelectorN = 2;
                }
                if (STATE != 0)
                {
                    // button press music
                    pwm_set_wrap(pwm_gpio_to_slice_num(2), (1000000 / C4));
                    pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 1000000 / (C4 * (1 / 0.99)));
                    CD_PRESS_SOUND = PRESS_SOUND_CD;
                }
                CD_B = DEBOUNCE_CD;
            }
            if (CD_C == 0 && gpio == BUTTON_C_PIN)
            {
                printf("Button: C\n");
                if (currentSelectorN == 0)
                {
                    drawSelectorN = 1;
                }
                if (currentSelectorN == 2)
                {
                    drawSelectorN = 3;
                }
                if (STATE != 0)
                {
                    // button press music
                    pwm_set_wrap(pwm_gpio_to_slice_num(2), (1000000 / C4));
                    pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 1000000 / (C4 * (1 / 0.99)));
                    CD_PRESS_SOUND = PRESS_SOUND_CD;
                }
                CD_C = DEBOUNCE_CD;
            }
            if (CD_D == 0 && gpio == BUTTON_D_PIN)
            {
                printf("Button: D\n");
                if (currentSelectorN == 0)
                {
                    drawSelectorN = 2;
                }
                if (currentSelectorN == 1)
                {
                    drawSelectorN = 3;
                }
                if (STATE != 0)
                {
                    // button press music
                    pwm_set_wrap(pwm_gpio_to_slice_num(2), (1000000 / C4));
                    pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 1000000 / (C4 * (1 / 0.99)));
                    CD_PRESS_SOUND = PRESS_SOUND_CD;
                }
                CD_D = DEBOUNCE_CD;
            }
        }
        //settings
        if (STATE == 2)
        {
            printf("currentSettingN: %d, drawSettingN: %d\n", currentSettingN, drawSettingN);
            if (CD_X == 0 && gpio == BUTTON_X_PIN)
            {
                CD_X = DEBOUNCE_CD;
                if (currentSettingN == 0)
                {
                    // SNAKE
                    printf("ez mode\n");
                    SNAKE_FRAME_CD=500;

                }
                if (currentSettingN == 1)
                {
                    // SNAKE
                    SNAKE_FRAME_CD = 300;

                }
                if (currentSettingN == 2)
                {
                    // SETTING
                    SNAKE_FRAME_CD = 200;
                }
                if (currentSettingN == 3)
                {
                    // SETTING
                    SNAKE_FRAME_CD = 100;
                }
                if (STATE != 0)
                {
                    // button press music
                    pwm_set_wrap(pwm_gpio_to_slice_num(2), (1000000 / C4));
                    pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 1000000 / (C4 * (1 / 0.99)));
                    CD_PRESS_SOUND = PRESS_SOUND_CD;
                }
            }
            if (CD_A == 0 && gpio == BUTTON_A_PIN)
            {
                printf("Button: A\n");
                if (currentSettingN == 2)
                {
                    drawSettingN = 0;
                }
                if (currentSettingN == 3)
                {
                    drawSettingN = 1;
                }
                if (STATE != 0)
                {
                    // button press music
                    pwm_set_wrap(pwm_gpio_to_slice_num(2), (1000000 / C4));
                    pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 1000000 / (C4 * (1 / 0.99)));
                    CD_PRESS_SOUND = PRESS_SOUND_CD;
                }
                CD_A = DEBOUNCE_CD;
            }
            if (CD_B == 0 && gpio == BUTTON_B_PIN)
            {
                printf("Button: B\n");
                if (currentSettingN == 1)
                {
                    drawSettingN = 0;
                }
                if (currentSettingN == 3)
                {
                    drawSettingN = 2;
                }
                if (STATE != 0)
                {
                    // button press music
                    pwm_set_wrap(pwm_gpio_to_slice_num(2), (1000000 / C4));
                    pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 1000000 / (C4 * (1 / 0.99)));
                    CD_PRESS_SOUND = PRESS_SOUND_CD;
                }
                CD_B = DEBOUNCE_CD;
            }
            if (CD_C == 0 && gpio == BUTTON_C_PIN)
            {
                printf("Button: C\n");
                if (currentSettingN == 0)
                {
                    drawSettingN = 1;
                }
                if (currentSettingN == 2)
                {
                    drawSettingN = 3;
                }
                if (STATE != 0)
                {
                    // button press music
                    pwm_set_wrap(pwm_gpio_to_slice_num(2), (1000000 / C4));
                    pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 1000000 / (C4 * (1 / 0.99)));
                    CD_PRESS_SOUND = PRESS_SOUND_CD;
                }
                CD_C = DEBOUNCE_CD;
            }
            if (CD_D == 0 && gpio == BUTTON_D_PIN)
            {
                printf("Button: D\n");
                if (currentSettingN == 0)
                {
                    drawSettingN = 2;
                }
                if (currentSettingN == 1)
                {
                    drawSettingN = 3;
                }
                if (STATE != 0)
                {
                    // button press music
                    pwm_set_wrap(pwm_gpio_to_slice_num(2), (1000000 / C4));
                    pwm_set_chan_level(pwm_gpio_to_slice_num(2), PWM_CHAN_A, 1000000 / (C4 * (1 / 0.99)));
                    CD_PRESS_SOUND = PRESS_SOUND_CD;
                }
                CD_D = DEBOUNCE_CD;
            }
        }

}

int main() {


    stdio_init_all();

    /////MUSIC///
    gpio_set_function(2, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(2);

    pwm_set_clkdiv(slice_num, CLKDIV);

    pwm_set_wrap(slice_num, 1000000 / 261.6);

    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);

    pwm_set_enabled(slice_num, true);

    //////////// UART////////////
    uart_init(uart1, 2400);

    //gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    int __unused actual = uart_set_baudrate(uart1, BAUD_RATE);
    uart_set_hw_flow(uart1, false, false);
    uart_set_format(uart1, DATA_BITS, STOP_BITS, PARITY);


    irq_set_exclusive_handler(UART1_IRQ, on_uart_rx);
    irq_set_enabled(UART1_IRQ, true);
    uart_set_irq_enables(uart1, true, false);
    ////////////////////////////

    tft_init_hw();
    tft_begin();

    gpio_init(BUTTON_X_PIN);
    gpio_set_dir(BUTTON_X_PIN, GPIO_IN);

    gpio_init(BUTTON_Y_PIN);
    gpio_set_dir(BUTTON_Y_PIN, GPIO_IN);

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);

    gpio_init(BUTTON_C_PIN);
    gpio_set_dir(BUTTON_C_PIN, GPIO_IN);

    gpio_init(BUTTON_D_PIN);
    gpio_set_dir(BUTTON_D_PIN, GPIO_IN);

    gpio_set_irq_enabled_with_callback(BUTTON_X_PIN, GPIO_IRQ_EDGE_RISE, true, &button_cb);
    gpio_set_irq_enabled_with_callback(BUTTON_Y_PIN, GPIO_IRQ_EDGE_RISE, true, &button_cb);
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_RISE, true, &button_cb);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_RISE, true, &button_cb);
    gpio_set_irq_enabled_with_callback(BUTTON_C_PIN, GPIO_IRQ_EDGE_RISE, true, &button_cb);
    gpio_set_irq_enabled_with_callback(BUTTON_D_PIN, GPIO_IRQ_EDGE_RISE, true, &button_cb);


    struct repeating_timer timer_core_0;
    add_repeating_timer_ms(-1,
                           repeating_timer_callback_core_0, NULL, &timer_core_0);
    pt_add_thread(pt_drawslr);
    pt_add_thread(pt_app0);
    pt_add_thread(pt_app1);
    pt_add_thread(pt_app2);
    pt_add_thread(pt_app3);
    pt_schedule_start;

}
