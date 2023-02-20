#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>



//This Program is used in Combination with a Windows Forms Application to Screenshare your PC Screen to an WS2812b Matrix
//It is recomendet to use a ESP32
//Created by fabe1999
//last updated: 10.02.2023



//Things you have to change:
//Put your WiFi Credentials here:
//const char* ssid = "AndroidAP";
//const char* password = "vczt1677";
const char* ssid = "LAPTOP-RGT3RNUS 4540";
const char* password = "08?f665M";

//Set the Pin and the *total* Dimensions of Your Matrix here:
const int pin = 0;
const int width = 8;
const int height = 8;

//If you have a special kind of Matrix ore use more than one Tile you should change the settings here,
//For mor Information Visit the Adafruit NeoMatrix Guide: https://learn.adafruit.com/adafruit-neopixel-uberguide/neomatrix-library
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(width, height, pin,
                           NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
                           NEO_MATRIX_ROWS  + NEO_MATRIX_PROGRESSIVE,
                           NEO_GRBW            + NEO_KHZ800);


uint8_t brightnessRGBW = 100;
const uint8_t kWhiteRedChannel = 255;
const uint8_t kWhiteGreenChannel = 244;
const uint8_t kWhiteBlueChannel = 239;

const int buttonPin = 27;  // the number of the pushbutton pin
int buttonState = 0; // variables will change:

//Example of a Tiled Matrix
//If you use a tiled matrix you have to set the individual matrix sizes here manually
// Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, 2, 2, pin,
//                            NEO_MATRIX_TOP      + NEO_MATRIX_LEFT +
//                            NEO_MATRIX_COLUMNS  + NEO_MATRIX_ZIGZAG +
//                            NEO_TILE_BOTTOM     + NEO_TILE_LEFT +
//                            NEO_TILE_ROWS       + NEO_TILE_ZIGZAG);



//If you want to change the Packet Size you can change that here
//It's recommended to use a large package size to cut down the number of packages needed per frame
//If the packages are to big the ESP can not read the content and wont display an Image.
const int udpMaxSize = 1500;

// if you want to change the UDP Port dont forget to change it in the Windows Application as well
const int localUdpPort = 4210;



//No need to change anything after this Point.
//+++++++++++++++++++++++++++++++++++++++++++



const int iDelay = (1000 / width);
int iYIndex = 0;
int iIndex = 0;
//The Actual used UDP Package Size is smaler zhan the max Value.
//It can be calculated by the Size of the Matrix.
const int udpByLength = (((round((height * 1.0) / round((height * width * 2.0 / udpMaxSize) + 0.5)) * width) * 2) + 1);
byte byReceive[udpByLength];
WiFiUDP Udp;

uint32_t RGB565_to_RGBW(uint16_t color, uint8_t brightness);

void setup()
{
  //Initialise the Matrix
  matrix.begin();

  matrix.clear();
  matrix.show();

  //The Max Brightness is limited until the Windows program is connected
  matrix.setBrightness(100);

  //Try to connect to the WiFi
  WiFi.setHostname("ledMatrix");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  //Start to Listen to UDP Packages send to the specified Port
  Udp.begin(localUdpPort);

  //Create a Pink square on the Matrix as a signal that the Setup is finished and the ESP is ready to Connect to the PC Software.
  //If you cant see the Pink Square check your WiFi credentials, Probably the ESP couldnt connect to the WiFi.
  for (int i = 0; i <= width; i++)
  {
    matrix.drawRect(0, 0, i, i, matrix.Color(255, 0, 255));
    matrix.drawRect(0, 0, (i - 3), (i - 3), matrix.Color(0, 0, 0));
    matrix.show();
    delay(iDelay);
  }
  matrix.clear();
  matrix.show();

  Serial.begin(115200);
  Serial.println("Screenshare_SK6812");
  pinMode(buttonPin, INPUT_PULLUP);
}



