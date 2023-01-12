#include <SPI.h>
#include <Adafruit_MCP23X17.h>
/*
int x = 16; //row
int y = 16; //colum
int opp = x*y;

// https://www.geeksforgeeks.org/multidimensional-arrays-c-cpp/
//bool sensor_array[x][y];
*/

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

#define CS1_PIN 5 //chip select 1
#define CS2_PIN 6 //chip select 2

// FOR TESTING. THIS IS SO IF VALUE CHANGES IT ONLY THEN PRINTS TO SCREEN
uint16_t OLD_value_mcp_A0, OLD_value_mcp_A1, OLD_value_mcp_A2, OLD_value_mcp_A3, OLD_value_mcp_A4, OLD_value_mcp_A5, OLD_value_mcp_A6, OLD_value_mcp_A7 = 0;
uint16_t OLD_value_mcp_B0, OLD_value_mcp_B1, OLD_value_mcp_B2, OLD_value_mcp_B3, OLD_value_mcp_B4, OLD_value_mcp_B5, OLD_value_mcp_B6, OLD_value_mcp_B7 = 0;


void setup() {
  Serial.begin(9600);
  Serial.println("MCP23S17 test met CS");

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
  Serial.println("Looping...");
}


void loop() {
//RIJ 1  
  uint16_t value_mcp_A0 = mcp_A0.readGPIOAB();
  if(value_mcp_A0 != OLD_value_mcp_A0)
  {
    OLD_value_mcp_A0 = value_mcp_A0;
    Serial.print("mcp_A0:");
    Serial.println(value_mcp_A0, BIN);
  }

  uint16_t value_mcp_A1 = mcp_A1.readGPIOAB();
  if(value_mcp_A1 != OLD_value_mcp_A1)
  {
    OLD_value_mcp_A1 = value_mcp_A1;
    Serial.print("mcp_A1:");
    Serial.println(value_mcp_A1, BIN);
  }
  
  uint16_t value_mcp_A2 = mcp_A2.readGPIOAB();
  if(value_mcp_A2 != OLD_value_mcp_A2)
  {
    OLD_value_mcp_A2 = value_mcp_A2;
    Serial.print("mcp_A2:");
    Serial.println(value_mcp_A2, BIN);
  }

  uint16_t value_mcp_A3 = mcp_A3.readGPIOAB();
  if(value_mcp_A3 != OLD_value_mcp_A3)
  {
    OLD_value_mcp_A3 = value_mcp_A3;
    Serial.print("mcp_A3:");
    Serial.println(value_mcp_A3, BIN);
  }

  uint16_t value_mcp_A4 = mcp_A4.readGPIOAB();
  if(value_mcp_A4 != OLD_value_mcp_A4)
  {
    OLD_value_mcp_A4 = value_mcp_A4;
    Serial.print("mcp_A4:");
    Serial.println(value_mcp_A4, BIN);
  }

  uint16_t value_mcp_A5 = mcp_A5.readGPIOAB();
  if(value_mcp_A5 != OLD_value_mcp_A5)
  {
    OLD_value_mcp_A5 = value_mcp_A5;
    Serial.print("mcp_A5:");
    Serial.println(value_mcp_A5, BIN);
  }
  
  uint16_t value_mcp_A6 = mcp_A6.readGPIOAB();
  if(value_mcp_A6 != OLD_value_mcp_A6)
  {
    OLD_value_mcp_A6 = value_mcp_A6;
    Serial.print("mcp_A6:");
    Serial.println(value_mcp_A6, BIN);
  }

  uint16_t value_mcp_A7 = mcp_A7.readGPIOAB();
  if(value_mcp_A7 != OLD_value_mcp_A7)
  {
    OLD_value_mcp_A7 = value_mcp_A7;
    Serial.print("mcp_A7:");
    Serial.println(value_mcp_A7, BIN);
  }

//RIJ 2
  uint16_t value_mcp_B0 = mcp_B0.readGPIOAB();
  if(value_mcp_B0 != OLD_value_mcp_B0)
  {
    OLD_value_mcp_B0 = value_mcp_B0;
    Serial.print("mcp_B0:");
    Serial.println(value_mcp_B0, BIN);
  }

  uint16_t value_mcp_B1 = mcp_B1.readGPIOAB();
  if(value_mcp_B1 != OLD_value_mcp_B1)
  {
    OLD_value_mcp_B1 = value_mcp_B1;
    Serial.print("mcp_B1:");
    Serial.println(value_mcp_B1, BIN);
  }
  
  uint16_t value_mcp_B2 = mcp_B2.readGPIOAB();
  if(value_mcp_B2 != OLD_value_mcp_B2)
  {
    OLD_value_mcp_B2 = value_mcp_B2;
    Serial.print("mcp_B2:");
    Serial.println(value_mcp_B2, BIN);
  }

  uint16_t value_mcp_B3 = mcp_B3.readGPIOAB();
  if(value_mcp_B3 != OLD_value_mcp_B3)
  {
    OLD_value_mcp_B3 = value_mcp_B3;
    Serial.print("mcp_B3:");
    Serial.println(value_mcp_B3, BIN);
  }

  uint16_t value_mcp_B4 = mcp_B4.readGPIOAB();
  if(value_mcp_B4 != OLD_value_mcp_B4)
  {
    OLD_value_mcp_B4 = value_mcp_B4;
    Serial.print("mcp_B4:");
    Serial.println(value_mcp_B4, BIN);
  }

  uint16_t value_mcp_B5 = mcp_B5.readGPIOAB();
  if(value_mcp_B5 != OLD_value_mcp_B5)
  {
    OLD_value_mcp_B5 = value_mcp_B5;
    Serial.print("mcp_B5:");
    Serial.println(value_mcp_B5, BIN);
  }
  
  uint16_t value_mcp_B6 = mcp_B6.readGPIOAB();
  if(value_mcp_B6 != OLD_value_mcp_B6)
  {
    OLD_value_mcp_B6 = value_mcp_B6;
    Serial.print("mcp_B6:");
    Serial.println(value_mcp_B6, BIN);
  }

  uint16_t value_mcp_B7 = mcp_B7.readGPIOAB();
  if(value_mcp_B7 != OLD_value_mcp_B7)
  {
    OLD_value_mcp_B7 = value_mcp_B7;
    Serial.print("mcp_B7:");
    Serial.println(value_mcp_B7, BIN);
  }
}





