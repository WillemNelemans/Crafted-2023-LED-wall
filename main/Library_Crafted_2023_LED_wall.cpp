#include "Library_Crafted_2023_LED_wall.h"

#include <Arduino.h>

#include <SPI.h>
#include <Adafruit_MCP23X17.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

static uint8_t brightnessRGBW;
static uint8_t kWhiteRedChannel;
static uint8_t kWhiteGreenChannel;
static uint8_t kWhiteBlueChannel;


void LED_wall::set_brightnessRGBW(int n) {
  brightnessRGBW = n; 
}

int LED_wall::get_brightnessRGBW() {
  return brightnessRGBW;
}

void LED_wall::set_WhiteRedChannel(int n) {
  kWhiteRedChannel = n;
}

int LED_wall::get_WhiteRedChannel() {
  return kWhiteRedChannel;
}

void LED_wall::set_WhiteGreenChannel(int n) {
  kWhiteGreenChannel = n;
}

int LED_wall::get_WhiteGreenChannel() {
  return kWhiteGreenChannel;
}

void LED_wall::set_WhiteBlueChannel(int n) {
  kWhiteBlueChannel = n;
}

int LED_wall::get_WhiteBlueChannel() {
  return kWhiteBlueChannel;
}


uint32_t LED_wall::Rainbow(double ratio) {
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


uint32_t LED_wall::RGB_to_RGBW(uint32_t color) {
  uint8_t r = (uint8_t)color;
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)(color >> 16);

  r = (r * brightnessRGBW) >> 8;
  g = (g * brightnessRGBW) >> 8;
  b = (b * brightnessRGBW) >> 8;

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


uint32_t LED_wall::RGB_to_RGBW(uint8_t r, uint8_t g, uint8_t b) {

  r = (r * brightnessRGBW) >> 8;
  g = (g * brightnessRGBW) >> 8;
  b = (b * brightnessRGBW) >> 8;

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
uint32_t LED_wall::RGB_to_RGBW(uint32_t color, uint8_t brightness) {
  uint8_t r = (uint8_t)color;
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)(color >> 16);

  // i looked at the neopixel library and they add +1 to bightness https://github.com/adafruit/Adafruit_NeoPixel/blob/master/Adafruit_NeoPixel.cpp
//    r = (r * (brightness + 1)) >> 8;
//    g = (g * (brightness + 1)) >> 8;
//    b = (b * (brightness + 1)) >> 8;

  r = (r * brightnessRGBW) >> 8;
  g = (g * brightnessRGBW) >> 8;
  b = (b * brightnessRGBW) >> 8;


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


uint32_t LED_wall::RGB_to_RGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
//  r = (r * brightness) >> 8;
//  g = (g * brightness) >> 8;
//  b = (b * brightness) >> 8;

  r = (r * brightnessRGBW) >> 8;
  g = (g * brightnessRGBW) >> 8;
  b = (b * brightnessRGBW) >> 8;

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