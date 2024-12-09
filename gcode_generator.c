#include <stdio.h>
#include <string.h>
#include "gcode_generator.h"
#include "font_loader.h"
#include "stroke.h"
#include "serial.h"

void process_text_via_rs232(const char* text_filename, float text_height) {
    FILE* text_file = fopen(text_filename, "r");
    if (!text_file) {
        printf("Error: Unable to open text file '%s'.\n", text_filename);
        return;
    }

    float scale_factor = text_height / 18.0f; // Scale factor for text height
    char gcode_line[100];
    char word[100];
    int current_x = 0;   // Current X position (in mm)
    int current_y = 0;   // Current Y position (in mm)
    const int max_line_width = 100; // Maximum writing width (in mm)

    while (fscanf(text_file, "%99s", word) == 1) {
        // Process each character in the word
        for (const char* p = word; *p != '\0'; p++) {
            int ascii = (int)*p;

            // Skip unsupported characters
            if (ascii < 0 || ascii >= 128 || !font[ascii].strokes) {
                continue;
            }

            // Scale the character strokes
            scale_character(&font[ascii], scale_factor);

            // Check if the character fits in the current line
            if (current_x + font[ascii].strokes[font[ascii].stroke_count - 1].x * scale_factor > max_line_width) {
                // Move to the next line
                current_y -= 5; // Add 5 mm line spacing
                current_x = 0;  // Reset X position
                sprintf(gcode_line, "G0 X0 Y%d\n", current_y);
                PrintBuffer(gcode_line);
                WaitForReply();
            }

            // Generate G-code for each stroke of the character
            for (int i = 0; i < font[ascii].stroke_count; i++) {
                struct Stroke* stroke = &font[ascii].strokes[i];
                if (stroke->pen_down) {
                    sprintf(gcode_line, "S1000\nG1 X%d Y%d\n",
                            current_x + stroke->x, current_y + stroke->y);
                } else {
                    sprintf(gcode_line, "S0\nG0 X%d Y%d\n",
                            current_x + stroke->x, current_y + stroke->y);
                }
                PrintBuffer(gcode_line);
                WaitForReply();
            }

            // Update the X position for the next character
            current_x += (int)((float)(font[ascii].strokes[font[ascii].stroke_count - 1].x) * scale_factor);
        }

        // Add a space between words
        current_x += (int)(5 * scale_factor);

    }

    // Finalize: lift pen and move to the origin
    PrintBuffer("S0\nG0 X0 Y0\n");
    WaitForReply();

    fclose(text_file);
}
