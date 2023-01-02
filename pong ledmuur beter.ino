#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#define UP_BUTTON 2
#define DOWN_BUTTON 3

// Color definitions
#define LED_BLACK		0
#define LED_RED		(15 << 11)
#define LED_GREEN 	(31 << 5)  
#define LED_BLUE 	15
#define LED_ORANGE	(LED_RED  + LED_GREEN)
#define LED_PURPLE	(LED_RED  + LED_BLUE)
#define LED_CYAN	(LED_GREEN  + LED_BLUE)
#define LED_WHITE	(LED_RED  + LED_GREEN  + LED_BLUE)


const unsigned long PADDLE_RATE = 125;
const unsigned long BALL_RATE = 250;
const uint8_t PADDLE_HEIGHT = 2;
int MAX_SCORE = 8;

int CPU_SCORE = 0;
int PLAYER_SCORE = 0;

#define LED_PIN 8

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, LED_PIN,
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
    NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
    NEO_GRBW + NEO_KHZ800);

void drawCourt();

uint8_t ball_x = 3, ball_y = 3;
uint8_t ball_dir_x = 1, ball_dir_y = 1;

boolean gameIsRunning = true;
boolean resetBall = false;


unsigned long ball_update;
unsigned long paddle_update;

const uint8_t CPU_X = 1;
uint8_t cpu_y = 3;

const uint8_t PLAYER_X = 6;
uint8_t player_y = 3;

void setup() {

    pinMode(UP_BUTTON, INPUT_PULLUP);
    pinMode(DOWN_BUTTON, INPUT_PULLUP);
    matrix.begin();
    matrix.fillScreen(0);
    matrix.show();

    unsigned long start = millis();

    drawCourt();

    while (millis() - start < 2000);
    ball_update = millis();
    paddle_update = ball_update;
    ball_x = random(3, 4);
    ball_y = random(3, 4);
}

void loop() {

    unsigned long time = millis();
    static bool up_state = false;
    static bool down_state = false;

    up_state |= (digitalRead(UP_BUTTON) == LOW);
    down_state |= (digitalRead(DOWN_BUTTON) == LOW);

    if (resetBall)
    {
        ball_x = random(3, 4);
        ball_y = random(3, 4);
        do
        {
            ball_dir_x = random(-1, 1);
        } while (ball_dir_x == 0);

        do
        {
            ball_dir_y = random(-1, 1);
        } while (ball_dir_y == 0);


        resetBall = false;
    }

    if (time > ball_update && gameIsRunning) {
        uint8_t new_x = ball_x + ball_dir_x;
        uint8_t new_y = ball_y + ball_dir_y;

        // Check if we hit the vertical walls
        if (new_x == 0) //Player Gets a Point
        {
            PLAYER_SCORE++;
            if (PLAYER_SCORE == MAX_SCORE)
            {
                gameOver();
            }
            else
            {
                showScore();
            }
        }

        // Check if we hit the vertical walls
        if (new_x == 7) //CPU Gets a Point
        {
            CPU_SCORE++;
            if (CPU_SCORE == MAX_SCORE)
            {
                gameOver();
            }
            else
            {
                showScore();
            }
        }

        // Check if we hit the horizontal walls.
        if (new_y == 0 || new_y == 7) {
            ball_dir_y = -ball_dir_y;
            new_y += ball_dir_y + ball_dir_y;
        }

        // Check if we hit the CPU paddle
        if (new_x == CPU_X && new_y >= cpu_y && new_y <= cpu_y + PADDLE_HEIGHT) {
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        // Check if we hit the player paddle
        if (new_x == PLAYER_X
            && new_y >= player_y
            && new_y <= player_y + PADDLE_HEIGHT)
        {
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        matrix.drawPixel(ball_x, ball_y, LED_BLACK);
        matrix.drawPixel(new_x, new_y, LED_WHITE);
        matrix.show();
        ball_x = new_x;
        ball_y = new_y;

        ball_update += BALL_RATE;

    }

    if (time > paddle_update && gameIsRunning) {
        paddle_update += PADDLE_RATE;

        // CPU paddle
        matrix.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, LED_BLACK);
        const uint8_t half_paddle = PADDLE_HEIGHT >> 1;
        if (cpu_y + half_paddle > ball_y) {
            cpu_y -= 1;
        }
        if (cpu_y + half_paddle < ball_y) {
            cpu_y += 1;
        }
        if (cpu_y < 1) cpu_y = 1;
        if (cpu_y + PADDLE_HEIGHT > 7) cpu_y = 7 - PADDLE_HEIGHT;
        matrix.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, LED_RED);
        matrix.show();

        // Player paddle
        matrix.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, LED_BLACK);
        if (up_state) {
            player_y -= 1;
        }
        if (down_state) {
            player_y += 1;
        }
        up_state = down_state = false;
        if (player_y < 1) player_y = 1;
        if (player_y + PADDLE_HEIGHT > 7) player_y = 7 - PADDLE_HEIGHT;
        matrix.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, LED_GREEN);
        matrix.show();
    }
  matrix.show();
}

void drawCourt() {
  matrix.drawRect(0, 0, 8, 8, LED_WHITE);
  matrix.show();
}

void gameOver()
{
    gameIsRunning = false;
    matrix.fillScreen(LED_BLACK);
    drawCourt();
    /*if (PLAYER_SCORE > CPU_SCORE)
    {
        display.setCursor(5, 4);
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.print("You Won");
    }
    else
    {
        display.setCursor(5, 4);
        display.setTextColor(WHITE);
        display.setTextSize(2);
        display.print("CPU WON");
    }

    display.setCursor(20, 30);
    display.setTextColor(RED);
    display.setTextSize(3);
    display.print(String(CPU_SCORE));

    display.setCursor(60, 30);
    display.setTextColor(GREEN);
    display.setTextSize(3);
    display.print(String(PLAYER_SCORE));

    delay(2000);
    */
    while (digitalRead(UP_BUTTON) == HIGH && digitalRead(DOWN_BUTTON) == HIGH)
    {
        delay(100);
    }
    gameIsRunning = true;

    CPU_SCORE = PLAYER_SCORE = 0;

    unsigned long start = millis();
    matrix.fillScreen(LED_BLACK);
    drawCourt();
    while (millis() - start < 2000);
    ball_update = millis();
    paddle_update = ball_update;
    gameIsRunning = true;
    resetBall = true;
}

void showScore()
{
    gameIsRunning = false;
    matrix.fillScreen(LED_BLACK);
    drawCourt();
    /*
    display.setCursor(15, 4);
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.print("Score");

    display.setCursor(20, 30);
    display.setTextColor(RED);
    display.setTextSize(3);
    display.print(String(CPU_SCORE));

    display.setCursor(60, 30);
    display.setTextColor(GREEN);
    display.setTextSize(3);
    display.print(String(PLAYER_SCORE));
    */
    delay(2000);
    unsigned long start = millis();

    matrix.fillScreen(LED_BLACK);
    drawCourt();
    while (millis() - start < 2000);
    ball_update = millis();
    paddle_update = ball_update;
    gameIsRunning = true;
    resetBall = true;
}

