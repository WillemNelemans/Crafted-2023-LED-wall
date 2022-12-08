#include <SPI.h>
#include <Adafruit_MCP23X17.h>

const int panelen_x = 2;  //links rechts
const int panelen_y = 2;  //boven onder

int pixel_x = panelen_x * 8;
int pixel_y = panelen_y * 8;

bool sensor_array[panelen_x * 8][panelen_y * 8] = {false};

uint16_t value_mcp_A0, value_mcp_A1, value_mcp_A2, value_mcp_A3, value_mcp_A4, value_mcp_A5, value_mcp_A6, value_mcp_A7 = 0;
uint16_t value_mcp_B0, value_mcp_B1, value_mcp_B2, value_mcp_B3, value_mcp_B4, value_mcp_B5, value_mcp_B6, value_mcp_B7 = 0;

int pinnumber;

Adafruit_MCP23X17 mcp_A0;
Adafruit_MCP23X17 mcp_A1;
Adafruit_MCP23X17 mcp_A2;
Adafruit_MCP23X17 mcp_A3;
Adafruit_MCP23X17 mcp_A4;
Adafruit_MCP23X17 mcp_A5;
Adafruit_MCP23X17 mcp_A6;
Adafruit_MCP23X17 mcp_A7;

Adafruit_MCP23X17 mcp_B0;
Adafruit_MCP23X17 mcp_B1;
Adafruit_MCP23X17 mcp_B2;
Adafruit_MCP23X17 mcp_B3;
Adafruit_MCP23X17 mcp_B4;
Adafruit_MCP23X17 mcp_B5;
Adafruit_MCP23X17 mcp_B6;
Adafruit_MCP23X17 mcp_B7;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("test PCB sensoren. niet zeker of dit werkt");

  delay(5);

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

  if (!mcp_A4.begin_SPI(CS1_PIN, &SPI, 0x24)) {
    Serial.println("Error. mcp_A4");
    while (1);
  }
  mcp_A4.enableAddrPins();

  if (!mcp_A5.begin_SPI(CS1_PIN, &SPI, 0x25)) {
    Serial.println("Error. mcp_A5");
    while (1);
  }
  mcp_A5.enableAddrPins();

  if (!mcp_A6.begin_SPI(CS1_PIN, &SPI, 0x26)) {
    Serial.println("Error. mcp_A6");
    while (1);
  }
  mcp_A6.enableAddrPins();
  
  if (!mcp_A7.begin_SPI(CS1_PIN, &SPI, 0x27)) {
    Serial.println("Error. mcp_A7");
    while (1);
  }
  mcp_A7.enableAddrPins(); 


// rij 2
  if (!mcp_B0.begin_SPI(CS2_PIN, &SPI, 0x20)) {
    Serial.println("Error. mcp_B0");
    while (1);
  }
  mcp_B0.enableAddrPins();

  if (!mcp_B1.begin_SPI(CS2_PIN, &SPI, 0x21)) {
    Serial.println("Error. mcp_B1");
    while (1);
  }
  mcp_B1.enableAddrPins();

  if (!mcp_B2.begin_SPI(CS2_PIN, &SPI, 0x22)) {
    Serial.println("Error. mcp_B2");
    while (1);
  }
  mcp_B2.enableAddrPins();

  if (!mcp_B3.begin_SPI(CS2_PIN, &SPI, 0x23)) {
    Serial.println("Error. mcp_B3");
    while (1);
  }
  mcp_B3.enableAddrPins();

  if (!mcp_B4.begin_SPI(CS2_PIN, &SPI, 0x24)) {
    Serial.println("Error. mcp_B4");
    while (1);
  }
  mcp_B4.enableAddrPins();

  if (!mcp_B5.begin_SPI(CS2_PIN, &SPI, 0x25)) {
    Serial.println("Error. mcp_B5");
    while (1);
  }
  mcp_B5.enableAddrPins();

  if (!mcp_B6.begin_SPI(CS2_PIN, &SPI, 0x26)) {
    Serial.println("Error. mcp_B6");
    while (1);
  }
  mcp_B6.enableAddrPins();

  if (!mcp_B7.begin_SPI(CS2_PIN, &SPI, 0x27)) {
    Serial.println("Error. mcp_B7");
    while (1);
  }
  mcp_B7.enableAddrPins();


