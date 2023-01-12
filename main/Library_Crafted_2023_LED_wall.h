#ifndef LIBRARY_CRAFTED_2023_LED_WALL_H
#define LIBRARY_CRAFTED_2023_LED_WALL_H



#include <SPI.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

class LED_wall {
  public:
    uint8_t brightnessRGBW;
    uint8_t kWhiteRedChannel;
    uint8_t kWhiteGreenChannel;
    uint8_t kWhiteBlueChannel;

    void set_brightnessRGBW(int n);
    int get_brightnessRGBW();
    void set_WhiteRedChannel(int n);
    int get_WhiteRedChannel();
    void set_WhiteGreenChannel(int n);
    int get_WhiteGreenChannel();
    void set_WhiteBlueChannel(int n);
    int get_WhiteBlueChannel();

    uint32_t Rainbow(double ratio);
    
    uint32_t RGB_to_RGBW(uint32_t color);
    uint32_t RGB_to_RGBW(uint8_t r, uint8_t g, uint8_t b);

  private:


  protected:


} ;




#endif