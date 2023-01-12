#include <SPI.h>
#include <Adafruit_MCP23X17.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

/*
    ← x →
  ↑
  y
  ↓ 
  
*/

const int panelen_x = 1;  //horizontal size
const int panelen_y = 1;  //vertical size

int pixel_x = panelen_x * 8;
int pixel_y = panelen_y * 8;

bool sensor_array[panelen_x * 8][panelen_y * 8] = {false};
uint32_t led_array[panelen_x * 8][panelen_y * 8] = {false};

//uint16_t value_mcp_X0_Y0, value_mcp_X1_Y0, value_mcp_X0_Y1, value_mcp_X1_Y1;
int pinnumber;
double counter, range = 1000.0;
uint32_t placeholder_color_rgbw;

uint32_t placeholder_led_array;
uint8_t array_red, array_green, array_blue;

Adafruit_MCP23X17 mcp_X0_Y0;
Adafruit_MCP23X17 mcp_X1_Y0;
Adafruit_MCP23X17 mcp_X0_Y1;
Adafruit_MCP23X17 mcp_X1_Y1;

#define CS1_PIN 5 //chip select 1
#define LED_PIN 2
#define BRIGHTNESS 255

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(pixel_x, pixel_y, LED_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_TILE_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRBW            + NEO_KHZ800);

// to make white true white because of kelvin value
const uint8_t kWhiteRedChannel    = 255;
const uint8_t kWhiteGreenChannel  = 244;
const uint8_t kWhiteBlueChannel   = 239;

void setup() {
//serial setup
  Serial.begin(115200);
  Serial.println("test tekenen");
  delay(1000);

//matrix setup
  matrix.begin();
  matrix.setBrightness(BRIGHTNESS);
  matrix.setPassThruColor();
  matrix.fillScreen(0); //should make turn of all the led if they were on
  matrix.show();

// terminal
  Serial.print(pixel_x);
  Serial.print(" x ");
  Serial.print(pixel_y);
  Serial.println(" raster");

// sets alle te values to low inside the sensor and led array's
  for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
       sensor_array[colom][row] = false; 
       led_array[colom][row] = 0;
    }
  }

// setup the IO expanders
  if (!mcp_X0_Y0.begin_SPI(CS1_PIN, &SPI, 0x20)) {
    Serial.println("Error. mcp_X0_Y0");
    while (1);
  }
  mcp_X0_Y0.enableAddrPins();

  if (!mcp_X1_Y0.begin_SPI(CS1_PIN, &SPI, 0x21)) {
    Serial.println("Error. mcp_X1_Y0");
    while (1);
  }
  mcp_X1_Y0.enableAddrPins();

  if (!mcp_X0_Y1.begin_SPI(CS1_PIN, &SPI, 0x22)) {
    Serial.println("Error. mcp_X0_Y1");
    while (1);
  }
  mcp_X0_Y1.enableAddrPins();
  
  if (!mcp_X1_Y1.begin_SPI(CS1_PIN, &SPI, 0x23)) {
    Serial.println("Error. mcp_X1_Y1");
    while (1);
  }
  mcp_X1_Y1.enableAddrPins(); 

// define al the pins of the IO expanders as inputs 
  for(int config_pinnumber = 0; config_pinnumber > 16; config_pinnumber++){
    //cs 1
    mcp_X0_Y0.pinMode(config_pinnumber, INPUT); 
    mcp_X1_Y0.pinMode(config_pinnumber, INPUT);
    mcp_X0_Y1.pinMode(config_pinnumber, INPUT);
    mcp_X1_Y1.pinMode(config_pinnumber, INPUT);
  }

//test led's for al 4 of its colors
//w
  matrix.setPassThruColor(4278190080);
  matrix.fillScreen(0); 
  matrix.show();
  delay(1000);

//r
  matrix.setPassThruColor(16711680);
  matrix.fillScreen(0);
  matrix.show();
  delay(1000);

//g
  matrix.setPassThruColor(65280);
  matrix.fillScreen(0); //should make turn of all the led if they were on
  matrix.show();
  delay(1000);

//b
  matrix.setPassThruColor(255);
  matrix.fillScreen(0); 
  matrix.show();
  delay(1000);

// looping
  matrix.setPassThruColor();
  matrix.fillScreen(0); 
  Serial.println("Looping...");
  delay(1000);
}

