/* KerbalSimpitNavball
   A demonstration of drawing a Navball to connected to a KSP instance

   Use a Adafruit_TFTLCD with a 8-bit parallel interface. See this tutorial : https://learn.adafruit.com/adafruit-2-8-and-3-2-color-tft-touchscreen-breakout-v2/pinouts
*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

#include <KerbalSimpit.h>

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

KerbalSimpit mySimpit(Serial);

KerbalNavball navball;

void setup() {
  Serial.begin(115200);

  // Set up the build in LED, and turn it on.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  tft.reset();

  tft.begin(0x8357);

  // To check the connection to the screen, fill the screen in red then black
  tft.fillScreen(RED);
  tft.fillScreen(BLACK);

  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);
  tft.println("Connection ...");

  //Connect to Simpit
  while (!mySimpit.init()) {
    delay(100);
  }

  digitalWrite(LED_BUILTIN, LOW);

  mySimpit.printToKSP("Navball Connected", PRINT_TO_SCREEN);

  mySimpit.inboundHandler(messageHandler);
  mySimpit.registerChannel(ROTATION_DATA);
  mySimpit.registerChannel(MANEUVER_MESSAGE);
  mySimpit.registerChannel(TARGETINFO_MESSAGE);
}

void loop()
{ 
  mySimpit.update();

  /*
  navball.set_rpy(roll, pitch, yaw);
  navball.set_target(45, 30);
  navball.set_maneuver(15, -30);
  navball.set_speed_orientation(-20, -20, -40, -40, -40, 40);
  */

  navball.draw(&tft);

  delay(200);
}

void messageHandler(byte messageType, byte msg[], byte msgSize) {
  switch(messageType) {
    case ROTATION_DATA:
      if (msgSize == sizeof(vesselPointingMessage)) {
        vesselPointingMessage rotMsg;
        rotMsg = parseMessage<vesselPointingMessage>(msg);
        navball.set_rpy(rotMsg.roll, rotMsg.pitch, rotMsg.heading);
        navball.set_speed_orientation(rotMsg.orbitalVelocityHeading, rotMsg.orbitalVelocityPitch, -40, -40, -40, 40);
      }
      break;
    case MANEUVER_MESSAGE:
      if (msgSize == sizeof(maneuverMessage)) {
        maneuverMessage manMsg;
        manMsg = parseMessage<maneuverMessage>(msg);
        navball.set_maneuver(manMsg.headingNextManeuver, manMsg.pitchNextManeuver);
      }
      break;
    case TARGETINFO_MESSAGE:
      if (msgSize == sizeof(targetMessage)) {
        targetMessage targetMsg;
        targetMsg = parseMessage<targetMessage>(msg);
        navball.set_maneuver(targetMsg.heading, targetMsg.pitch);
      }
      break;
  }
}