// maakt allen pinnen van de mcp's inputs 
  for(int pinnummer = 0; pinnummer > 16; pinnummer++){
    mcp_A0.pinMode(pinnummer, INPUT); //cs 1
    mcp_A1.pinMode(pinnummer, INPUT);
    mcp_A2.pinMode(pinnummer, INPUT);
    mcp_A3.pinMode(pinnummer, INPUT);
    mcp_A4.pinMode(pinnummer, INPUT); 
    mcp_A5.pinMode(pinnummer, INPUT);
    mcp_A6.pinMode(pinnummer, INPUT);
    mcp_A7.pinMode(pinnummer, INPUT);

    mcp_B0.pinMode(pinnummer, INPUT); //cs 2
    mcp_B1.pinMode(pinnummer, INPUT);
    mcp_B2.pinMode(pinnummer, INPUT);
    mcp_B3.pinMode(pinnummer, INPUT);
    mcp_B4.pinMode(pinnummer, INPUT); 
    mcp_B5.pinMode(pinnummer, INPUT);
    mcp_B6.pinMode(pinnummer, INPUT);
    mcp_B7.pinMode(pinnummer, INPUT);

  }

// looping
  delay(5);
  Serial.println("Looping...");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  uint16_t value_mcp_A0 = mcp_A0.readGPIOAB();
  uint16_t value_mcp_A1 = mcp_A1.readGPIOAB();
  uint16_t value_mcp_A2 = mcp_A2.readGPIOAB();
  uint16_t value_mcp_A3 = mcp_A3.readGPIOAB();
  uint16_t value_mcp_A4 = mcp_A4.readGPIOAB();
  uint16_t value_mcp_A5 = mcp_A5.readGPIOAB();
  uint16_t value_mcp_A6 = mcp_A6.readGPIOAB();
  uint16_t value_mcp_A7 = mcp_A7.readGPIOAB();

  uint16_t value_mcp_B0 = mcp_B0.readGPIOAB();
  uint16_t value_mcp_B1 = mcp_B1.readGPIOAB();
  uint16_t value_mcp_B2 = mcp_B2.readGPIOAB();
  uint16_t value_mcp_B3 = mcp_B3.readGPIOAB();
  uint16_t value_mcp_B4 = mcp_B4.readGPIOAB();
  uint16_t value_mcp_B5 = mcp_B5.readGPIOAB();
  uint16_t value_mcp_B6 = mcp_B6.readGPIOAB();
  uint16_t value_mcp_B7 = mcp_B7.readGPIOAB();

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
// uint16_t value_mcp_A0 = mcp_A0.readGPIOAB(); this is how we read the input of the external IO
//sensor_array[colom][row] = ((uint16_t van de mcp worden >> pinnumber) & 1);

// this methode is fucking shit but i don't know how to do it better
      if ((row < 4) && (colom < 4)) {
        sensor_array[colom][row] = ((value_mcp_A0 >> pinnumber) & 1);
      } else if ((row < 4) && (colom < 8)) {
        sensor_array[colom][row] = ((value_mcp_A1 >> pinnumber) & 1);
      } else if ((row < 4) && (colom < 12)) {
        sensor_array[colom][row] = ((value_mcp_B0 >> pinnumber) & 1);
      } else if ((row < 4) && (colom < 16)) {
        sensor_array[colom][row] = ((value_mcp_B1 >> pinnumber) & 1);
      } else if ((row < 8) && (colom < 4)) {
        sensor_array[colom][row] = ((value_mcp_A2 >> pinnumber) & 1);
      } else if ((row < 8) && (colom < 8)) {
        sensor_array[colom][row] = ((value_mcp_A3 >> pinnumber) & 1);
      } else if ((row < 8) && (colom < 12)) {
        sensor_array[colom][row] = ((value_mcp_B2 >> pinnumber) & 1);
      } else if ((row < 8) && (colom < 16)) {
        sensor_array[colom][row] = ((value_mcp_B3 >> pinnumber) & 1);
      } else if ((row < 12) && (colom < 4)) {
        sensor_array[colom][row] = ((value_mcp_A4 >> pinnumber) & 1);
      } else if ((row < 12) && (colom < 8)) {
        sensor_array[colom][row] = ((value_mcp_A5 >> pinnumber) & 1);
      } else if ((row < 12) && (colom < 12)) {
        sensor_array[colom][row] = ((value_mcp_B4 >> pinnumber) & 1);
      } else if ((row < 12) && (colom < 16)) {
        sensor_array[colom][row] = ((value_mcp_B5 >> pinnumber) & 1);
      } else if ((row < 16) && (colom < 4)) {
        sensor_array[colom][row] = ((value_mcp_A6 >> pinnumber) & 1);
      } else if ((row < 16) && (colom < 8)) {
        sensor_array[colom][row] = ((value_mcp_A7 >> pinnumber) & 1);
      } else if ((row < 16) && (colom < 12)) {
        sensor_array[colom][row] = ((value_mcp_B6 >> pinnumber) & 1);
      } else if ((row < 16) && (colom < 16)) {
        sensor_array[colom][row] = ((value_mcp_B7 >> pinnumber) & 1);
      }       
    }
  }
    for(int row = 0; row < pixel_y; row++) {
    for(int colom = 0; colom < pixel_x; colom++) {
      Serial.print(sensor_array[colom][row]);
    }
    Serial.println(" ");
  } 
}