/*
Adafruit_MCP23X17 mcp_C0;
Adafruit_MCP23X17 mcp_C1;
Adafruit_MCP23X17 mcp_C2;
Adafruit_MCP23X17 mcp_C3;
Adafruit_MCP23X17 mcp_C4;
Adafruit_MCP23X17 mcp_C5;
Adafruit_MCP23X17 mcp_C6;
Adafruit_MCP23X17 mcp_C7;

Adafruit_MCP23X17 mcp_D0;
Adafruit_MCP23X17 mcp_D1;
Adafruit_MCP23X17 mcp_D2;
Adafruit_MCP23X17 mcp_D3;
Adafruit_MCP23X17 mcp_D4;
Adafruit_MCP23X17 mcp_D5;
Adafruit_MCP23X17 mcp_D6;
Adafruit_MCP23X17 mcp_D7;

Adafruit_MCP23X17 mcp_E0;
Adafruit_MCP23X17 mcp_E1;
Adafruit_MCP23X17 mcp_E2;
Adafruit_MCP23X17 mcp_E3;
Adafruit_MCP23X17 mcp_E4;
Adafruit_MCP23X17 mcp_E5;
Adafruit_MCP23X17 mcp_E6;
Adafruit_MCP23X17 mcp_E7;

Adafruit_MCP23X17 mcp_F0;
Adafruit_MCP23X17 mcp_F1;
Adafruit_MCP23X17 mcp_F2;
Adafruit_MCP23X17 mcp_F3;
Adafruit_MCP23X17 mcp_F4;
Adafruit_MCP23X17 mcp_F5;
Adafruit_MCP23X17 mcp_B6;
Adafruit_MCP23X17 mcp_F7;

Adafruit_MCP23X17 mcp_G0;
Adafruit_MCP23X17 mcp_G1;
Adafruit_MCP23X17 mcp_G2;
Adafruit_MCP23X17 mcp_G3;
Adafruit_MCP23X17 mcp_G4;
Adafruit_MCP23X17 mcp_G5;
Adafruit_MCP23X17 mcp_G6;
Adafruit_MCP23X17 mcp_G7;

Adafruit_MCP23X17 mcp_H0;
Adafruit_MCP23X17 mcp_H1;
Adafruit_MCP23X17 mcp_H2;
Adafruit_MCP23X17 mcp_H3;
Adafruit_MCP23X17 mcp_H4;
Adafruit_MCP23X17 mcp_H5;
Adafruit_MCP23X17 mcp_H6;
Adafruit_MCP23X17 mcp_H7;
*/
