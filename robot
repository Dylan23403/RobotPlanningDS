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