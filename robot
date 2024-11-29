#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to hold X, Y coordinates
struct Position {
    int x; // X-coordinate
    int y; // Y-coordinate
};

// Structure to hold stroke data (coordinates and pen state)
struct Stroke {
    int x;         // X-coordinate of the stroke
    int y;         // Y-coordinate of the stroke
    int pen_down;  // Pen state: 1 for down, 0 for up
};

// Structure to represent a character's stroke data
struct Character {
    int ascii_code;   // ASCII code of the character
    int stroke_count; // Number of strokes for the character
    struct Stroke* strokes;  // Dynamic array of strokes
};

// Global font array to store data for all ASCII characters
struct Character font[128];

void load_font(const char* filename) {
    // Attempt to open the font file in read mode
    FILE* file = fopen("SingleStrokeFont.txt", "r");
    if (file == NULL) {
        // If the file cannot be opened, print an error message and exit the function
        printf("\nThe file could not be opened for reading, exiting.");
        return; // Return to avoid continuing with an invalid file
    }

    int ascii, stroke_count; // Variables to hold ASCII code and number of strokes for each character

    // Loop to read character data from the file until no more valid lines are found
    while (fscanf(file, "999 %d %d", &ascii, &stroke_count) == 2) {
        // Read the character's ASCII code and the number of strokes from the file
        // Example: For "999 72 7", ascii = 72 ('H'), stroke_count = 7

        // Store the ASCII code in the corresponding entry of the font array
        font[ascii].ascii_code = ascii;

        // Store the number of strokes in the corresponding entry
        font[ascii].stroke_count = stroke_count;

        // Allocate memory dynamically to hold the strokes for this character
        // Each stroke requires a structure with x, y coordinates and pen state
        font[ascii].strokes = malloc(sizeof(struct Stroke) * stroke_count);

        // Check if memory allocation was successful
        if (font[ascii].strokes == NULL) {
            printf("\nError allocating memory for strokes. Exiting.");
            fclose(file); // Close the file before exiting
            exit(EXIT_FAILURE); // Exit the program with an error code
        }

        // Loop through the number of strokes and read each stroke's data
        for (int i = 0; i < stroke_count; i++) {
            // Read x, y, and pen_down for each stroke
            // Example line: "0 0 0" means x=0, y=0, pen up (pen_down=0)
            fscanf(file, "%d %d %d",
                   &font[ascii].strokes[i].x,       // Store x-coordinate
                   &font[ascii].strokes[i].y,       // Store y-coordinate
                   &font[ascii].strokes[i].pen_down); // Store pen state (0=up, 1=down)
        }
    }
}