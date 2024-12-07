#include <stdlib.h>
#include "stroke.h"

// Function to scale a character's strokes based on the desired height
void scale_character(struct Character* character, float scale_factor) 
{
    for (int i = 0; i < character->stroke_count; i++) {
        character->strokes[i].x = (int)(character->strokes[i].x * scale_factor);
        character->strokes[i].y = (int)(character->strokes[i].y * scale_factor);
    }
}
// Free the strokes of a character
void free_character(struct Character* character) 
{
    if (character->strokes != NULL) {
        free(character->strokes);
        character->strokes = NULL;
    }
}