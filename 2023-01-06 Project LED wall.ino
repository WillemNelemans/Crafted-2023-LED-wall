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

Adafruit_MCP23X17 mcp_X0_Y0, mcp_X1_Y0, mcp_X2_Y0, mcp_X3_Y0, //    ← X →
                  mcp_X0_Y1, mcp_X1_Y1, mcp_X2_Y1, mcp_X3_Y1, //  ↑
                  mcp_X0_Y2, mcp_X1_Y2, mcp_X2_Y2, mcp_X3_Y2, //  Y  4x4 panelen
                  mcp_X0_Y3, mcp_X1_Y3, mcp_X2_Y3, mcp_X3_Y3; //  ↓

uint16_t  value_mcp_X0_Y0, value_mcp_X1_Y0, value_mcp_X2_Y0, value_mcp_X3_Y0,
          value_mcp_X0_Y1, value_mcp_X1_Y1, value_mcp_X2_Y1, value_mcp_X3_Y1,
          value_mcp_X0_Y2, value_mcp_X1_Y2, value_mcp_X2_Y2, value_mcp_X3_Y2,
          value_mcp_X0_Y3, value_mcp_X1_Y3, value_mcp_X2_Y3, value_mcp_X3_Y3;

double counter, range = 1000.0;
uint32_t placeholder_color, placeholder_color_rgb, placeholder_color_rgbw = 0; 
uint32_t placeholder_led_array;
uint8_t array_red, array_green, array_blue = 0;

#define LED_PIN 2
#define CS1_PIN 5 //chip select 1
#define CS2_PIN 13 //chip select 2
uint8_t LED_brightness = 255;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(pixel_x, pixel_y, panelen_x, panelen_y, LED_PIN,
  NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_MATRIX_ZIGZAG +
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_TILE_PROGRESSIVE,
  NEO_GRBW + NEO_KHZ800);

// PONG
const unsigned long paddle_rate = 10; // speed of paddle
const unsigned long ball_rate = 500; // speed of the ball
const uint8_t ball_size = 1;
const uint8_t player_paddle_height = 2;
const uint8_t cpu_paddle_height = 2;

uint8_t max_score = 5; // amount of points you need to score to win
uint8_t cpu_score, player_score = 0;

uint8_t ball_x = 3, ball_y = 3; // start position ball
uint8_t ball_dir_x = 1, ball_dir_y = 1; // start direction ball

boolean gameIsRunning = true;
boolean resetBall = true; // was false now reset ball on startup

unsigned long paddle_update, ball_update;
  
uint8_t cpu_x = 3; //cpu paddle position
const uint8_t cpu_y = 1;

uint8_t player_x = 3; //cpu paddle position
const uint8_t player_y = pixel_y - 2;

int position = 0;
int count = 0;

// to make white true white because of color temperature
// https://andi-siess.de/rgb-to-color-temperature/
const uint8_t kWhiteRedChannel    = 255;
const uint8_t kWhiteGreenChannel  = 244;
const uint8_t kWhiteBlueChannel   = 239;

uint32_t Rainbow(double ratio);
uint32_t RGB_to_RGBW(uint32_t color, uint8_t brightness);
// uint32_t RGB_to_RGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness); cant get this to work idk why https://www.google.com/search?q=2+functions+with+the+same+name&rlz=1C1UEAD_nlNL953NL953&oq=2+functions+with+the+same+name+&aqs=chrome..69i57.12234j0j1&sourceid=chrome&ie=UTF-8
void drawCourt();

void setup() {
  //serial setup and print information to Serial Monitor
  Serial.begin(115200);
  Serial.println("Project LED wall");
  Serial.print(panelen_x);
  Serial.print(" x ");
  Serial.print(panelen_y);
  Serial.println(" panelen");
  Serial.print(pixel_x);
  Serial.print(" x ");
  Serial.print(pixel_y);
  Serial.println(" raster");
  delay(1000);

  // sets alle te values inside the sensor/LED_array's to 0 / false 
  for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
       sensor_array[colom][row] = false; 
       led_array[colom][row] = 0;
    }
  }

  //matrix setup
  matrix.begin();
  matrix.setPassThruColor();
  matrix.fillScreen(0); //should make turn of all the led if they were on
  matrix.show();

  // test all the LEDS and go to setup brightness
  for(int setup_color = 0; setup_color < 5; setup_color++){
    for(int setup_brightness = 0; setup_brightness < LED_brightness; setup_brightness++){
      if (setup_color == 0) placeholder_color = RGB_to_RGBW(4278190080, setup_brightness);
      if (setup_color == 1) placeholder_color = RGB_to_RGBW(16711680, setup_brightness);
      if (setup_color == 2) placeholder_color = RGB_to_RGBW(65280, setup_brightness);
      if (setup_color == 3) placeholder_color = RGB_to_RGBW(255, setup_brightness);
      if (setup_color == 4) placeholder_color = 0;
      matrix.setPassThruColor(placeholder_color);
      matrix.fillScreen(0); 
      matrix.show();
      delay(10);
    }
    delay(10);
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
//
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
      // maybe make this a switch case statment you know what i mean
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

  counter++;
  if (counter >= 1000) counter = 0;

  for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
      if(sensor_array[colom][row]) { // if the value is true
        placeholder_color_rgb = Rainbow(counter / range);
        led_array[colom][row] = RGB_to_RGBW(placeholder_color_rgb, LED_brightness);
        matrix.setPassThruColor(led_array[colom][row]);
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
        } else{
          led_array[colom][row] = 0;
        }
        matrix.setPassThruColor(led_array[colom][row]);
      }
      matrix.drawPixel(colom, row, 0);
    }
  }

  matrix.show();
  delay(10);
}


