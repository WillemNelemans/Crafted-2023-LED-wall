#include <SPI.h>
#include <Adafruit_MCP23X17.h>

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

const int panelen_x = 1;  //horizontal size
const int panelen_y = 1;  //vertical size

int pixel_x = panelen_x * 8;
int pixel_y = panelen_y * 8;

bool sensor_array[panelen_x * 8][panelen_y * 8] = {false};

uint16_t value_mcp_A0, value_mcp_A1, value_mcp_A2, value_mcp_A3;
int pinnumber;

Adafruit_MCP23X17 mcp_A0;
Adafruit_MCP23X17 mcp_A1;
Adafruit_MCP23X17 mcp_A2;
Adafruit_MCP23X17 mcp_A3;

#define CS1_PIN 5 //chip select 1

#define LED_PIN 8
#define BRIGHTNESS 25 // Max is 255, 25 is a conservative value to not overload 
//bightness doesnt work in with the functions that are used because but maybe it does matter idk cant test it 

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(pixel_x, pixel_y, LED_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_TILE_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRBW            + NEO_KHZ800);

// MATRIX DECLARATION:
  // Parameter 1 = width of EACH NEOPIXEL MATRIX (not total display)
  // Parameter 2 = height of each matrix
  // Parameter 3 = number of matrices arranged horizontally
  // Parameter 4 = number of matrices arranged vertically
  // Parameter 5 = pin number (most are valid)
  // Parameter 6 = matrix layout flags, add together as needed:
  //   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
  //     Position of the FIRST LED in the FIRST MATRIX; pick two, e.g.
  //     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
  //   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs WITHIN EACH MATRIX are
  //     arranged in horizontal rows or in vertical columns, respectively;
  //     pick one or the other.
  //   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns WITHIN
  //     EACH MATRIX proceed in the same order, or alternate lines reverse
  //     direction; pick one.
  //   NEO_TILE_TOP, NEO_TILE_BOTTOM, NEO_TILE_LEFT, NEO_TILE_RIGHT:
  //     Position of the FIRST MATRIX (tile) in the OVERALL DISPLAY; pick
  //     two, e.g. NEO_TILE_TOP + NEO_TILE_LEFT for the top-left corner.
  //   NEO_TILE_ROWS, NEO_TILE_COLUMNS: the matrices in the OVERALL DISPLAY
  //     are arranged in horizontal rows or in vertical columns, respectively;
  //     pick one or the other.
  //   NEO_TILE_PROGRESSIVE, NEO_TILE_ZIGZAG: the ROWS/COLUMS OF MATRICES
  //     (tiles) in the OVERALL DISPLAY proceed in the same order for every
  //     line, or alternate lines reverse direction; pick one.  When using
  //     zig-zag order, the orientation of the matrices in alternate rows
  //     will be rotated 180 degrees (this is normal -- simplifies wiring).
  //   See example below for these values in action.
  // Parameter 7 = pixel type flags, add together as needed:
  //   NEO_RGB     Pixels are wired for RGB bitstream (v1 pixels)
  //   NEO_GRB     Pixels are wired for GRB bitstream (v2 pixels)
  //   NEO_KHZ400  400 KHz bitstream (e.g. FLORA v1 pixels)
  //   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)

  // Example with three 10x8 matrices (created using NeoPixel flex strip --
  // these grids are not a ready-made product).  In this application we'd
  // like to arrange the three matrices side-by-side in a wide display.
  // The first matrix (tile) will be at the left, and the first pixel within
  // that matrix is at the top left.  The matrices use zig-zag line ordering.
  // There's only one row here, so it doesn't matter if we declare it in row
  // or column order.  The matrices use 800 KHz (v2) pixels that expect GRB
  // color data.



//

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("test voor PCB na solderen.");
  delay(1000);

// terminal
  Serial.print(pixel_x);
  Serial.print(" x ");
  Serial.print(pixel_y);
  Serial.println(" raster");

// sets alle te values to low inside the array
  for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
       sensor_array[colom][row] = false; 
    }
  }

// print the value of the array to therminal. should all be zero's
  for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
      Serial.print(sensor_array[colom][row]);
    }
    Serial.println(" ");
  } 

// rij 1
  if (!mcp_A0.begin_SPI(CS1_PIN, &SPI, 0x20)) {
    Serial.println("Error. mcp_A0");
    while (1);
  }
  mcp_A0.enableAddrPins();

  if (!mcp_A1.begin_SPI(CS1_PIN, &SPI, 0x21)) {
    Serial.println("Error. mcp_A1");
    while (1);
  }
  mcp_A1.enableAddrPins();

  if (!mcp_A2.begin_SPI(CS1_PIN, &SPI, 0x22)) {
    Serial.println("Error. mcp_A2");
    while (1);
  }
  mcp_A2.enableAddrPins();
  
  if (!mcp_A3.begin_SPI(CS1_PIN, &SPI, 0x23)) {
    Serial.println("Error. mcp_A3");
    while (1);
  }
  mcp_A3.enableAddrPins(); 

// maakt allen pinnen van de mcp's inputs 
  for(int config_pinnumber = 0; config_pinnumber > 16; config_pinnumber++){
    mcp_A0.pinMode(config_pinnumber, INPUT); //cs 1
    mcp_A1.pinMode(config_pinnumber, INPUT);
    mcp_A2.pinMode(config_pinnumber, INPUT);
    mcp_A3.pinMode(config_pinnumber, INPUT);
  }

// led
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(BRIGHTNESS);
  matrix.fillScreen(0); //should make turn of all the led if they were on

// looping
  delay(1000);
  Serial.println("Looping...");
}



void loop() {
  // put your main code here, to run repeatedly:
//reading all the inputs of the external IO and writing them to a uint16_t
  uint16_t value_mcp_A0 = mcp_A0.readGPIOAB();
  uint16_t value_mcp_A1 = mcp_A1.readGPIOAB();
  uint16_t value_mcp_A2 = mcp_A2.readGPIOAB();
  uint16_t value_mcp_A3 = mcp_A3.readGPIOAB();

  for (int row = 0; row < pixel_y; row++) {
    for (int colom = 0; colom < pixel_x; colom++) {
      int row_type = row % 4;
      if (row_type == 0) {
        pinnumber = 9 + (2 * (colom % 4));
      }
      if (row_type == 1) {
        pinnumber = 8 + (2 * (colom % 4));
      }
      if (row_type == 2) {
        pinnumber = 7 - (2 * (colom % 4));
      }
      if (row_type == 3) {
        pinnumber = 6 - (2 * (colom % 4));
      }

      if ((row < 4) && (colom < 4)) {
        sensor_array[colom][row] = ((value_mcp_A0 >> pinnumber) & 1);
      } else if ((row < 4) && (colom < 8)) {
        sensor_array[colom][row] = ((value_mcp_A1 >> pinnumber) & 1);
      }else if ((row < 8) && (colom < 4)) {
        sensor_array[colom][row] = ((value_mcp_A2 >> pinnumber) & 1);
      } else if ((row < 8) && (colom < 8)) {
        sensor_array[colom][row] = ((value_mcp_A3 >> pinnumber) & 1);
      }
    }
  }

  for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
      Serial.print(sensor_array[colom][row]);
    }
    Serial.println(" ");
  }

  for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
      if(sensor_array[colom][row] == false){
        matrix.drawPixel(colom, row, 0); // turn OFF led if array value is LOW
      } else {
        matrix.drawPixel(colom, row, 6243); // turn ON led if array value is HIGH
      }
    }
  }
}
