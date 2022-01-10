#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

#include "navball.h"

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

float roll, pitch, yaw;
Navball navball;

unsigned long start_time, end_time;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Simpit NavBall test"));
  tft.reset();

  roll = 0;
  pitch = 0;
  yaw = 0;

  tft.begin(0x8357);

  tft.fillScreen(RED);
  tft.fillScreen(BLACK);
}

void loop()
{
  roll += 5;
  pitch += 2;
  yaw += 3;

  navball.set_rpy(roll, pitch, yaw);

  start_time = millis();
  navball.draw(&tft);
  end_time = millis();

  // Draw the time it took to compute a frame, in ms
  tft.setCursor(0, 0);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.print(end_time - start_time);

  delay(200);
}
