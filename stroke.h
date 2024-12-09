#ifndef STROKE_H
#define STROKE_H

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

// Function declarations
void scale_character(struct Character* character, float scale_factor);
void free_character(struct Character* character);

#endif
