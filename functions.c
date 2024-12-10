#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "serial.h"

// Global font array to store stroke data for each character
struct Character font[128]; // Indexed by ASCII values to store character data

// Function to load font data from `SingleStrokeFont.txt`
void load_font(void) 
{
    // Open the font file in read mode
    FILE* fInput = fopen("SingleStrokeFont.txt", "r");
    if (fInput == NULL) {
        // Handle error if the file cannot be opened
        printf("Error: Could not open font file 'SingleStrokeFont.txt'.\n");
        exit(1); // Terminate the program
    }

    printf("Loading font data...\n");

    int prefix, ascii, stroke_count; // Variables for reading font data
    // Read each character's data until the end of the file
    while (fscanf(fInput, "%d %d %d", &prefix, &ascii, &stroke_count) == 3) {
        // Store character ASCII code and stroke count
        font[ascii].ascii_code = ascii;
        font[ascii].stroke_count = stroke_count;

        // Allocate memory for the strokes of the character
        font[ascii].strokes = malloc(sizeof(struct Stroke) * (size_t)stroke_count);
        if (font[ascii].strokes == NULL) {
            // Handle error in memory allocation
            printf("Error: Memory allocation failed for ASCII %d.\n", ascii);
            fclose(fInput); // Ensure the file is closed
            exit(1);
        }

        // Read each stroke's data (x, y coordinates, pen state)
        for (int i = 0; i < stroke_count; i++) {
            fscanf(fInput, "%d %d %d",
                   &font[ascii].strokes[i].x, 
                   &font[ascii].strokes[i].y, 
                   &font[ascii].strokes[i].pen_down); 
        }
    }

    fclose(fInput); // Close the file after reading
    printf("Font data loaded successfully.\n");
}

// Function to free memory allocated for font data
void free_font_data(void) {
    // Loop through the font array
    for (int i = 0; i < 128; i++) {
        if (font[i].strokes) {
            free(font[i].strokes);  // Free memory for strokes
            font[i].strokes = NULL; // Avoid dangling pointers
        }
    }
    printf("Font data freed successfully.\n");
}

// Function to process text, generate G-code, and send commands to the robot
void process_text_via_rs232(const char* text_filename, float text_height) 
{
    // Open the text file containing the words to draw
    FILE* text_file = fopen(text_filename, "r");
    if (!text_file) {
        // Handle error if the file cannot be opened
        printf("Error: Could not open text file '%s'.\n", text_filename);
        exit(1);
    }

    // Calculate the scale factor for text height
    float scale_factor = text_height / 18.0f;
    char word[100];           // Buffer to store each word
    int current_x = 0;        // Tracks the robot's current X position
    int current_y = -(int)(text_height); // Start below Y=0 to prevent overlapping
    int line_spacing = 5;     // Fixed spacing between lines in mm
    const int max_line_width = 100; // Maximum allowable line width in mm

    // Start the robot at the initial position with the pen lifted
    printf("S0\nG0 X0 Y%d\n", current_y);

    // Process each word in the text file
    while (fscanf(text_file, "%99s", word) == 1) {
        // Calculate the width of the current word
        int word_width = 0;
        for (const char* p = word; *p != '\0'; p++) {
            int ascii = (int)*p;
            if (ascii >= 0 && ascii < 128 && font[ascii].strokes) {
                word_width += (int)(font[ascii].strokes[font[ascii].stroke_count - 1].x * scale_factor);
            }
        }

        // Check if the word fits in the current line; move to a new line if necessary
        if (current_x + word_width > max_line_width) {
            current_x = 0; // Reset X position for the new line
            current_y -= (int)(text_height + line_spacing); // Move downward
            printf("S0\nG0 X%d Y%d\n", current_x, current_y); // Update robot's position
        }

        // Generate G-code for each letter in the word
        for (const char* p = word; *p != '\0'; p++) {
            int ascii = (int)*p;

            // Skip unsupported characters
            if (ascii < 0 || ascii >= 128 || !font[ascii].strokes) {
                continue;
            }

            // Process each stroke for the character
            for (int i = 0; i < font[ascii].stroke_count; i++) {
                struct Stroke* stroke = &font[ascii].strokes[i];
                int scaled_x = current_x + (int)(stroke->x * scale_factor);
                int scaled_y = current_y - (int)((text_height - stroke->y) * scale_factor); // Move downward

                if (stroke->pen_down) {
                    // Pen down: Draw a line
                    printf("S1000\nG1 X%.2f Y%.2f\n", (double)scaled_x, (double)scaled_y);
                } else {
                    // Pen up: Move without drawing
                    printf("S0\nG0 X%.2f Y%.2f\n", (double)scaled_x, (double)scaled_y);
                }
            }

            // Update X position for the next character
            current_x += (int)(font[ascii].strokes[font[ascii].stroke_count - 1].x * scale_factor);
        }

        // Add spacing between words
        current_x += (int)(5 * scale_factor);
    }

    fclose(text_file); // Close the text file

    // Ensure the robot ends at (0,0) with the pen lifted
    printf("S0\nG0 X0 Y0\n");
    printf("Text file processed successfully.\n");
}
