#include <stdio.h>
#include <stdlib.h>
#include "font_loader.h"

// Global font array
struct Character font[128];

void load_font(const char* filename) 
{
    // Attempt to open the font file in read mode
    FILE* file = fopen("SingleStrokeFont.txt", "r");
    if (file == NULL) {
        // If the file cannot be opened, print an error message and exit the function
        printf("\nThe file could not be opened for reading, exiting.");
        return; // Return to avoid continuing with an invalid file
    }

    int ascii, stroke_count; // Variables to hold ASCII code and number of strokes for each character

    // Loop to read character data from the file until no more valid lines are found
    while (fscanf(file, "999 %d %d", &ascii, &stroke_count) == 2) 
    {
        // Store the ASCII code in the corresponding entry of the font array
        font[ascii].ascii_code = ascii;

        // Store the number of strokes in the corresponding entry
        font[ascii].stroke_count = stroke_count;

        // Allocate memory dynamically to hold the strokes for this character
        font[ascii].strokes = malloc(sizeof(struct Stroke) * stroke_count);

        // Check if memory allocation was successful
        if (font[ascii].strokes == NULL) 
        {
            printf("\nError allocating memory for strokes. Exiting.");
            fclose(file); // Close the file before exiting
            exit(EXIT_FAILURE); // Exit the program with an error code
        }

        // Loop through the number of strokes and read each stroke's data
        for (int i = 0; i < stroke_count; i++) 
        {
            // Read x, y, and pen_down for each stroke
            fscanf(file, "%d %d %d",
                   &font[ascii].strokes[i].x,       // Store x-coordinate
                   &font[ascii].strokes[i].y,       // Store y-coordinate
                   &font[ascii].strokes[i].pen_down); // Store pen state (0=up, 1=down)
        }
    }
    fclose(file);
}

// Free the font data
void free_font_data() 
{
    for (int i = 0; i < 128; i++) {
        free_character(&font[i]);
    }
}
