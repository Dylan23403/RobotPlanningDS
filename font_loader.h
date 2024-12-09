#ifndef FONT_LOADER_H
#define FONT_LOADER_H

#include "stroke.h"

// Global font array
extern struct Character font[128];

// Function declarations
void load_font(void);
void free_font_data(void);

#endif // FONT_LOADER_H
