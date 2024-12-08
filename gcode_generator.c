#include <stdio.h>
#include "gcode_generator.h"
#include "font_loader.h"

// Function to generate G-Code commands for a character's strokes and write to a file
void generate_gcode_for_character(struct Character* character, FILE* output_file) 
{
    for (int i = 0; i < character->stroke_count; i++) 
    {
        struct Stroke* stroke = &character->strokes[i];
        if (stroke->pen_down) 
        {
            fprintf(output_file, "S1000\n"); // Lower the pen
            fprintf(output_file, "G1 X%d Y%d\n", stroke->x, stroke->y); // Draw line
        } 
        else 
        {
            fprintf(output_file, "S0\n"); // Lift the pen
            fprintf(output_file, "G0 X%d Y%d\n", stroke->x, stroke->y); // Move without drawing
        }
    }
}

// Processes the input text file and generates G-code
void process_text(const char* text_filename, float text_height, const char* output_filename) {
    float scale_factor = text_height / 18.0; // Original height is 18 units

    FILE* text_file = fopen(text_filename, "r");
    if (text_file == NULL) {
        printf("Error: Unable to open text file '%s'.\n", text_filename);
        return;
    }


FILE* text_file = fopen(text_filename, "r");
    if (text_file == NULL) {
        printf("Error: Unable to open text file '%s'.\n", text_filename);
        return;
    }

    FILE* output_file = fopen(output_filename, "w");
    if (output_file == NULL) {
        printf("Error: Unable to open output file '%s'.\n", output_filename);
        fclose(text_file);
        return;
    }

fprintf(output_file, "G21\n");  // Set units to millimeters
    fprintf(output_file, "G90\n");  // Set absolute positioning
    fprintf(output_file, "M3\n");   // Spindle on (pen ready)

    char word[100];
    int current_x = 0;
    const int max_line_width = 100;

    while (fscanf(text_file, "%99s", word) == 1) {
        for (const char* p = word; *p != '\0'; p++) {
            int ascii = (int)*p;

            if (ascii < 0 || ascii >= 128 || font[ascii].strokes == NULL) {
                continue; // Skip unsupported characters
            }

            scale_character(&font[ascii], scale_factor);
            generate_gcode_for_character(&font[ascii], output_file);

            current_x += (font[ascii].strokes[font[ascii].stroke_count - 1].x + 5) * scale_factor;

            if (current_x > max_line_width) {
                fprintf(output_file, "G0 X0 Y-5\n");
                current_x = 0;
            }
        }
        current_x += 5 * scale_factor; // Space between words
    }

    fprintf(output_file, "S0\n");
    fprintf(output_file, "G0 X0 Y0\n");

    fclose(text_file);
    fclose(output_file);
}
