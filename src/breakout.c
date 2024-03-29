#include "fb.h"
#include "mini_uart.h"
#include "peripherals/base.h"
#include "timer.h"
#include "printf.h"
#include "irq.h"
#include "peripherals/irq.h"
#include "print_to_screen.h"
#include "breakout.h"

unsigned int bricks = ROWS * COLS;

// OBJECT TRACKING
struct Object {
    unsigned int type;
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    unsigned char alive;
};

enum {
    OBJ_NONE   = 0,
    OBJ_BRICK  = 1,
    OBJ_PADDLE = 2,
    OBJ_BALL   = 3
};

unsigned int numobjs = 0;
struct Object *objects = (struct Object *)SAFE_ADDRESS;
struct Object *ball;
struct Object *paddle;

int lives = NUM_LIVES;
int points = 0;

void removeObject(struct Object *object) {
    drawRect(object->x, object->y, object->x + object->width, object->y + object->height, 0, 1);
    object->alive = 0;
}

void moveObject(struct Object *object, int xoff, int yoff) {
    moveRect(object->x, object->y, object->width, object->height, xoff, yoff, 0x00);
    object->x = object->x + xoff;
    object->y = object->y + yoff;
}

struct Object *detectCollision(struct Object *with, int xoff, int yoff) {
    for (int i=0; i<numobjs;i++) {
	if (&objects[i] != with && objects[i].alive == 1) {
	   if (with->x + xoff > objects[i].x + objects[i].width || objects[i].x > with->x + xoff + with->width) {
              // with is too far left or right to ocllide
	   } else if (with->y + yoff > objects[i].y + objects[i].height || objects[i].y > with->y + yoff + with->height) {
              // with is too far up or down to ocllide
	   } else {
	      // Collision!
	      return &objects[i];
	   }
        }
    }
    return 0;
}

// KEY HANDLER
char getUart(){
    char ch = 0;
    if (uart_input_char_from_irq) ch = uart_input_char_from_irq;
    printf("breakout received %c\n", ch);
    uart_input_char_from_irq = 0;
    return ch;
}

// OBJECT INITIALISERS

void initBricks() {
    int brickwidth = 32;
    int brickheight = 8;
    int brickspacer = 20;
    static int brickcols[] = { 0x11, 0x22, 0xEE, 0x44, 0x66 };

    int ybrick = MARGIN + brickheight;

    for (int i=0; i<ROWS; i++) {
        int xbrick = MARGIN + (VIRTWIDTH/COLS/2) - (brickwidth/2);
        
        for (int j = 0; j<COLS; j++) {
            drawRect(xbrick, ybrick, xbrick+brickwidth, ybrick+brickheight, brickcols[i], 1);

            objects[numobjs].type = OBJ_BRICK;
            objects[numobjs].x = xbrick;
            objects[numobjs].y = ybrick;
            objects[numobjs].width = brickwidth;
            objects[numobjs].height = brickheight;
            objects[numobjs].alive = 1;
            numobjs++;

            xbrick += (VIRTWIDTH/COLS);
        }
        ybrick = ybrick + brickspacer;
     }
}

void initBall() {
    int ballradius = 15;

    drawCircle(WIDTH/2, HEIGHT/2, ballradius, 0x55, 1);

    objects[numobjs].type = OBJ_BALL;
    objects[numobjs].x = (WIDTH/2) - ballradius;
    objects[numobjs].y = (HEIGHT/2) - ballradius;
    objects[numobjs].width = ballradius * 2;
    objects[numobjs].height = ballradius * 2;
    objects[numobjs].alive = 1;
    ball = &objects[numobjs];
    numobjs++;
}

void initPaddle() {
    int paddlewidth = 80;
    int paddleheight = 20;

    drawRect((WIDTH-paddlewidth)/2, (HEIGHT-MARGIN-paddleheight), (WIDTH-paddlewidth)/2 + paddlewidth, (HEIGHT-MARGIN), 0x11, 1);

    objects[numobjs].type = OBJ_PADDLE;
    objects[numobjs].x = (WIDTH-paddlewidth)/2;
    objects[numobjs].y = (HEIGHT-MARGIN-paddleheight);
    objects[numobjs].width = paddlewidth;
    objects[numobjs].height = paddleheight;
    objects[numobjs].alive = 1;
    paddle = &objects[numobjs];
    numobjs++;
}

