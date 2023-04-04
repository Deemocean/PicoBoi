# PicoBoi: A Handheld Gaming Device with GPS Functionalities
![IMG_9091-1-1-1536x1152](https://user-images.githubusercontent.com/39002684/229661348-a25eb4f5-2200-45f1-ab79-fdb74ddaf2d7.jpeg)


## Introduction
![boi](https://user-images.githubusercontent.com/39002684/229660775-6b64bf72-0756-4061-84da-48a455a8d7b0.JPG)

Drawing inspiration from the handheld gaming devices of our youth, we sought to utilize the RP2040 to recreate that experience with some cool and unique additions. Our Picoboi comes packed with a wide range of features, all of which are available in a device with a relatively compact form factor. These features include different game modes, original music, accurate time and GPS navigation. The Picoboi is a culmination of many of the ideas, tools and protocols covered in the course, and our intention was to create a stand alone device and platform capable of hosting a large variety of applications. Through our implementation of snake, GPS navigation and audio our intention is to show that with some creativity and ingenuity the sky's the limit for the Picoboi.

## High level design

This project aims to have a handheld device similar to a traditional GameBoy, but with some modern features. So it should have most of the hardware functionality of a GameBoy, such as a screen, a control pad, a speaker to play music, and a charging system. Besties those, we also added the support for GPS and wrote a library for that usage. Instead of having a linear-style software system, the way we designed our system treated each of the applications separately, meaning each software owns its own running environment and should be able to run in the background if necessary.  As we want to build this device from the ground up, we didn't go with the way of emulation but wrote a user interface system to contain our own applications. For the applications, we wrote a total of four apps: a snake game that the snake has the ability to move diagonally, a setting for the snake game in which the user can change the difficulty of the game, a satellite clock shows the current time in EST and indicating if the GPS module is getting a fix, and a map tracks the path where the user has been going through. This way, the user is able to have this standing-alone device, turn it on, and select the application they wish to run.

We tried several ways to improve the software performance, one of which is bringing the online computation offline, for example, the projection of the latitude and longitude of the user onto the 2D map is being implemented by using a python script to calculate the conversion factors and we hardcoded those coefficients into the GPS library we wrote to improve real-time performance.

### Equirectangular projection
![map](https://user-images.githubusercontent.com/39002684/229660808-b3168f23-d941-4d16-8012-6bcdd4a16bd2.png)

The Latitude and Longitude information we have describes a point on a sphere, however, in order to display that on a 2d flat surface(our screen), we need to project such a point onto the plane we have. As the area we gonna display on the screen is fairly small(the Cornell Campus), we can use some relatively simple projections such as the equirectangular project. Let us use $R$ to denote the radius of the earth in meters, $\lambda$ as the longitude of the point we want to project, $\phi$ as the latitude of the point, and we collect a center point from the map, where $\phi_0$ is the latitude of the center point and $\lambda_0$ is the longitude of the center point, we can have projection:

$x=R(\lambda-\lambda_0)cos(\lambda_0)$

$y=R(\phi - \phi_0)$

And we can calculate the $x,y$ value of the top-left and bottom-right points of our map, then do a two-point linear regression to convert the $x,y$ value to pixel values on the screen. Those numbers can be computed offline as such:


```python
import numpy as np
```


```python
#lat-long
mid=[42.444493,-76.483730 ]
up_left=[42.455345,-76.494570]
buttom_right=[42.433823,-76.472446]

home=[42.43528657760313, -76.48587321578678]
R=6371000
```


```python
X_left=np.cos(mid[1]*(np.pi/180))*R*(np.pi/180)*(up_left[1]-mid[1])
X_right=np.cos(mid[1]*(np.pi/180))*R*(np.pi/180)*(buttom_right[1]-mid[1])

Y_down=R*(np.pi/180)*(buttom_right[0]-mid[0])
Y_up=R*(np.pi/180)*(up_left[0]-mid[0])
print("x1: ", X_left,"x2: ", X_right)
print("y1: ", Y_down,"y2: ", Y_up)
```

    x1:  -281.71688041438676 x2:  293.2558375085227
    y1:  -1186.4498672979555 y2:  1206.687343946736



```python
x_proj_slope = (240-0)/(X_right-X_left)
x_proj_bias = 240-x_proj_slope*X_right


y_proj_slope = (320-0)/(Y_up-Y_down)
y_proj_bias = 320-y_proj_slope*Y_up

print("xp=",x_proj_slope,"*x+",x_proj_bias)
print("yp=",y_proj_slope,"*x+",y_proj_bias)
```

    xp= 0.41741110929054287 *x+ 117.59175555964036
    yp= 0.1337156927301988 *x+ 158.64696589539855


## Program/hardware design

### Hardware
![hardware](https://user-images.githubusercontent.com/39002684/229660880-3fbd9a73-dea9-4b2d-ac61-43ffa065ca86.png)


### The Modularized System
In order to decouple subsystems, we decided to divide our hardware into three separated components: The Control Pad, the Button Pad, and the Screen. The button pad and the screen connect the control pad with a custom connecter onto a custom socket designed for that connecter. This way, we can modulize the whole system to prevent single-point failure.
#### The Control Pad

![controlfront](https://user-images.githubusercontent.com/39002684/229660907-86c85ec6-8edb-4708-b99c-fadc8def4c09.JPG)


The control pad has most of the electric hardware including the charging/power system, which is the combination of the three components, the 2000mAh Lip battery, the Lipo charger with a Type-C connector, and a switch for the user to turn on and off the device. For the microcontroller, we are using a Raspberry Pi Pico, which is used for all the computing tasks of our program, and the code and the byte array of our images are stored in its 2MB flash. And for the connection with the Adafruit Ultimate GPS module, as we only need to receive data from the GPS we only connect the RX port on the Pi Pico to the TX port on the GPS module. And we connect the coin speaker directly. Even tho we have built the OpAmp circuit while prototyping, for the case we designed, we decided to just connect them directly.


#### The Button Pad
![buttonpadf](https://user-images.githubusercontent.com/39002684/229660938-5a0ea3f0-fa9a-4147-827f-35e9554febe8.JPG)

The Button Pad supports all the major user interactions with the 6 buttons on the pad, the button X, Y, A, B, C, D. The X button is used for the "select/enter" functionalities, the Y button is used for "exit" , and the rest 4 buttons are for "select".

### The Case
<img width="443" alt="case" src="https://user-images.githubusercontent.com/39002684/229660961-2a4da67a-5ac9-4ca1-88a6-cf05ed8ea3db.png">

For the case, we developed several prototypes, with the first one being too small(with respect to our socket design). We designed the case using OnShape MCAD tool, and printed it with a resin 3D printer.

### Software

#### System Overview
![threads](https://user-images.githubusercontent.com/39002684/229660997-67a284a5-bf2e-44d5-bc20-6acf0f7cbc8e.png)

As for the structure of the system, we have a main() function to do the system initialization, including setting up the GPIO function and interrupts, the repeating timer, and the threads. There are five threads in total running on core 0, one for each application, plus one for the menu. And three interrupt handlers: the RX interrupt handler, the repeating timer interrupt, and the button press interrupt.

#### The State Machine and variables

A global variable is declared to keep track of the current state of the system, for example, we use -1 to indicate the system is in the menu, 0 for in app0, 1 for in app1…etc. In each thread, it first checks if the current state is the application this thread corresponds to, if not, this thread will be ignored.

For each application–including the menu, we have declared a binary initialization variable to indicate if this is the first time this thread got executed. And the thread executes some initial setups depending on the value of those variables.

### Buttons

The user uses the buttons on the control pad to input instructions, and the functionality of the buttons on the pad depends on the specific application that is currently running.

#### The Debouncing Algorithm

We want to ensure the button presses reflect the real situation, so a debouncing algorithm is implemented to prevent a single press from being detected multiple times. This is achieved by having a repeat timer interrupt triggered every 1 millisecond. In total, we have 6 buttons, and we declared a cool-down variable for each of the buttons, for the cool-down variable being larger than 0, the button is not valid to be detected as pressed. And every valid press of the button will reset this cool-down variable to a predetermined value.

#### The Button Callback function

<img width="1099" alt="buttoncb" src="https://user-images.githubusercontent.com/39002684/229661029-671d74a6-b6a5-4d95-ab8f-14ef85607c2d.png">


All the GPIOs for the buttons are sharing one single callback function which is triggered on the rising edge of each GPIO port. This function carries the corresponding GPIO number of the one triggered. We validate a button press by checking if its corresponding debouncing cooldown variable reaches zero. However, the response of the button press depends on the current state of the system. Except for the button Y, which is used as an exit button, and that functionality is valid throughout the system. And what it does is reset the cool-down variable for button Y, and set all the initialization variables to 1 as all the apps are exited. And turn off any possible on-playing music.

### Menu
![menu](https://user-images.githubusercontent.com/39002684/229661050-00c68d69-2984-4f5e-b74f-bd61e9feea43.JPG)


We have created a library for the menu, and the graphics functionalities are based on the library built for the screen we are using[1]. This library is mainly being used to provide functionalities to draw the outlines and logo for each application and the selectors.
The Logo Bitmap
We created some png logos in Photoshop with the correct sizes, converted each pixel to a byte value, formulate the whole picture to a byte array by an online website[4], and we stored the byte array in our header file.

#### Menu Thread
If the current state is the menu, then inside of the thread, for the initialization, it draws a black background and draws the outline and the logo for the apps. And set the currently selected app to app0. As we have two variables, one is used to indicate the currently selected app, and the other one is changed inside of the button callback function to instruct the next app to select(this semaphore approach is used to not delay the interrupt). And for every execution of the thread, it checks if the currently selected app is the same as the draw command sent from the button callback, if they are different, it calls functions from the library to remove the previous selector, draw the one commanded, and reset the current application selected to be the one just drawn.
 

### The Snake Game
![snake](https://user-images.githubusercontent.com/39002684/229661065-27fda73c-7c15-4e67-9658-1f057c721d2c.JPG)


A library written from the snake game has all the functions and struct we needed for the snake game. We have a snake type declared to keep track of all the information during the game, which includes: two arrays that stores the x and y coordinate of the snake body, an int variable that stores the direction of the snake head, which has 8 possibilities as our snake supports diagonal movements, and variable keeps track of the length of the snake, and an array of size two which stores the current location of the food. A snake is declared as a global static variable. As we want a way to control the frame rate of the game, we declared a cooldown variable for the snake game update, which is decremented in the timer callback function.

#### The Snake Thread

```
if (App_0_init)
{
   tft_fillScreen(ILI9340_BLACK);
   draw_boundary();
   spawn_snake(&mysnake);
   draw_snake(&mysnake);
   App_0_init = 0;
}

```
For app initialization, we draw a black background and draw the boundary of the arena, then we call the spawn_snake() function to assign an initial value to each field of the snake, followed by draw_snake() which only draw the new head of the snake while remove_snake removes the tail of the snake.

```
       if(CD_SNAKE_FRAME==0){
           remove_snake(&mysnake);
           if (!update_snake(&mysnake)){
               //game over
               STATE=-2;
           }
           draw_snake(&mysnake);
           CD_SNAKE_FRAME=SNAKE_FRAME_CD;
          
       }

```
If CD_SNAKE_FRAME counts to zero, the next frame is ready to be executed. The update_snake() function returns a boolean value, indicating whether the game is over or not. If the game continues, we draw the snake and reset the frame countdown variable. Otherwise, we switch to the game over state.

### Snake Game Logic

### ROrand()-The Real Random Number Generator

As the foods are supposed to be spawned randomly, surely, we can call the naive random function with some specified seeds, but that will give the same number every time the board boots up. Because such a random number is calculated with the same parameter every time for that algorithm. So in order to make the foods spawn at different positions, an external random source has to be introduced. On the RP2040s, there is something called ring oscillators, which are basically a bunch of odd-number NOT gates. As a result, the reading of the output for these ring oscillators is not predictable.

```
// return random number by ROSC between 0-2^N
// N should be in [0,32]
fix15 ROrand(int N)
{
 static volatile uint32_t *randbit_reg = (uint32_t *)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);
 unsigned int random = 0;
 for (int i = 0; i < N; i++)
 {
   fix15 random_bit = 0x1 & (*randbit_reg);
   random = random << 1 | random_bit;
 }
 return int2fix15(random);
}
```
By using this feature, we were able to create a function with an input of integers ranging from 0 to 32, and the function outputs a fix15 ranging from 0-2^16(if unsigned int the range would be  0-2^32). 
***ROSC_BASE + ROSC_RANDOMBIT_OFFSET*** are the register addresses for the random bit value for the ring oscillator, and we can either include the corresponding header file or just define them ourselves. 
Thus we declared a pointer to this register and make it volatile. Depending on the value of the random number we want, we run a for loop, in which we read the value of that random bit and shift that bit into the random number we want to output. This operation should be fairly efficient as the shift operation is very cheap. 

### Snake Update
We calculate the new location of the head with respect to the direction in which the snake is moving. And we declare a temperate location array, set the first element of the array as the new head, and move the previous location array to the rest of the temp array. This way, we calculate the position array for the next frame. With this new head location, we can check if the next frame is game over, by checking if the new head location is within some distance to the wall or the snake body, or if the game is over because the length of the snake is over the max allowed length. 
We generate the position of the food randomly by using this random number generator. For the event of the new head hitting the food, we remove the old food, copy the temp array to the snake’s field, and increase the length. Otherwise, we don't keep the tail and keep the length unchanged.

### Game Over

![gameover](https://user-images.githubusercontent.com/39002684/229661085-696de271-f470-4e38-830f-c3e4e5033fdf.JPG)


In the event of a game over, we change the current state to the game over state. Which shows a game over the screen and a score showing the length of the snake before death.

### Snake Setting
![setting](https://user-images.githubusercontent.com/39002684/229661102-db18e22e-0d38-458e-86fe-a3d24157e3da.JPG)



The difficulty of the snake game is depending on the frame update speed, the faster the snake, the harder the game is. The selection of the difficulty UI reuses the menu graphic library but uses two extra different selection variables currentSettingN and drawSettingN, for the selection of the different difficulty levels, it changes the frame cool-down variable that resets.

### GPS

As the library adafruit provides is dependent on the Arduino filesystem and is written in C++. So we implemented our own GPS library from scratch. 
The GPS module constantly sends GPS messages through the UART channel, we have a UART RX interrupt handler for the event of receiving characters from the UART channel. As we need channel 0 for serial debugging, for the UART communication with the GPS module, we use channel 1. For the GPS struct, we have a buffer array called watch_dog with some specified size to store the latest data from the GPS, a character array stores the UTC time, 3 int variable stores the current hour, minute, and second in EST time, and two float variable each store the Latitude and Longitude information

In the RX interrupt handler we call a function in the library, buffer_shift, that shift a character to the end of the buffer and shift out the first character out from the buffer. This way, the buffer constantly contains the current GPS message. Even though there are a lot of GPS messages with different headers that can provide much different information, the one we care about is the GNRMC messages, which contain the UTC time, fix status, and location information. So we check the first five characters in the array, if that is GNRMC, we know the following data format. This way, we can get the time, location, and fix status by collecting the character at different intervals in the watch_dog. With the RTC battery, the GPS always sends the valid time if it at least got a fix before. However, it is not true for the location as the RTC would not help if there is no fix, thus we only get an update on the location if only the fix status is valid.
For the time, we process the string by converting the UTC hour, minute, and second to integer, and converting to EST time by subtracting the UTC time by 5(with the corresponding time rules). And stores them in the corresponding struct fields. For the fix status, we check if that character is “V” or “A”, if it is V, then the status is not valid, and vice versa. For the latitude and longitude, they are in the unit of degree-minute, and we have the function converting it to the radius.

As the interrupt function runs, the field in the GPS struct keeps updating itself, and we can extract that information as we need.

### The Sat Clock
![clock](https://user-images.githubusercontent.com/39002684/229661129-3aaff7ab-0dae-47c9-99d8-37cf9109d240.JPG)


This application provides a fix status indicator and a clock in EST time. For initialization, it draws the background and the boundary using the menu library function.  As we don't want the screen to update too frequently, we also have a cool-down variable for the frame update. With a valid frame update, we display a red dot if we check FIX as 0, and green otherwise. For the time, instead of writing the string and then removing it by drawing a black rectangle. A built-in function in the screen library can let us specify the background color and update the string without the flicking effects. We process the 3 int variables to string and use this function to update the on-screen clock.

### The Sat Map

![satmap](https://user-images.githubusercontent.com/39002684/229661148-17699c9c-0de6-418c-ad53-144f44732a54.JPG)


The map shows the Cornell campus as a background, and displays the user’s current location as a blue dot, as we don't remove the previous dot, the app shows the trace of the user has been since the app opened. With the offline projection calculation, we have a conversion function hardcoded. 
We screenshot a piece of the Cornell campus map from Google Maps, and we converted it to a byte array as we did for the logos. 

#### Map Thread
For the thread initialization, we draw the background using the stored Cornell campus bitmap. As for the rest of the thread, what it does is repeatedly poll the current latitude and longitude information from the GPS struct, and convert it to the corresponding pixel values on the screen, and draw it with a blue dot.

### Audio
To achieve the retro sounding music and forgo using a DAC, music and audio is generated using square wave output from a GIPO port configured for PWM. Music and audio were synthesized using interrupts, which were implemented using a repeating timer and a repeating timer callback function. Notes were decomposed into their frequencies which we played and manipulated by setting the PWM wrap by dividing the number of clock cycles per second by our desired frequency in hertz. Furthermore, volume was manipulated by varying the PWM channel level, or duty cycle. The note, or sound, would be changed after a certain number of interrupts where that number is determined by how long we want the sound to be played. With this implementation, we were able to construct songs using an array comprised of all the frequencies of the notes we wanted to play as well as a corresponding array of the desired duration of each of these notes. With this being the case, we were able to iterate through the array changing the note by resetting the wrap value and the duration by adjusting the number of interrupts before the next change.
 

## Results

Any and all test data, scope traces, waveforms, etc
speed of execution (hesitation, filcker, interactiveness, concurrency)
accuracy (numeric, music frequencies, video signal timing, etc)
how you enforced safety in the design.
usability by you and other people

We intended to make the applications in the system isolated and the execution of one of the applications should not halt the execution of the others. So the applications, including the menu are multithreaded and use their own set of variables. The such decoupled design makes adding applications or swapping between applications have less chance to freeze or conflict. And we designed each application to have a similar execution scene--initialization, and app logic that is being executed over and over again.

For button debouncing, we chose a debounce value of 200ms, and we did a test, that is printing out a message from the serial port to indicate a button is pressed. Without debouncing, it has a high chance of showing a button being pressed multiple times in a row while in the reality it was only one press. However, with the debounce algorithm, this issue is fully solved, as no matter how we press the button, there is only one message for one button press. And 200ms is more than enough for our applications.

When we designed the graphics implementation of the snake game, in order to prevent mass flickering each frame the snake moves, instead of removing the whole snake and redrawing the whole new position. We only remove the tail and redraw the new head, so there is no high-frequency removal and redraw, which prevents flickering. 

Also for the clock app, we utilized a built-in function for string drawing, that as we specify the background color, the screen buffers the background and updates the new string without flickering. 

However, for the music, as we didnt have enough room for a large OpAmp circiut we built during prototyping, we directly connected the speaker to the GPIO pin, which makes the sound quality not modern enough.

In general, PicoBoi is a very stale device, there is no flickering or performance issue that the user can notice, including the video signal and the music.

In order to ensure the safety of our design, we utilized a 3D printed case to enclose all of the hardware, only exposing to the user necessary elements such as buttons and the screen. This protects them from any dangers associated with the battery used to power the Picoboi. Furthermore,  it creates an elegant and simple-looking device devoid of visible clutter associated with the densely packed hardware improving user experience. Lastly, exclusively exposing to the user elements which they are capable of interacting with helps to avoid confusion and increases usability. 

## Conclusion

Throughout the process of developing the PicoBoi, in some realms, we ultimately needed to shift our expectations. As with most projects under a time crunch, evolving expectations are necessary as it becomes apparent that some goals may be more, or less, achievable than initially thought. In addition, hardware technical difficulties resulted in setbacks, but overcoming these challenges, at least in retrospect, were some of the most interesting aspects of Picoboi's development. All this being said, the Picoboi was able to meet our expectations as it came complete with all of the features we had initially intended. 

Although we were generally pleased with the Picoboi we had constructed, that is not to say there aren’t optimizations to be made. One of the primary reasons this project was so interesting is that all of our design choices were in many ways a battle between form and function. In its development, we ultimately wanted to include as many features as possible to maximize user experience; however if those additions required adding hardware, then we would have to increase the size of the device in turn decreasing user experience. This required a lot of critical decisions to be made. For example, we did not include the amplifier circuit that we built because it was too large, but as a result we are limited in our audio volume and quality. Design choices like using a PCB rather than a solder board and buying an amplifier rather than building an amplifier circuit are just a few ways we can improve the PicoBoi and shift our device more in the handheld pocket friendly direction.

All of our code was built from the ground up which extends to code design and music. We did not utilize anything that is open source or in the public domain. Although choosing to go this route may have hindered us in the quantity of features we were able to offer, our final product is something that is unique and the experience of building all of its features was a great learning experience. Although the implementations may be unique, there is nothing specifically new or novel related to our project and as a result, there aren’t any apparent patent opportunities
From an ethical standpoint, our project doesn’t raise any red flags. Foregoing the dangers associated with any toy or handheld gaming device, The PicoBoi doesn’t appear to pose any significant risks to the user however there are probably steps that would need to be taken before it could meet industry standard if it were to go into production. Outside of some of the graphics that were used to indicate game modes and features on our GUI, we do not utilize anyone else’s intellectual property so there are no major legal considerations to be made.

Development of the PicoBoi was both a fun project and a great learning experience. In doing so we were able to continue to explore many of the topics covered in this class and delve into some of the features and capabilities of the RP2040.


## Reference:

[1] TFT Screen lib modifided by Syed: http://tahmidmc.blogspot.com/2014/10/interfacing-color-tft-display-with.html

[2] Pi Pico Pinout and Datasheet: https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html#raspberry-pi-pico

[3] https://afrotechmods.com/tutorials/2017/01/17/how-to-make-a-simple-1-watt-audio-amplifier-lm386-based/

[4] img to byte array: https://www.mischianti.org/images-to-byte-array-online-converter-cpp-arduino/


