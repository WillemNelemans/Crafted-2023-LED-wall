// test array sensors.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

const int panelen_x = 2;  //links rechts
const int panelen_y = 2;  //boven onder

int pixel_x = panelen_x * 8;
int pixel_y = panelen_y * 8;

//https://electronics.stackexchange.com/questions/400324/accessing-and-storing-a-bit-from-a-uint8

bool sensor_array[panelen_x * 8][panelen_y * 8] = {false};


uint16_t value_mcp_A0 = 49239;
uint16_t value_mcp_A1 = 981;
uint16_t value_mcp_A2 = 59907;
uint16_t value_mcp_A3 = 43968;
uint16_t value_mcp_A4 = 16296;
uint16_t value_mcp_A5 = 64554;
uint16_t value_mcp_A6 = 5628;
uint16_t value_mcp_A7 = 21567;

uint16_t value_mcp_B0;
uint16_t value_mcp_B1;
uint16_t value_mcp_B2;
uint16_t value_mcp_B3;
uint16_t value_mcp_B4 = 49239;
uint16_t value_mcp_B5 = 981;
uint16_t value_mcp_B6 = 59907;
uint16_t value_mcp_B7 = 43968;

int pinnumber;

int main()
{
    std::cout << pixel_x;
    std::cout << " x ";
    std::cout << pixel_y;
    std::cout << " raster\n";

    // sets alle te values to low inside the array
    for (int row = 0; row < pixel_y; row++) {
        for (int colom = 0; colom < pixel_x; colom++) {
            sensor_array[colom][row] = false; 
        }
    }

    
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
    

    
    //print out the values of the array
    for (int row = 0; row < pixel_y; row++) {
        for (int colom = 0; colom < pixel_x; colom++) {
            std::cout << sensor_array[colom][row];
        }
        std::cout << "\n";
    }
    
}
    