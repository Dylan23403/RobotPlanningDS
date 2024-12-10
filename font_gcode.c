#include <stdio.h>
#include <stdlib.h>
#include "font_gcode.h"
#include "serial.h"

// Global font array
struct Character font[128];

// Function to load font data from `SingleStrokeFont.txt`
void load_font(void) 
{
    FILE* fInput = fopen("SingleStrokeFont.txt", "r");
    if (fInput == NULL) {
        printf("Error: Could not open font file 'SingleStrokeFont.txt'.\n");
        exit(1); // Exit if the file cannot be opened
    }

    printf("Loading font data...\n");

    int prefix, ascii, stroke_count;
    while (fscanf(fInput, "%d %d %d",&prefix, &ascii, &stroke_count) == 3) {
        font[ascii].ascii_code = ascii;
        font[ascii].stroke_count = stroke_count;

        // Allocate memory for strokes of this character
        font[ascii].strokes = malloc(sizeof(struct Stroke) * (size_t)stroke_count);
        if (font[ascii].strokes == NULL) {
            printf("Error: Memory allocation failed for ASCII %d.\n", ascii);
            fclose(fInput); // Close the file before exiting
            exit(1);
        }

        // Read each stroke
        for (int i = 0; i < stroke_count; i++) {
            fscanf(fInput, "%d %d %d",
                   &font[ascii].strokes[i].x, 
                   &font[ascii].strokes[i].y, 
                   &font[ascii].strokes[i].pen_down); 
        }
    }

    fclose(fInput);
    printf("Font data loaded successfully.\n");
}

// Function to free memory allocated for font data
void free_font_data(void) {
    for (int i = 0; i < 128; i++) {
        if (font[i].strokes) {
            free(font[i].strokes);  // Free the dynamically allocated stroke array
            font[i].strokes = NULL; // Avoid dangling pointers
        }
    }
    printf("Font data freed successfully.\n");
}

// Function to process text, generate G-code, and send commands to the robot
void process_text_via_rs232(const char* text_filename, float text_height) 
{
    FILE* text_file = fopen(text_filename, "r");
    if (!text_file) {
        printf("Error: Could not open text file '%s'.\n", text_filename);
        exit(1);
    }

    float scale_factor = text_height / 18.0f; // Scale text height
    char word[100];
    int current_x = 0;         // Current X position
    int current_y = -(int)(text_height); // Start below Y=0
    int line_spacing = 5;      // Spacing between lines in mm
    const int max_line_width = 100; // Maximum width for a line in mm

    // Ensure the robot starts at the correct position with the pen lifted
    printf("S0\nG0 X0 Y%d\n", current_y);

    while (fscanf(text_file, "%99s", word) == 1) {
        // Calculate the width of the word
        int word_width = 0;
        for (const char* p = word; *p != '\0'; p++) {
            int ascii = (int)*p;
            if (ascii >= 0 && ascii < 128 && font[ascii].strokes) {
                word_width += (int)(font[ascii].strokes[font[ascii].stroke_count - 1].x * scale_factor);
            }
        }

        // If the word doesn't fit in the current line, move to the next line
        if (current_x + word_width > max_line_width) {
            current_x = 0;            // Reset X position
            current_y -= (int)(text_height + line_spacing); // Move downward for the next line
            printf("S0\nG0 X%d Y%d\n", current_x, current_y); // Move to the new line
        }

        // Draw the word
        for (const char* p = word; *p != '\0'; p++) {
            int ascii = (int)*p;

            // Skip unsupported characters
            if (ascii < 0 || ascii >= 128 || !font[ascii].strokes) {
                continue;
            }

            // Generate G-code for each stroke of the character
            for (int i = 0; i < font[ascii].stroke_count; i++) {
                struct Stroke* stroke = &font[ascii].strokes[i];
                int scaled_x = current_x + (int)(stroke->x * scale_factor);
                int scaled_y = current_y - (int)((text_height - stroke->y) * scale_factor); // Ensure downward strokes

                if (stroke->pen_down) {
                    printf("S1000\nG1 X%.2f Y%.2f\n", (double)scaled_x, (double)scaled_y);
                } else {
                    printf("S0\nG0 X%.2f Y%.2f\n", (double)scaled_x, (double)scaled_y);
                }
            }

            // Update current_x based on the last stroke's endpoint of the current letter
            current_x += (int)(font[ascii].strokes[font[ascii].stroke_count - 1].x * scale_factor);
        }

        // Add spacing between words
        current_x += (int)(5 * scale_factor);
    }

    fclose(text_file);

    // Ensure the robot ends at (0,0) with the pen lifted
    printf("S0\nG0 X0 Y0\n");
    printf("Text file processed successfully.\n");
}
