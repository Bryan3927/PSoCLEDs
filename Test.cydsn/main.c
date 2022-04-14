/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>

// Create LED struct
typedef struct{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t white;
} LED;

// Function definitions
void SendPixels(LED* buffer, uint8_t pixel_count);
void SendPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void SendHighBit();
void SendLowBit();
void SendByte(uint8_t byte);
void SendReset();

// Define state constants/variables
#define OUT_ON 1
#define OUT_OFF 0
uint8_t state = 0;

int main()
{
    PWM_1_Start();
    LED buffer[50]; // Initialize LED buffer to hold 50 LEDs
    
    // Initialize primary LED colors
    LED white = {.red = 0, .green = 0, .blue = 0, .white = 255};
    LED red = {.red = 255, .green = 0, .blue = 0, .white = 0};
    LED green = {.red = 0, .green = 255, .blue = 0, .white = 0};
    LED blue = {.red = 0, .green = 0, .blue = 255, .white = 0};
    LED off = {.red = 0, .green = 0, .blue = 0, .white = 0};
    
    // Used for state switching
    uint8_t last = 0;
    uint8_t current = 0;
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    uint8_t index;

    for(;;)
    {
        /* Place your application code here. */
        
        // Sets 40 LEDs to off except the one at index state
        for (index = 0; index < 40; index++) {
            buffer[index] = (state == index) ? red : off;
        }
        
        // Sends Pixels
        SendPixels(buffer, 40);
        
        // Changes state based on square wave
        current = Pin_1_Read();
        if (current != last) {
            last = current;
            state++;
            if (state > 39) state = 0;
        }
        
    }
}

/*
 Sends pixel_count amount of buffered pixels to selected output Pin
*/
void SendPixels(LED* buffer, uint8_t pixel_count) {
    uint8_t index;
    for (index = 0; index < pixel_count; index++) {
        SendPixel(
        buffer[index].red,
        buffer[index].green,
        buffer[index].blue,
        buffer[index].white);
    }
    SendReset();
}

/*
 Sends RGBW of pixel to selected output Pin

 Currently sends it in GRBW order. Some LEDs might use RGBW instead
*/
void SendPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    SendByte(g);
    SendByte(r);
    SendByte(b);
    SendByte(w);
}

/*
 Sends a single Byte to selected output Pin
*/
void SendByte(uint8_t byte) {
    byte & 128 ? SendHighBit() : SendLowBit();
    byte & 64 ? SendHighBit() : SendLowBit();
    byte & 32 ? SendHighBit() : SendLowBit();
    byte & 16 ? SendHighBit() : SendLowBit();
    byte & 8 ? SendHighBit() : SendLowBit();
    byte & 4 ? SendHighBit() : SendLowBit();
    byte & 2 ? SendHighBit() : SendLowBit();
    byte & 1 ? SendHighBit() : SendLowBit();
}

/*
 Sends a single High bit to selected output Pin
*/
void SendHighBit() {
    LED_STRIP_Write(OUT_ON);
    CyDelayCycles(10);
    LED_STRIP_Write(OUT_OFF);
}

/*
 Sends a single Low bit to selected output Pin
*/
void SendLowBit() {
    LED_STRIP_Write(OUT_ON);
    LED_STRIP_Write(OUT_OFF);
    CyDelayCycles(10);
}

/*
 Sends the reset signal to selected output Pin

 Used to end a sequence of output and restart from the first LED
*/
void SendReset() {
    LED_STRIP_Write(OUT_OFF);
    CyDelayUs(80);
}

/* [] END OF FILE */
