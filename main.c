#include <stdio.h>
#include <stdlib.h>

#define MAC

#ifdef MAC
#include <unistd.h>
#else
#include <Windows.h>
#include "rs232.h"
#include <conio.h>
#endif

#include "serial.h"
#include "font_loader.h"
#include "gcode_generator.h"

#define bdrate 115200 /* 115200 baud */

void SendCommands(char *buffer);

int main(void) {
    char buffer[100];
    char text_filename[100];
    float text_height;

    // Get input file name and text height from the user
    printf("Enter the name of the text file to process: ");
    scanf("%99s", text_filename);
    printf("Enter the desired letter height (4-10 mm): ");
    scanf("%f", &text_height);

    // Validate text height
    if (text_height < 4.0 || text_height > 10.0) {
        printf("Error: Text height must be between 4 and 10 mm.\n");
        exit(1);
    }

    // If we cannot open the port then give up immediately
    if (CanRS232PortBeOpened() == -1) {
        printf("\nUnable to open the COM port (specified in serial.h)");
        exit(0);
    }

    // Time to wake up the robot
    printf("\nAbout to wake up the robot\n");

    // We do this by sending a new-line
    sprintf(buffer, "\n");
    PrintBuffer(&buffer[0]);
    #ifdef MAC
    sleep(1);  // Wait for 1 second
    #else
    Sleep(100); // Wait for 100 ms
    #endif

    // This is a special case - we wait until we see a dollar ($)
    WaitForDollar();

    printf("\nThe robot is now ready to draw\n");

    // Ensure the pen is up and move to the origin (0,0) at the start
    sprintf(buffer, "S0\n");          // Lift the pen
    SendCommands(buffer);
    sprintf(buffer, "G0 X0 Y0\n");    // Move to the origin (0,0)
    SendCommands(buffer);

    // Load the font data
    load_font();

    // Process the text file and generate G-code dynamically
    process_text_via_rs232(text_filename, text_height);

    // Free the font data memory
    free_font_data();

    // Lift the pen and move to the origin (0,0) at the end
    sprintf(buffer, "S0\n");          // Lift the pen
    SendCommands(buffer);
    sprintf(buffer, "G0 X0 Y0\n");    // Move to the origin (0,0)
    SendCommands(buffer);

    // Before we exit the program, close the COM port
    CloseRS232Port();
    printf("COM port now closed\n");

    return 0;
}

// Send the data to the robot
void SendCommands(char *buffer) {
    PrintBuffer(&buffer[0]);
    WaitForReply();
    #ifdef MAC
    sleep(1); // 1-second delay
    #else
    Sleep(100); // 100ms delay
    #endif
}
