#ifndef GCODE_GENERATOR_H
#define GCODE_GENERATOR_H

#include <stdio.h>
#include "stroke.h" // For struct Character and scaling functionality

// Function to process a text file and send G-code to the robot via RS232
void process_text_via_rs232(const char* text_filename, float text_height);

#endif // GCODE_GENERATOR_H
