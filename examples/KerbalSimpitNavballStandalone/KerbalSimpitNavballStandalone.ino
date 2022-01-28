/* KerbalSimpitNavballStandalone
   A demonstration of drawing a moving Navball to illustrate the functionnality without 
   requiring a connection to a KSP instance

   Use a Adafruit_TFTLCD with a 8-bit parallel interface. See this tutorial : https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2/pinouts
*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

#include <KerbalNavball.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

float roll, pitch, yaw;
KerbalNavball navball;

void setup() {
  // Set up the build in LED, and turn it on.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  tft.reset();

  // Set the initial position of the navball
  roll = 0;
  pitch = 0;
  yaw = 0;
  navball.set_rpy(roll, pitch, yaw);

  tft.begin(0x8357);

  // To check the connection, fill the screen in red then black
  tft.fillScreen(RED);
  tft.fillScreen(BLACK);
}

void loop()
{ 
  // Set a rotation at each frame
  roll += 5;
  pitch += 2;
  yaw += 3;
  
  navball.set_rpy(roll, pitch, yaw);
  navball.set_target(45, 30);
  navball.set_maneuver(15, -30);
  navball.set_speed_orientation(-20, -20, -40, -40, -40, 40);

  navball.draw(&tft);

  delay(200);

  // Uncomment this line to freeze the display
  // while(1){}
}
