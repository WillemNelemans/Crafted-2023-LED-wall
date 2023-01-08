#include "Library_Crafted_2023_LED_wall.h"
#include <SPI.h>
#include <Adafruit_MCP23X17.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

/*
  0 ← x → pixel_x
  ↑
  y
  ↓ 
  pixel_y
*/

const int panelen_x = 1;  //horizontal size
const int panelen_y = 1;  //vertical size

int pixel_x = panelen_x * 8;
int pixel_y = panelen_y * 8;

int setup_pinnumber;
int pinnumber;

bool sensor_array[panelen_x * 8][panelen_y * 8] = {false};
uint32_t led_array[panelen_x * 8][panelen_y * 8] = {false};

LED_wall muur;

Adafruit_MCP23X17 mcp_X0_Y0, mcp_X1_Y0, mcp_X2_Y0, mcp_X3_Y0, //    ← X →
                  mcp_X0_Y1, mcp_X1_Y1, mcp_X2_Y1, mcp_X3_Y1, //  ↑
                  mcp_X0_Y2, mcp_X1_Y2, mcp_X2_Y2, mcp_X3_Y2, //  Y  4x4 IO expanders
                  mcp_X0_Y3, mcp_X1_Y3, mcp_X2_Y3, mcp_X3_Y3; //  ↓

uint16_t  value_mcp_X0_Y0, value_mcp_X1_Y0, value_mcp_X2_Y0, value_mcp_X3_Y0,
          value_mcp_X0_Y1, value_mcp_X1_Y1, value_mcp_X2_Y1, value_mcp_X3_Y1,
          value_mcp_X0_Y2, value_mcp_X1_Y2, value_mcp_X2_Y2, value_mcp_X3_Y2,
          value_mcp_X0_Y3, value_mcp_X1_Y3, value_mcp_X2_Y3, value_mcp_X3_Y3;

#define LED_PIN 2
#define CS1_PIN 5   //chip select 1
#define CS2_PIN 13  //chip select 2

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(pixel_x, pixel_y, panelen_x, panelen_y, LED_PIN,
  NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_MATRIX_ZIGZAG +
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_TILE_PROGRESSIVE,
  NEO_GRBW + NEO_KHZ800);

//rainbow
double counter, range = 1000.0;

// drawing
uint32_t placeholder_color, placeholder_color_rgb, placeholder_color_rgbw = 0; 
uint32_t placeholder_led_array;
uint8_t array_red, array_green, array_blue = 0;


// PONG
const unsigned long paddle_rate = 200; // speed of paddle
const unsigned long ball_rate = 500; // speed of the ball
const uint8_t ball_size = 1;
const uint8_t player_paddle_height = 2;
const uint8_t cpu_paddle_height = 2;

uint8_t max_score = 5; // amount of points you need to score to win
uint8_t cpu_score, player_score = 0;

int8_t ball_x = pixel_x / 2;
int8_t ball_y = pixel_y / 2;
int8_t ball_dir_x = 1, ball_dir_y = 1; // start position ball and direction ball

bool gameIsRunning = true;
bool resetBall = true; 

unsigned long paddle_update;
unsigned long ball_update;

// cpu  paddle position
uint8_t cpu_x = pixel_x/2 - cpu_paddle_height/2;
const uint8_t cpu_y = 1;
// player paddle position
uint8_t player_x = pixel_x/2 - player_paddle_height/2;
const uint8_t player_y = pixel_y - 2;

int position = 0;
int count = 0;

void drawCourt(); // add to lib

// screensaver
bool reset_screensaver = true; 
const unsigned long screensaver_rate = 1000;
unsigned long screensaver_update;
int8_t screensaver_size = 2;
int8_t screensaver_x = pixel_x / 2;
int8_t screensaver_y = pixel_y / 2;
int8_t screensaver_dir_x = 1;
int8_t screensaver_dir_y = 1; // start position ball and direction ball
 
