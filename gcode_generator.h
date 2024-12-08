#ifndef GCODE_GENERATOR_H
#define GCODE_GENERATOR_H

#include <stdio.h>
#include "stroke.h" // For struct Character and scaling functionality

// Function to generate G-code for a single character
void generate_gcode_for_character(struct Character* character, FILE* output_file);

// Function to process a text file and generate G-code
void process_text(const char* text_filename, float text_height, const char* output_filename);

#endif // GCODE_GENERATOR_H