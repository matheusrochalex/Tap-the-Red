/*
 * Matheus Alexandre
 * CS 298-02; Fall 2018
 * 
 * Final project code. This is the software for a simple game I call "Tap the Red".
 * The 4x4 LED display will spin and change colors, showing RED, GREEN, BLUE, and YELLOW.
 * Everytime the user hits the switch when the color RED is displayed, the game gets faster.
 * The goal is to get to the fastest level, displayed on the 7-segment display as F.
 */

// Include the library
#include <RGB.h>
#include <SPI.h>

// Defining variables
#define RGBPin 2 // RGB display
#define SwitchPin 3 // The switch we will Tap

#define Correct 9 // LED to blink when the red is tapped

#define numPixels 16 // number of LEDs in display

// 7-segment display section
#define SPIClockPin 13
#define SPIMISOPin 12
#define SPIMOSIPin 11
#define SPISelectPin 10

// Create an RGB object
RGB rgb(RGBPin, numPixels); 

byte pixels[6] = {7, 0, 1, 2, 3, 4}; // starting values for the C shape on LED display
byte jump[16] = {1, 2, 3, 4, 11, -1, -1, 0, 7, -1, -1, 12, 13, 14, 15, 8}; // indeces tells us where to jump to

// Hexadecimal digit to segment mapping
volatile byte segmentMap[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

// Declaration for the interrupt function
volatile bool switchMe = true;
void guess_ISR();

// RGB colors
byte green = 255;
byte red = 255;
byte blue = 255;

// This value only goes up to 4, it'll select the colors as it goes up
byte countColor = 0;

// This value determines the difficulty of the game
byte countSeg = 0;

// The delayVal determines the speed/difficulty
byte delayVal = 128;

void setup() 
{
  rgb.clear(); // always start the LEDs clear

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  pinMode(SPISelectPin, OUTPUT);

  pinMode(SwitchPin, INPUT_PULLUP);

  pinMode(Correct, OUTPUT);

  attachInterrupt(1, guess_ISR, FALLING);

}

void loop() 
{
  rgb.clear(); // make sure to clear the display every iteration

  // Loop that runs the colors in circles
  for (int i = 0; i < 6; i++)
  {
    rgb.setColor(pixels[i], green, red, blue);
    pixels[i] = jump[pixels[i]];
  }

  rgb.show();

  delay(delayVal);

  // This changes the color of the display according to "countColor"
  switch(countColor)
  {
    case 0: // if 0: red
    {
      green = 0;
      red = 255;
      blue = 0;
      break;
    }
    case 1: // if 1: blue
    {
      green = 0;
      red = 0;
      blue = 255;
      break;
    }
    case 2: // if 2: green
    {
      green = 255;
      red = 0;
      blue = 0;
      digitalWrite(Correct, LOW); // make sure to turn off the LED after a cycle
      break;
    }
    case 3: // if 3: yellow
    {
      green = 255;
      red = 255;
      blue = 12;
      break;
    }
  }

  // If the button is pressed when the color is RED and the segment is less than 16
  if (switchMe && countSeg < 16 && countColor == 0)
  {
    countSeg++; // increase the difficulty
    delayVal -= 8; // increase the speed
    digitalWrite(Correct, HIGH); // flash the green LED
    switchMe = !switchMe; // reset the switch
  }

  updateShiftRegister(segmentMap[countSeg]);

  // Make sure to reset the game when the final level is beat
  if (countSeg >= 16)
  {
    countSeg = 0;
    delayVal = 128;
  }

} // end the loop function here

// Function for the 7-segment display
void updateShiftRegister(byte segments)
{
  digitalWrite(SPISelectPin, LOW);
  SPI.transfer(segments);
  digitalWrite(SPISelectPin, HIGH);
}

// Function for the interrupt
void guess_ISR()
{
  switchMe = !switchMe;
}