//tekst scrolling
int x    = matrix.width(); 
unsigned long previousMillis = 0;
const long interval = 100; 


void setup() {
  //serial setup and print information to Serial Monitor
  Serial.begin(115200);
  Serial.println(" ");
  Serial.println("Crafted_2023_LED_wall");
  Serial.print(panelen_x);
  Serial.print(" x ");
  Serial.print(panelen_y);
  Serial.println(" panelen");
  Serial.print(pixel_x);
  Serial.print(" x ");
  Serial.print(pixel_y);
  Serial.println(" pixels");
  delay(1000);

  muur.set_brightnessRGBW(100);
  muur.set_WhiteRedChannel(255);
  muur.set_WhiteGreenChannel(244);
  muur.set_WhiteBlueChannel(239);

  uint8_t print_brightness  = muur.get_brightnessRGBW();
  uint8_t print_WhiteRed    = muur.get_WhiteRedChannel();
  uint8_t print_WhiteGreen  = muur.get_WhiteGreenChannel();
  uint8_t print_WhiteBlue   = muur.get_WhiteBlueChannel(); 

  Serial.print("Brightness: ");
  Serial.println(print_brightness);
  Serial.print("WhiteRed: ");
  Serial.println(print_WhiteRed);
  Serial.print("WhiteGreen: ");
  Serial.println(print_WhiteGreen);
  Serial.print("WhiteBlue: ");
  Serial.println(print_WhiteBlue);

  // sets alle te values inside the sensor/LED_array's to 0 / false 
  for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
      sensor_array[colom][row] = false; 
      led_array[colom][row] = 0; 
    }
  }

  //matrix setup
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setPassThruColor();
  matrix.fillScreen(0); //should make turn of all the led if they were on
  matrix.show();


  for(int setup_color = 0; setup_color < 5; setup_color++){
    if (setup_color == 0) placeholder_color = muur.RGB_to_RGBW(255, 255, 255);
    if (setup_color == 1) placeholder_color = muur.RGB_to_RGBW(255, 0, 0);
    if (setup_color == 2) placeholder_color = muur.RGB_to_RGBW(0, 255, 0);
    if (setup_color == 3) placeholder_color = muur.RGB_to_RGBW(0, 0, 255);
    if (setup_color == 4) placeholder_color = 0;
    matrix.setPassThruColor(placeholder_color);
    matrix.fillScreen(0); 
    matrix.show();
    delay(1000);
  }

  // setup the IO expanders
  // panel 1
  if (!mcp_X0_Y0.begin_SPI(CS1_PIN, &SPI, 0x20)) {
    Serial.println("Error. mcp_X0_Y0");
    while (1);
  } mcp_X0_Y0.enableAddrPins();

  if (!mcp_X1_Y0.begin_SPI(CS1_PIN, &SPI, 0x21)) {
    Serial.println("Error. mcp_X1_Y0");
    while (1);
  } mcp_X1_Y0.enableAddrPins();

  if (!mcp_X0_Y1.begin_SPI(CS1_PIN, &SPI, 0x22)) {
    Serial.println("Error. mcp_X0_Y1");
    while (1);
  } mcp_X0_Y1.enableAddrPins();

  if (!mcp_X1_Y1.begin_SPI(CS1_PIN, &SPI, 0x23)) {
    Serial.println("Error. mcp_X1_Y1");
    while (1);
  } mcp_X1_Y1.enableAddrPins();

  // panel 2
  if (!mcp_X0_Y2.begin_SPI(CS1_PIN, &SPI, 0x24)) {
    Serial.println("Error. mcp_X0_Y2");
    while (1);
  } mcp_X0_Y2.enableAddrPins();

  if (!mcp_X1_Y2.begin_SPI(CS1_PIN, &SPI, 0x25)) {
    Serial.println("Error. mcp_X1_Y2");
    while (1);
  } mcp_X1_Y2.enableAddrPins();

  if (!mcp_X0_Y3.begin_SPI(CS1_PIN, &SPI, 0x26)) {
    Serial.println("Error. mcp_X0_Y3");
    while (1);
  } mcp_X0_Y3.enableAddrPins();

  if (!mcp_X1_Y3.begin_SPI(CS1_PIN, &SPI, 0x27)) {
    Serial.println("Error. mcp_X1_Y3");
    while (1);
  } mcp_X1_Y3.enableAddrPins();

  // panel 3
  if (!mcp_X2_Y0.begin_SPI(CS2_PIN, &SPI, 0x20)) {
    Serial.println("Error. mcp_X2_Y0");
    while (1);
  } mcp_X2_Y0.enableAddrPins();

  if (!mcp_X3_Y0.begin_SPI(CS2_PIN, &SPI, 0x21)) {
    Serial.println("Error. mcp_X3_Y0");
    while (1);
  } mcp_X3_Y0.enableAddrPins();

  if (!mcp_X2_Y1.begin_SPI(CS2_PIN, &SPI, 0x22)) {
    Serial.println("Error. mcp_X2_Y1");
    while (1);
  } mcp_X2_Y1.enableAddrPins();

  if (!mcp_X3_Y1.begin_SPI(CS2_PIN, &SPI, 0x23)) {
    Serial.println("Error. mcp_X3_Y1");
    while (1);
  } mcp_X3_Y1.enableAddrPins();

  // panel 4
  if (!mcp_X2_Y2.begin_SPI(CS2_PIN, &SPI, 0x24)) {
    Serial.println("Error. mcp_X2_Y2");
    while (1);
  } mcp_X2_Y2.enableAddrPins();

  if (!mcp_X3_Y2.begin_SPI(CS2_PIN, &SPI, 0x25)) {
    Serial.println("Error. mcp_X3_Y2");
    while (1);
  } mcp_X3_Y2.enableAddrPins();

  if (!mcp_X2_Y3.begin_SPI(CS2_PIN, &SPI, 0x26)) {
    Serial.println("Error. mcp_X2_Y3");
    while (1);
  } mcp_X2_Y3.enableAddrPins();

  if (!mcp_X3_Y3.begin_SPI(CS2_PIN, &SPI, 0x27)) {
    Serial.println("Error. mcp_X3_Y3");
    while (1);
  } mcp_X3_Y3.enableAddrPins();

  // define al the pins of the IO expanders as inputs 
  for(int config_pinnumber = 0; config_pinnumber > 16; config_pinnumber++){
    //CS 1
    mcp_X0_Y0.pinMode(config_pinnumber, INPUT); 
    mcp_X1_Y0.pinMode(config_pinnumber, INPUT);
    mcp_X0_Y1.pinMode(config_pinnumber, INPUT);
    mcp_X1_Y1.pinMode(config_pinnumber, INPUT);
    mcp_X0_Y2.pinMode(config_pinnumber, INPUT); 
    mcp_X1_Y2.pinMode(config_pinnumber, INPUT);
    mcp_X0_Y3.pinMode(config_pinnumber, INPUT);
    mcp_X1_Y3.pinMode(config_pinnumber, INPUT);    
    //CS 2
    mcp_X2_Y0.pinMode(config_pinnumber, INPUT); 
    mcp_X3_Y0.pinMode(config_pinnumber, INPUT);
    mcp_X2_Y1.pinMode(config_pinnumber, INPUT);
    mcp_X3_Y1.pinMode(config_pinnumber, INPUT);
    mcp_X2_Y2.pinMode(config_pinnumber, INPUT); 
    mcp_X3_Y2.pinMode(config_pinnumber, INPUT);
    mcp_X2_Y3.pinMode(config_pinnumber, INPUT);
    mcp_X3_Y3.pinMode(config_pinnumber, INPUT);    
  }

  // setup PONG
  unsigned long start = millis();

  //drawCourt();
  while (millis() - start < 2000);
  ball_update = millis();
  paddle_update = ball_update;

  Serial.println("setup done");
}