void loop() {
  counter++;
  if (counter >= 1000) counter = 0;

  //reading all the inputs of the external IO and writing them to a uint16_t
  uint16_t value_mcp_X0_Y0 = mcp_X0_Y0.readGPIOAB();
  uint16_t value_mcp_X1_Y0 = mcp_X1_Y0.readGPIOAB();
  uint16_t value_mcp_X0_Y1 = mcp_X0_Y1.readGPIOAB();
  uint16_t value_mcp_X1_Y1 = mcp_X1_Y1.readGPIOAB();

  for (int row = 0; row < pixel_y; row++) {
    for (int colom = 0; colom < pixel_x; colom++) {
      int row_type = row % 4;
      // maybe make this a switch case statment you know what i mean
      if (row_type == 0) pinnumber = 9 + (2 * (colom % 4));
      if (row_type == 1) pinnumber = 8 + (2 * (colom % 4));
      if (row_type == 2) pinnumber = 7 - (2 * (colom % 4));
      if (row_type == 3) pinnumber = 6 - (2 * (colom % 4));

      if ((row < 4) && (colom < 4)) {
        sensor_array[colom][row] = ((value_mcp_X0_Y0 >> pinnumber) & 1);  // X0 Y0
      } else if ((row < 4) && (colom < 8)) {
        sensor_array[colom][row] = ((value_mcp_X1_Y0 >> pinnumber) & 1);  // X0 Y1
      } else if ((row < 8) && (colom < 4)) {
        sensor_array[colom][row] = ((value_mcp_X0_Y1 >> pinnumber) & 1);  // X1 Y0
      } else if ((row < 8) && (colom < 8)) {
        sensor_array[colom][row] = ((value_mcp_X1_Y1 >> pinnumber) & 1);  // X1 Y1
      }
    }
  }

  for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
      if(sensor_array[colom][row]) { // if the value of the sensor is true
        led_array[colom][row] = Rainbow(counter / range);
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
  delay(10); // without a delay it was hard to upload to the arduino
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

uint32_t RGB_to_RGBW(uint32_t color) {
  // rgb to rgbw converter
  uint8_t r = (uint8_t)color;
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)(color >> 16);


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
  for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
      if(sensor_array[colom][row]) { // if the value of the sensor is true
        matrix.setPassThruColor(1077952576);
      } else {
        matrix.setPassThruColor(0);
      }
      matrix.drawPixel(colom, row, 0);
    }
  }
*/

  /*
  uint32_t placeholder_color_rgb = Rainbow(counter / range);
  uint32_t placeholder_color_rgbw = RGB_to_RGBW(placeholder_color_rgb);
  matrix.setPassThruColor(placeholder_color_rgbw);
  matrix.fillScreen(0); //should make turn of all the led if they were on
  */

/*

  for (int row = 0; row < pixel_y; row++) {
    for (int colom = 0; colom < pixel_x; colom++) {
      if (sensor_array[colom][row]) { // if the value of the sensor is true
        led_array[colom][row] = Rainbow(counter / range);
        //placeholder_color_rgbw = RGB_to_RGBW(led_array[colom][row]);
        //matrix.setPassThruColor(placeholder_color_rgbw);
        matrix.setPassThruColor(led_array[colom][row]);
        //matrix.setPassThruColor(4026531840);
      } else { // if the value of the sensor is low
        placeholder_led_array = led_array[colom][row];
        array_red = (uint8_t)placeholder_led_array;
        placeholder_led_array = led_array[colom][row];
        array_green = (uint8_t)(placeholder_led_array >> 8);
        placeholder_led_array = led_array[colom][row];
        array_blue = (uint8_t)(placeholder_led_array >> 16);
        if(colom == 0 &&  row == 7) {
          Serial.println(placeholder_led_array);
          Serial.println(array_red);
          Serial.println(array_green);
          Serial.println(array_blue);
        }

        if ((array_red < 15) && (array_green < 15) && (array_blue < 15)) {
          
          if (array_red   >= 10)  array_red--;
          if (array_green >= 10)  array_green--;
          if (array_blue  >= 10)  array_blue--;

          placeholder_led_array = array_red + (array_green << 8) + (array_blue << 16);
          led_array[colom][row] = placeholder_led_array;
          matrix.setPassThruColor(placeholder_led_array);
          //placeholder_color_rgbw = RGB_to_RGBW(led_array[colom][row]);
          //matrix.setPassThruColor(placeholder_color_rgbw);
          
        } else {
          led_array[colom][row] = 0;
          matrix.setPassThruColor(0);
        }
      }
      matrix.drawPixel(colom, row, 0);
    }
  }
*/
