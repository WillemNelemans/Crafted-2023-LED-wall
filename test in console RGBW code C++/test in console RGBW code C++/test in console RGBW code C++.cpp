// test in console RGBW code C++.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

int main()
{
    int r, g, b;

    std::cout << "\nValue RED ";
    std::cin >> r;

    std::cout << "\nValue GREEN ";
    std::cin >> g;
  
    std::cout << "\nValue BLUE ";
    std::cin >> b;

    
    // Reference, currently set to 4500k white light:
    // https://andi-siess.de/rgb-to-color-temperature/
    const int kWhiteRedChannel = 255;
    const int kWhiteGreenChannel = 243;
    const int kWhiteBlueChannel = 239;

    // The transformation has to be normalized to 255
    //static_assert(kWhiteRedChannel >= 255 || kWhiteGreenChannel >= 255 || kWhiteBlueChannel >= 255);

    

    // These values are what the 'white' value would need to
    // be to get the corresponding color value.
    double whiteValueForRed = r * 255.0 / kWhiteRedChannel;
    double whiteValueForGreen = g * 255.0 / kWhiteGreenChannel;
    double whiteValueForBlue = b * 255.0 / kWhiteBlueChannel;

    // Set the white value to the highest it can be for the given color
    // (without over saturating any channel - thus the minimum of them).
    double minWhiteValue = std::min(whiteValueForRed, std::min(whiteValueForGreen, whiteValueForBlue));
    int Wo = (minWhiteValue <= 255 ? (int)minWhiteValue : 255);

    // The rest of the channels will just be the original value minus the
    // contribution by the white channel.
    int Ro = (int)(r - minWhiteValue * kWhiteRedChannel / 255);
    int Go = (int)(g - minWhiteValue * kWhiteGreenChannel / 255);
    int Bo = (int)(b - minWhiteValue * kWhiteBlueChannel / 255);

    std::cout << "\n Output RED ";
    std::cout << Ro;
    std::cout << "\n Output GREEN ";
    std::cout << Go;
    std::cout << "\n Output BLUE ";
    std::cout << Bo;
    std::cout << "\n Output WHITE ";
    std::cout << Wo;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