void loop()
{
  //Listen for UDP Packages
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    //As soon as a Package is Recived the Bytes are stored in a buffer
    int iIndex = 0;
    int len = Udp.read(byReceive, (udpByLength + 2));
    if (len > 0)
    {
      byReceive[len] = 0;
    }

    //If the Send String starts with a C the Windows programm wants to know the Matrix dimensions
    if (byReceive[0] == 'C')
    {
      //Set the maximum Brightness of the Matrix
      matrix.setBrightness(byReceive[1]);
      brightnessRGBW = byReceive[1];

      //Send back the Dimensions of the Matrix and the maximum Package Size to configure the Windows application.
      char replyPacket[15];
      sprintf(replyPacket, "%i;%i;%i", width, height, udpMaxSize);
      Udp.beginPacket(Udp.remoteIP(), byReceive[2]);
      Udp.print(replyPacket);
      Udp.endPacket();

      //Show a Green Circle on the Matrix to show the User a successfull connection
      for (int i = 0; i < (width); i++)
      {
        matrix.drawCircle((width / 2), (height / 2), i, matrix.Color(0, 255, 0));
        matrix.drawCircle((width / 2), (height / 2), (i - 3), matrix.Color(0, 0, 0));
        matrix.show();
        delay(iDelay);
      }
      matrix.clear();
      matrix.show();
    }

    //If the Send String starts with a X the Windows program is closing, Clear the Matrix so the last Picture wont stay on there
    else if (byReceive[0] == 'X')
    {
      //Show a Red Circle on the Matrix to show the Program has been closed
      for (int i = 0; i < (width); i++) {
        matrix.drawCircle((width / 2), (height / 2), i, matrix.Color(255, 0, 0));
        matrix.drawCircle((width / 2), (height / 2), (i - 3), matrix.Color(0, 0, 0));
        matrix.show();
        delay(iDelay);
      }
      matrix.clear();
      matrix.show();
    }

    //If the Send String starts with a number its a Picture Packet to display on the Matrix.
    else
    {
      //Byte 0 is the Y Value of the first LED
      iYIndex = byReceive[0];
      iIndex = 1;

      //Translate the Package into the Display colors
      while ((sizeof(byReceive) - 1) > iIndex)
      {
        for (int x = 0; x < width; x++)
        {
          //Combine two bytes to recive the Color of the current LED
          word wColor = 0;
          wColor = (byReceive[iIndex++] << 8) | (byReceive[iIndex++]);
          uint32_t placeholder_color = RGB565_to_RGBW(wColor, brightnessRGBW);
          matrix.setPassThruColor(placeholder_color);
          matrix.drawPixel(x, iYIndex, wColor);
          matrix.setPassThruColor();
        }
        iYIndex++;
      }
      //If the Whole Package is recived and calculated Show the Picture on the matrix
      //Wait for the next Package, Every Package is a whole Picture
      matrix.show();
    }
  }
  buttonState = digitalRead(buttonPin);
}




uint32_t RGB565_to_RGBW(uint16_t color, uint8_t brightness) { //
  uint8_t r = ((color >> 11) & 0x1F);
  uint8_t g = ((color >> 5) & 0x3F);
  uint8_t b = (color & 0x1F);

  r = map(r, 0, 31, 0, 255);
  g = map(g, 0, 63, 0, 255);
  b = map(b, 0, 31, 0, 255);


  if (buttonState == HIGH) {
    //Serial.println(g);
  } else {
    Serial.println(" ");
    Serial.print("red:");
    Serial.println(r);
    Serial.print("green:");
    Serial.println(g);
    Serial.print("blue:");
    Serial.println(b);
  }

  r = (r * brightness) >> 8;
  g = (g * brightness) >> 8;
  b = (b * brightness) >> 8;

  // These values are what the 'white' value would need to
  // be to get the corresponding color value.
  double whiteValueForRed = r * 255.0 / kWhiteRedChannel;
  double whiteValueForGreen = g * 255.0 / kWhiteGreenChannel;
  double whiteValueForBlue = b * 255.0 / kWhiteBlueChannel;

  // Set the white value to the highest it can be for the given color
  // (without over saturating any channel - thus the minimum of them).
  double minWhiteValue = min(whiteValueForRed,
      min(whiteValueForGreen,
          whiteValueForBlue));
  uint8_t Wo = (minWhiteValue <= 255 ? (uint8_t)minWhiteValue : 255);

  // The rest of the channels will just be the original value minus the
  // contribution by the white channel.
  uint8_t Ro = (uint8_t)(r - minWhiteValue * kWhiteRedChannel / 255);
  uint8_t Go = (uint8_t)(g - minWhiteValue * kWhiteGreenChannel / 255);
  uint8_t Bo = (uint8_t)(b - minWhiteValue * kWhiteBlueChannel / 255);

  return Bo + (Go << 8) + (Ro << 16) + (Wo << 24);
}