void loop() {
  value_mcp_X0_Y0 = mcp_X0_Y0.readGPIOAB(); // paneel 1
  value_mcp_X1_Y0 = mcp_X1_Y0.readGPIOAB();
  value_mcp_X0_Y1 = mcp_X0_Y1.readGPIOAB();
  value_mcp_X1_Y1 = mcp_X1_Y1.readGPIOAB();
  
  if (panelen_y >= 2) {
    value_mcp_X0_Y2 = mcp_X0_Y2.readGPIOAB(); // paneel 2
    value_mcp_X1_Y2 = mcp_X1_Y2.readGPIOAB();
    value_mcp_X0_Y3 = mcp_X0_Y3.readGPIOAB();
    value_mcp_X1_Y3 = mcp_X1_Y3.readGPIOAB();
  }

  if (panelen_x >= 2) {
    value_mcp_X2_Y0 = mcp_X0_Y0.readGPIOAB(); // paneel 3
    value_mcp_X3_Y0 = mcp_X1_Y0.readGPIOAB();
    value_mcp_X2_Y1 = mcp_X0_Y1.readGPIOAB();
    value_mcp_X3_Y1 = mcp_X1_Y1.readGPIOAB();
  }

  if (panelen_x >= 2 && panelen_y >= 2) {
    value_mcp_X2_Y2 = mcp_X0_Y2.readGPIOAB(); // paneel 4
    value_mcp_X3_Y2 = mcp_X1_Y2.readGPIOAB();
    value_mcp_X2_Y3 = mcp_X0_Y3.readGPIOAB();
    value_mcp_X3_Y3 = mcp_X1_Y3.readGPIOAB();
  }

  for (int row = 0; row < pixel_y; row++) {
    for (int colom = 0; colom < pixel_x; colom++) {
      int row_type = row % 4;
      // maybe make this a switch case statment you know what i mean but maybee its not that importand idk
      if (row_type == 0) pinnumber = 9 + (2 * (colom % 4));
      if (row_type == 1) pinnumber = 8 + (2 * (colom % 4));
      if (row_type == 2) pinnumber = 7 - (2 * (colom % 4));
      if (row_type == 3) pinnumber = 6 - (2 * (colom % 4));

      if      ((row < 4) && (colom < 4  ))  sensor_array[colom][row] = ((value_mcp_X0_Y0 >> pinnumber) & 1);  // X0 Y0
      else if ((row < 4) && (colom < 8  ))  sensor_array[colom][row] = ((value_mcp_X1_Y0 >> pinnumber) & 1);  // X Y
      else if ((row < 4) && (colom < 12 ))  sensor_array[colom][row] = ((value_mcp_X2_Y0 >> pinnumber) & 1);  // X Y
      else if ((row < 4) && (colom < 16 ))  sensor_array[colom][row] = ((value_mcp_X3_Y0 >> pinnumber) & 1);  // X Y

      else if ((row < 8) && (colom < 4  ))  sensor_array[colom][row] = ((value_mcp_X0_Y1 >> pinnumber) & 1);  // X Y
      else if ((row < 8) && (colom < 8  ))  sensor_array[colom][row] = ((value_mcp_X1_Y1 >> pinnumber) & 1);  // X1 Y1
      else if ((row < 8) && (colom < 12 ))  sensor_array[colom][row] = ((value_mcp_X2_Y1 >> pinnumber) & 1);  // X Y
      else if ((row < 8) && (colom < 16 ))  sensor_array[colom][row] = ((value_mcp_X3_Y1 >> pinnumber) & 1);  // X Y

      else if ((row < 12) && (colom < 4  )) sensor_array[colom][row] = ((value_mcp_X0_Y2 >> pinnumber) & 1);  // X Y
      else if ((row < 12) && (colom < 8  )) sensor_array[colom][row] = ((value_mcp_X1_Y2 >> pinnumber) & 1);  // X Y
      else if ((row < 12) && (colom < 12 )) sensor_array[colom][row] = ((value_mcp_X2_Y2 >> pinnumber) & 1);  // X2 Y2
      else if ((row < 12) && (colom < 16 )) sensor_array[colom][row] = ((value_mcp_X3_Y2 >> pinnumber) & 1);  // X Y

      else if ((row < 16) && (colom < 4  )) sensor_array[colom][row] = ((value_mcp_X0_Y3 >> pinnumber) & 1);  // X Y
      else if ((row < 16) && (colom < 8  )) sensor_array[colom][row] = ((value_mcp_X1_Y3 >> pinnumber) & 1);  // X Y
      else if ((row < 16) && (colom < 12 )) sensor_array[colom][row] = ((value_mcp_X2_Y3 >> pinnumber) & 1);  // X Y
      else if ((row < 16) && (colom < 16 )) sensor_array[colom][row] = ((value_mcp_X3_Y3 >> pinnumber) & 1);  // X3 Y3
    }
  }

  // for rainbow
  counter++;
  if (counter >= 1000) counter = 0;

//line test
/*
  for (int line = 0; line < 8; line++){
    placeholder_color = muur.RGB_to_RGBW(0, 255, 0);
    matrix.setPassThruColor(placeholder_color);
    matrix.drawLine(0, 0, (pixel_x -1), line, 0);
    matrix.show();
    delay(500);
    matrix.setPassThruColor();
    matrix.drawLine(0, 0, (pixel_x -1), line, 0);
    matrix.show();
  }
  for (int line = 8; line > 0; line--){
    placeholder_color = muur.RGB_to_RGBW(0, 0, 255);
    matrix.setPassThruColor(placeholder_color);
    matrix.drawLine(0, 0, (line - 1), (pixel_y -1), 0);
    matrix.show();
    delay(500);
    matrix.setPassThruColor();
    matrix.drawLine(0, 0, (line - 1), (pixel_y -1), 0);
    matrix.show();
  }
*/
  
//drawing
/*
  for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
      if(sensor_array[colom][row]) { // if the value is true
        led_array[colom][row] = muur.Rainbow(counter / range);
        placeholder_color_rgbw = muur.RGB_to_RGBW(led_array[colom][row]);
        matrix.setPassThruColor(placeholder_color_rgbw);
      } else {
        placeholder_led_array = led_array[colom][row];
        array_red = (uint8_t)placeholder_led_array;
        array_green = (uint8_t)(placeholder_led_array >> 8);
        array_blue = (uint8_t)(placeholder_led_array >> 16);

        if (array_red > 1 || array_green > 1 || array_blue > 1) {
          if (array_red   > 1)  array_red--;
          if (array_green > 1)  array_green--;
          if (array_blue  > 1)  array_blue--;
          led_array[colom][row] = array_red + (array_green << 8) + (array_blue << 16);
          placeholder_color_rgbw = muur.RGB_to_RGBW(led_array[colom][row]);
          matrix.setPassThruColor(placeholder_color_rgbw);
        } else{
          led_array[colom][row] = 0;
          matrix.setPassThruColor(led_array[colom][row]);
        }
      }
      matrix.drawPixel(colom, row, 0);
    }
  }
*/

// tekst
/*
  unsigned long currentMillis = millis(); 
  counter++;
  if (counter >= 1000) counter = 0;


  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    matrix.setPassThruColor();
    matrix.fillScreen(0);

    placeholder_color_rgb = muur.Rainbow(counter / range);
    placeholder_color_rgbw = muur.RGB_to_RGBW(placeholder_color_rgb);
    matrix.setPassThruColor(placeholder_color_rgbw);

    matrix.setCursor(x, 0);
    matrix.print(F("TEST"));
    if(--x < -36) {
      x = matrix.width();
      matrix.setTextColor(0); // 0 is placeholder
    }
  }
*/

// PONG
/*
  unsigned long time = millis();

  if (resetBall) { // if resetball is true than reset its position
    matrix.setPassThruColor();
    matrix.drawRect(ball_x, ball_y, ball_size, ball_size, 0);   // earases the old ball
    ball_x = pixel_x / 2;
    ball_y = pixel_y / 2;
    if (ball_dir_x == 0) ball_dir_x = random(-1, 1);
    if (ball_dir_y == 0) ball_dir_y = random(-1, 1);

    resetBall = false;
  }

  if (time > ball_update && gameIsRunning) {
    int8_t new_x = ball_x + ball_dir_x;
    int8_t new_y = ball_y + ball_dir_y;

    // Check if the player scored
    if (new_y < 0) { // if ball is at ball position is at y = 0 its in the goal
      player_score++;

      placeholder_color = muur.RGB_to_RGBW(0, 255, 0);
      matrix.setPassThruColor(placeholder_color);
      matrix.fillScreen(0);
      if (player_score == max_score) {
        gameOver();
      } else {
        showScore();
      }
    }

    // Check if the cpu scored
    if (new_y > pixel_y) { // if ball is at ball position is at y = 7 its in the goal
      cpu_score++;

      placeholder_color = muur.RGB_to_RGBW(255, 0, 0);
      matrix.setPassThruColor(placeholder_color);
      matrix.fillScreen(0);
      if (cpu_score == max_score) {
        gameOver();
      } else {
        showScore();
      }
    }

    // Check the ball hits the vertical walls
    if (new_x == 0 || new_x == (pixel_x - ball_size)) {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    // Check if the ball hits the cpu paddle
    if (new_y == cpu_y && new_x >= cpu_x && new_x <= cpu_x + cpu_paddle_height) {
      ball_dir_y = -ball_dir_y;
      new_y += ball_dir_y + ball_dir_y;
    }

    // check if the ball hits the player paddle
    if (new_y == (player_y - ball_size + 1) && new_x >= player_x && new_x <= player_x + player_paddle_height) {
      ball_dir_y = -ball_dir_y;
      new_y += ball_dir_y + ball_dir_y;
    }

    matrix.setPassThruColor();
    matrix.drawRect(ball_x, ball_y, ball_size, ball_size, 0);   // earases the old ball

    placeholder_color = muur.RGB_to_RGBW(0, 0, 255);
    matrix.setPassThruColor(placeholder_color);
    matrix.drawRect(new_x, new_y, ball_size, ball_size, 0);      // makes the new ball
    ball_x = new_x;
    ball_y = new_y;

    ball_update += ball_rate;
  }

  if (time > paddle_update && gameIsRunning) { // cpu paddle
    paddle_update += paddle_rate;

    matrix.setPassThruColor(0);
    matrix.drawFastHLine(cpu_x, cpu_y, cpu_paddle_height, 0);       // earases the old cpu paddle
    const uint8_t half_cpu_paddle = cpu_paddle_height >> 1; // maybeee change 1 to 2 if ball is size of 2
    if (cpu_x + half_cpu_paddle > ball_x) cpu_x -= 1;
    if (cpu_x + half_cpu_paddle < ball_x) cpu_x += 1;
    if (cpu_x < 1) cpu_x = 1;
    if (cpu_x + cpu_paddle_height > (pixel_x-1) ) cpu_x = (pixel_x-1) - cpu_paddle_height;
    placeholder_color = muur.RGB_to_RGBW(255, 0, 0);
    matrix.setPassThruColor(placeholder_color);
    matrix.drawFastHLine(cpu_x, cpu_y, cpu_paddle_height, 0);
  }

  if (gameIsRunning) { // player paddle
    matrix.setPassThruColor();
    matrix.drawFastHLine(player_x, player_y, player_paddle_height, 0);

    position = 0;
    count = 0;
    for (int colom = 0; colom < pixel_x; colom++) {
      if (sensor_array[colom][6]) {
        position = position + colom;
        count++;
      }
    }
    if (count > 0) player_x = position/count;
    if (player_x < 1) player_x = 1;
    if (player_x > (pixel_x - (player_paddle_height +1))) player_x = (pixel_x - (player_paddle_height +1));
    placeholder_color = muur.RGB_to_RGBW(0, 255, 0);
    matrix.setPassThruColor(placeholder_color);
    matrix.drawFastHLine(player_x, player_y, player_paddle_height, 0);
  }
  drawCourt();
*/

// screensaver

  unsigned long time = millis();
  if (sensor_array[0][6]) reset_screensaver = true;

  if (reset_screensaver) {
    matrix.setPassThruColor();
    matrix.fillScreen(0);
    screensaver_x = random(0, (pixel_x - 1));
    screensaver_y = random(0, (pixel_y - 1));
    screensaver_dir_x = random(-2, 2);
    screensaver_dir_y = random(-2, 2);

    if (screensaver_dir_x == 0) screensaver_dir_x = 1; //cant have it be 0 but want it random
    if (screensaver_dir_y == 0) screensaver_dir_y = 1;

    reset_screensaver = false;
  }

  if (time > screensaver_update) {
    int8_t new_screensaver_x = screensaver_x + screensaver_dir_x;
    int8_t new_screensaver_y = screensaver_y + screensaver_dir_y;

    // Check the screensaver hits the vertical walls
    if (screensaver_x <= 0 || screensaver_x >= (pixel_x - screensaver_size)) {
      screensaver_dir_x = -screensaver_dir_x;
      new_screensaver_x += screensaver_dir_x + screensaver_dir_x;
    }

    // Check the screensaver hits the horizontal walls
    if (screensaver_y <= 0 || screensaver_y >= (pixel_y - screensaver_size)) {
      screensaver_dir_y = -screensaver_dir_y;
      new_screensaver_y += screensaver_dir_y + screensaver_dir_y;
    }

    matrix.setPassThruColor();
    matrix.drawRect(screensaver_x, screensaver_y, screensaver_size, screensaver_size, 0);   // earases the old ball

    placeholder_color_rgb = muur.Rainbow(counter / range);
    placeholder_color_rgbw = muur.RGB_to_RGBW(placeholder_color_rgb);
    matrix.setPassThruColor(placeholder_color_rgbw);
    matrix.drawRect(screensaver_x, screensaver_y, screensaver_size, screensaver_size, 0);      // makes the new ball
    screensaver_x = new_screensaver_x;
    screensaver_y = new_screensaver_y;

    screensaver_update += screensaver_rate;
  }


  matrix.show();
  delay(10);
}

void drawCourt() {
  placeholder_color = muur.RGB_to_RGBW(255, 255, 255);
  matrix.setPassThruColor(placeholder_color);

  matrix.drawFastVLine(0, 0, pixel_y, 0);
  matrix.drawFastVLine(7, 0, pixel_y, 0);
  matrix.show();
}

void gameOver()
{
  gameIsRunning = false;
  matrix.setPassThruColor();
  matrix.fillScreen(0);
  drawCourt();

  delay(5000);
  gameIsRunning = true;

  cpu_score = player_score = 0;

  unsigned long start = millis();
  matrix.fillScreen(0);
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
  drawCourt();

  delay(2000);
  unsigned long start = millis();

  matrix.setPassThruColor();
  matrix.fillScreen(0);
  drawCourt();
  while (millis() - start < 2000);
  ball_update = millis();
  paddle_update = ball_update;
  gameIsRunning = true;
  resetBall = true;
}
