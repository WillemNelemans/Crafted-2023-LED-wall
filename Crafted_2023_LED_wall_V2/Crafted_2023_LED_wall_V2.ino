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

  const int panelen_x = 1;     // horizontal size
  const int panelen_y = 1;     // vertical size
  const int pixels_matrix = 8; // how much pixels the matric has horizontal/vertical

  int pixel_x = panelen_x * pixels_matrix; // so for our setup we have 8 pixels per panel and 2 panels so 16 pixel in total
  int pixel_y = panelen_y * pixels_matrix;

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

  #define LED_PIN   2
  #define CS1_PIN   5  //chip select 1
  #define CLK_PIN   18
  #define MISO_PIN  19
  #define MOSI_PIN  23


  Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, panelen_x, panelen_y, LED_PIN,
    NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_COLUMNS   + NEO_MATRIX_ZIGZAG +
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_TILE_PROGRESSIVE,
    NEO_GRBW + NEO_KHZ800);

  //rainbow
  double counter, range = 1000.0;

  // drawing
  uint32_t placeholder_color, placeholder_color_rgb, placeholder_color_rgbw = 0; 
  uint32_t placeholder_led_array;
  uint8_t array_red, array_green, array_blue = 0;

  void setup() {
    //serial setup and print information to Serial Monitor
    //Serial.begin(9600);
    Serial.begin(115200); //esp32
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

    muur.set_brightnessRGBW(255);
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

  // define al the pins of the IO expanders as inputs 
  for(int config_pinnumber = 0; config_pinnumber > 16; config_pinnumber++){
    mcp_X0_Y0.pinMode(config_pinnumber, INPUT); 
    mcp_X1_Y0.pinMode(config_pinnumber, INPUT);
    mcp_X0_Y1.pinMode(config_pinnumber, INPUT);
    mcp_X1_Y1.pinMode(config_pinnumber, INPUT);
  }

  Serial.println("setup done");
}


void loop() {
  value_mcp_X0_Y0 = mcp_X0_Y0.readGPIOAB(); // paneel 1
  value_mcp_X1_Y0 = mcp_X1_Y0.readGPIOAB();
  value_mcp_X0_Y1 = mcp_X0_Y1.readGPIOAB();
  value_mcp_X1_Y1 = mcp_X1_Y1.readGPIOAB();

  for (int row = 0; row < pixel_y; row++) {
    for (int colom = 0; colom < pixel_x; colom++) {
      int row_type = row % 4;
      if (row_type == 0) pinnumber = 9 + (2 * (colom % 4));
      if (row_type == 1) pinnumber = 8 + (2 * (colom % 4));
      if (row_type == 2) pinnumber = 7 - (2 * (colom % 4));
      if (row_type == 3) pinnumber = 6 - (2 * (colom % 4));

      if      ((row < 4) && (colom < 4  ))  sensor_array[colom][row] = ((value_mcp_X0_Y0 >> pinnumber) & 1);  // X0 Y0
      else if ((row < 4) && (colom < 8  ))  sensor_array[colom][row] = ((value_mcp_X1_Y0 >> pinnumber) & 1);  // X1 Y0
      else if ((row < 8) && (colom < 4  ))  sensor_array[colom][row] = ((value_mcp_X0_Y1 >> pinnumber) & 1);  // X0 Y1
      else if ((row < 8) && (colom < 8  ))  sensor_array[colom][row] = ((value_mcp_X1_Y1 >> pinnumber) & 1);  // X1 Y1
    }
  }

  // for rainbow
  counter++;
  if (counter >= 1000) counter = 0;


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

  matrix.show();
  delay(10);
}

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
/*
  if (!mcp_X0_Y0.begin_SPI(CS1_PIN, CLK_PIN, MISO_PIN, MOSI_PIN, 0)) {
    Serial.println("Error. mcp_X0_Y0");
    while (1);
  } mcp_X0_Y0.enableAddrPins();

  if (!mcp_X1_Y0.begin_SPI(CS1_PIN, CLK_PIN, MISO_PIN, MOSI_PIN, 1)) {
    Serial.println("Error. mcp_X1_Y0");
    while (1);
  } mcp_X1_Y0.enableAddrPins();

  if (!mcp_X0_Y1.begin_SPI(CS1_PIN, CLK_PIN, MISO_PIN, MOSI_PIN, 2)) {
    Serial.println("Error. mcp_X0_Y1");
    while (1);
  } mcp_X0_Y1.enableAddrPins();

  if (!mcp_X1_Y1.begin_SPI(CS1_PIN, CLK_PIN, MISO_PIN, MOSI_PIN, 3)) {
    Serial.println("Error. mcp_X1_Y1");
    while (1);
  } mcp_X1_Y1.enableAddrPins();*/
