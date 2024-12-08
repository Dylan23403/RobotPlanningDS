#include <stdio.h>
#include <stdlib.h>
#include "serial.h"
#include "gcode_generator.h" // Include your G-code generator header
#include "font_loader.h" // Include font loader for character strokes

void SendCommands(char *buffer);

int main() {
    char buffer[100];
    char text_filename[100];
    float text_height;

    // Prompt user for input text file and text height
    printf("Enter the name of the text file to process: ");
    scanf("%99s", text_filename);
    printf("Enter the desired letter height (4-10 mm): ");
    scanf("%f", &text_height);

    // Validate text height
    if (text_height < 4.0 || text_height > 10.0) {
        printf("Error: Text height must be between 4 and 10 mm.\n");
        return 1;
    }

    // Open the RS232 port
    if (CanRS232PortBeOpened() == -1) {
        printf("Unable to open the COM port.\n");
        return 1;
    }

    printf("Robot is waking up...\n");
    sprintf(buffer, "\n");
    PrintBuffer(buffer);
    WaitForDollar();
    printf("Robot is ready to draw.\n");

    // Load the font data
    load_font("SingleStrokeFont.txt");

    // Process the text file and send G-code
    process_text_via_rs232(text_filename, text_height);

    // Free font data memory
    free_font_data();

    // Close the RS232 port
    CloseRS232Port();
    printf("COM port closed. Program complete.\n");

    return 0;
}

// Send the data to the robot
void SendCommands(char *buffer) {
    PrintBuffer(buffer);
    WaitForReply();
}
