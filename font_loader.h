#ifndef FONT_LOADER_H
#define FONT_LOADER_H

#include "stroke.h"

// Global font array
extern struct Character font[128];

// Function declarations
void load_font(const char* filename);
void free_font_data();

#endif // FONT_LOADER_H