void drawScoreboard(int score, int lives)
{
    char tens = score / 10; score -= (10 * tens);
    char ones = score;

    drawString((WIDTH/2)-252, MARGIN-25, "Score: 0     Lives:  ", 0x0f, 3);
    drawChar(tens + 0x30, (WIDTH/2)-252 + (8*8*3), MARGIN-25, 0x0f, 3);
    drawChar(ones + 0x30, (WIDTH/2)-252 + (8*9*3), MARGIN-25, 0x0f, 3);
    drawChar((char)lives + 0x30, (WIDTH/2)-252 + (8*20*3), MARGIN-25, 0x0f, 3);
}

void breakout_init(){
    lives = NUM_LIVES;
    points = 0;
    clear_screen();
    initBricks();
    initBall();
    initPaddle();
    drawScoreboard(points, lives);
    uart_input_char_from_irq = 0;
    printf("Game Initialization done\n");
    //print_to_screen("Press ANY key to start\n");
}

int initial_v_x = 3;
int initial_v_y = 9;

int breakout() {
    struct Object *foundObject;
    unsigned char ch = 0;

    int velocity_x = initial_v_x;
    int velocity_y = initial_v_y;

    

    while(1){
        if (uart_input_char_from_irq) {
            ch = uart_input_char_from_irq;
            printf("breakout received %c\n", ch);
            uart_input_char_from_irq = 0;   
            break;
        }
    }
    breakout_init();
    timer_sleep(1000); // wait for initilization
    // Wait for keypress to start game
    // while (!getUart()); 
    
    printf("Game started\n");
     
    while (lives > 0 && bricks > 0) {
        printf("fresh\n");
        // Get any waiting input and flush the buffer
        if (uart_input_char_from_irq == 'l' || uart_input_char_from_irq == 'h'){
            ch = uart_input_char_from_irq;
            printf("main game loop received %s\n", ch);
        } 
        
        uart_input_char_from_irq = 0;  
        
        // if ( ch = getUart() ) {
            // if (ch == 0) continue;
            if (ch == 'l') if (paddle->x + paddle->width + (paddle->width / 2) <= WIDTH-MARGIN) moveObject(paddle, paddle->width / 2, 0);
            if (ch == 'h') if (paddle->x >= MARGIN+(paddle->width / 2)) moveObject(paddle, -(paddle->width / 2), 0);
        // }
        // uart_loadOutputFifo();
        ch = 0;

        // Are we going to hit anything?
        foundObject = detectCollision(ball, velocity_x, velocity_y);

        if (foundObject) {
            printf("Collision detected\n");
            if (foundObject == paddle) {
                velocity_y = -velocity_y;
            // Are we going to hit the side of the paddle
            if (ball->x + ball->width + velocity_x == paddle->x || ball->x + velocity_x == paddle->x + paddle->width) velocity_x = -velocity_x;
            } else if (foundObject->type == OBJ_BRICK) {
                removeObject(foundObject);
                velocity_y = -velocity_y;
                bricks--;
                points++;
                drawScoreboard(points, lives);
            }
        }

        timer_sleep(40); // Wait a little...
        moveObject(ball, velocity_x, velocity_y);

        // Check we're in the game arena still
        if (ball->x + ball->width >= WIDTH-MARGIN) {
            velocity_x = -velocity_x;
        } else if (ball->x <= MARGIN) {
            velocity_x = -velocity_x;
        } else if (ball->y + ball->height >= HEIGHT-MARGIN) {
            lives--;

            removeObject(ball);
            removeObject(paddle);

            initBall();
            initPaddle();
            drawScoreboard(points, lives);
        } else if (ball->y <= MARGIN) {
            velocity_y = -velocity_y;
        }

        
    }

    if (bricks == 0){
        drawString((WIDTH/2)-(END_WIDTH/2), (HEIGHT/2)-(END_HEIGHT/2), "Well done!", 0x02, END_ZOOM);
        return points+lives;
    }else{
        drawString((WIDTH/2)-(END_WIDTH/2), (HEIGHT/2)-(END_HEIGHT/2), "Game over!", 0x04, END_ZOOM);
        return -1;
    } 

    printf("game ended\n");
    
}
