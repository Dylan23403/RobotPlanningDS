#ifndef FONT_GCODE_H
#define FONT_GCODE_H

// Structure Definitions

// Represents a single stroke for drawing a character
struct Stroke {
    int x;          // X-coordinate of the stroke
    int y;          // Y-coordinate of the stroke
    int pen_down;   // 1 if the pen is down, 0 if the pen is up
};

// Represents a character with its associated strokes
struct Character {
    int ascii_code;             // ASCII code of the character
    int stroke_count;           // Number of strokes for the character
    struct Stroke* strokes;     // Array of strokes
};

// Global array to hold the font data for ASCII characters (0-127)
struct Character font[128];

// Function Declarations

// Loads font data from the file `SingleStrokeFont.txt` into the global `font` array.
void load_font(void);

// Frees the memory allocated for font data in the global `font` array.
void free_font_data(void);

// Reads the text file, processes each word, and generates G-code dynamically.
// Sends G-code commands to the robot for each word processed.
void process_text_via_rs232(const char* text_filename, float text_height);

#endif
