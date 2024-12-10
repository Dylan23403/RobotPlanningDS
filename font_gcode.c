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

    int ascii, stroke_count;
    while (fscanf(fInput, "999 %d %d", &ascii, &stroke_count) == 2) {
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
        printf("Loaded ASCII %d (%c) with %d strokes.\n", ascii, ascii, stroke_count);
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
void process_text_via_rs232(const char* text_filename, float text_height) {
    FILE* text_file = fopen(text_filename, "r");
    if (!text_file) {
        printf("Error: Could not open text file '%s'.\n", text_filename);
        exit(1); // Exit if the file cannot be opened
    }

    printf("Processing text file: %s\n", text_filename);

    float scale_factor = text_height / 18.0f;
    char gcode_line[100];
    char word[100];
    int current_x = 0;   // Current X position (in mm)
    int current_y = 0;   // Current Y position (in mm)
    const int max_line_width = 100; // Maximum width of a line in mm

    // Read and process each word in the file
    while (fscanf(text_file, "%99s", word) == 1) {
        printf("Processing word: %s\n", word);

        for (const char* p = word; *p != '\0'; p++) {
            int ascii = (int)*p;

            if (ascii < 0 || ascii >= 128 || !font[ascii].strokes) {
                printf("Skipping unsupported character: %c (ASCII %d)\n", *p, ascii);
                continue;
            }
             // Check if adding the character will exceed the max line width
            if (current_x + font[ascii].strokes[font[ascii].stroke_count - 1].x * scale_factor > max_line_width) {
            current_y -= 5; // Move to the next line (down 5mm)
            current_x = 0;  // Reset X position to the start of the new line
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
                printf("Generated G-code: %s", gcode_line);
                PrintBuffer(gcode_line); // Send the G-code to the robot
                WaitForReply();          // Wait for the robot to acknowledge
            }

            // Update X position for the next character
            current_x += (int)(font[ascii].strokes[font[ascii].stroke_count - 1].x * scale_factor);
        }

        // Add spacing between words
        current_x += (int)(5 * scale_factor);
    }

    fclose(text_file);
    printf("Text file processed successfully.\n");
}