uint32_t Rainbow(double ratio) {
  //we want to normalize ratio so that it fits in to 6 regions
  //where each region is 256 units long
  int normalized = int(ratio * 256 * 6);

  //find the region for this position
  int region = normalized / 256;

  //find the distance to the start of the closest region
  int x = normalized % 256;

  uint8_t r = 0, g = 0, b = 0;
  switch (region) {
  case 0: r = 255; g = 0;   b = 0;   g += x; break;
  case 1: r = 255; g = 255; b = 0;   r -= x; break;
  case 2: r = 0;   g = 255; b = 0;   b += x; break;
  case 3: r = 0;   g = 255; b = 255; g -= x; break;
  case 4: r = 0;   g = 0;   b = 255; r += x; break;
  case 5: r = 255; g = 0;   b = 255; b -= x; break;
  }
  return r + (g << 8) + (b << 16);
}

uint32_t RGB_to_RGBW(uint32_t color, uint8_t brightness) {
  uint8_t r = (uint8_t)color;
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)(color >> 16);

  // i looked at the neopixel library and they add +1 to bightness https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.cpp
  r = (r * (brightness +1)) >> 8;
  g = (g * (brightness +1)) >> 8;
  b = (b * (brightness +1)) >> 8;


  // These values are what the 'white' value would need to
  // be to get the corresponding color value.
  double whiteValueForRed   =   r * 255.0 / kWhiteRedChannel;
  double whiteValueForGreen =   g * 255.0 / kWhiteGreenChannel;
  double whiteValueForBlue  =   b * 255.0 / kWhiteBlueChannel;

  // Set the white value to the highest it can be for the given color
  // (without over saturating any channel - thus the minimum of them).
  double minWhiteValue = min(whiteValueForRed,
                             min(whiteValueForGreen,
                                 whiteValueForBlue));
  uint8_t Wo = (minWhiteValue <= 255 ? (uint8_t) minWhiteValue : 255);

  // The rest of the channels will just be the original value minus the
  // contribution by the white channel.
  uint8_t Ro = (uint8_t)(r - minWhiteValue * kWhiteRedChannel / 255);
  uint8_t Go = (uint8_t)(g - minWhiteValue * kWhiteGreenChannel / 255);
  uint8_t Bo = (uint8_t)(b - minWhiteValue * kWhiteBlueChannel / 255);

  return Bo + (Go << 8) + (Ro << 16) + (Wo << 24);
}

/*
uint32_t RGB_to_RGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness); {
  r = (r * brightness) >> 8;
  g = (g * brightness) >> 8;
  b = (b * brightness) >> 8;

  // These values are what the 'white' value would need to
  // be to get the corresponding color value.
  double whiteValueForRed   =   r * 255.0 / kWhiteRedChannel;
  double whiteValueForGreen =   g * 255.0 / kWhiteGreenChannel;
  double whiteValueForBlue  =   b * 255.0 / kWhiteBlueChannel;

  // Set the white value to the highest it can be for the given color
  // (without over saturating any channel - thus the minimum of them).
  double minWhiteValue = min(whiteValueForRed,
                             min(whiteValueForGreen,
                                 whiteValueForBlue));
  uint8_t Wo = (minWhiteValue <= 255 ? (uint8_t) minWhiteValue : 255);

  // The rest of the channels will just be the original value minus the
  // contribution by the white channel.
  uint8_t Ro = (uint8_t)(r - minWhiteValue * kWhiteRedChannel / 255);
  uint8_t Go = (uint8_t)(g - minWhiteValue * kWhiteGreenChannel / 255);
  uint8_t Bo = (uint8_t)(b - minWhiteValue * kWhiteBlueChannel / 255);

  return Bo + (Go << 8) + (Ro << 16) + (Wo << 24);
}
*/
