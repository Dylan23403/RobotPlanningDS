#ifndef GCODE_GENERATOR_H
#define GCODE_GENERATOR_H

#include <stdio.h>
#include "stroke.h" // For struct Character and scaling functionality

// Function to generate G-code for a single character and write to a file
void generate_gcode_for_character(struct Character* character, FILE* output_file);

// Function to process a text file and generate G-code into an output file
void process_text(const char* text_filename, float text_height, const char* output_filename);

// Function to process a text file and send G-code to the robot via RS232
void process_text_via_rs232(const char* text_filename, float text_height);

#endif // GCODE_GENERATOR_H
