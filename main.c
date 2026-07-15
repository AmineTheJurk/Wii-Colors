#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void *frameBuffer;
static GXRModeObj *rmode;

int main(int argc, char **argv) {
    // 1. Video initialization on the Wii
    VIDEO_Init();
    rmode = VIDEO_GetPreferredMode(NULL);
    frameBuffer = SYS_AllocateFramebuffer(rmode);
    
    // Initialize the console library for text output
    CON_Init(frameBuffer, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(frameBuffer);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    // 2. Controller initialization (Wiimote)
    WPAD_Init();

    // Array of color values accepted by the Wii (YUV format)
    // Colors order: Red, Blue, Green, Yellow
    u32 colors[] = {
        COLOR_RED,
        COLOR_BLUE,
        COLOR_GREEN,
        COLOR_YELLOW
    };
    int colorIndex = 0;
    int clickCount = 0; // Click counter variable

    // Apply the first background color (Red) on startup
    VIDEO_ClearFrameBuffer(rmode, frameBuffer, colors[colorIndex]);

    // 3. Main application loop
    while(1) {
        // Refresh controller states
        WPAD_ScanPads();
        
        // Get pressed buttons on Wiimote 1
        u32 buttonsDown = WPAD_ButtonsDown(0);

        // Clear previous text and refresh info every frame
        printf("\x1b[2;0H"); // Reset cursor position to top of screen
        printf("   Wii Application - Color Changer\n");
        printf("   Press A to change the background color.\n");
        printf("   Total clicks: %d\n", clickCount); // Display the counter
        printf("   Press HOME to exit.\n");

        // If the A button is pressed
        if (buttonsDown & WPAD_BUTTON_A) {
            colorIndex = (colorIndex + 1) % 4; // Cycle through 0 to 3
            clickCount++;                     // Increment counter by 1
            
            // Update the screen with the new background color
            VIDEO_ClearFrameBuffer(rmode, frameBuffer, colors[colorIndex]);
        }

        // If the HOME button is pressed, exit the application safely
        if (buttonsDown & WPAD_BUTTON_HOME) {
            break;
        }

        // Wait for screen refresh to avoid flickering
        VIDEO_WaitVSync();
    }

    return 0;
}
