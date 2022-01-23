#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

#include <KerbalSimpit.h>

#include <navball.h>

#define CONNECT_TO_SIMPIT false
#define USE_GFXcanvas true

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
//GFXcanvas1 canvas(300, 300);

KerbalSimpit mySimpit(Serial);

float roll, pitch, yaw;
Navball navball;

unsigned long start_time, end_time;

void setup() {
  if(CONNECT_TO_SIMPIT) Serial.begin(115200);

  // Set up the build in LED, and turn it on.
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  tft.reset();

  roll = 0;
  pitch = 0;
  yaw = 0;

  tft.begin(0x8357);

  tft.fillScreen(RED);
  tft.fillScreen(BLACK);

  if(CONNECT_TO_SIMPIT){
    //Connect to Simpit
    while (!mySimpit.init()) {
      delay(100);
    }

    digitalWrite(LED_BUILTIN, LOW);

    mySimpit.printToKSP("Navball Connected", PRINT_TO_SCREEN);

    mySimpit.inboundHandler(messageHandler);
    mySimpit.registerChannel(ROTATION_DATA);
  }
}

void loop()
{ 
  if(CONNECT_TO_SIMPIT) {
    mySimpit.update();
  } else {
    roll += 5;
    pitch += 2;
    yaw += 3;
  }
  navball.set_rpy(roll, pitch, yaw);

  start_time = millis();
  navball.draw(&tft);
  //navball.draw(&canvas);
  //tft.drawBitmap(0, 0, canvas.getBuffer(), 300, 300, BLACK, WHITE);
  end_time = millis();

  // Draw the time it took to compute a frame, in ms
  tft.setCursor(0, 0);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.print(end_time - start_time);

  delay(200);
}

void messageHandler(byte messageType, byte msg[], byte msgSize) {
  switch(messageType) {
  case ROTATION_DATA:
    if (msgSize == sizeof(vesselPointingMessage)) {
      vesselPointingMessage rotMsg;
      rotMsg = parseMessage<vesselPointingMessage>(msg);
      roll = rotMsg.roll;
      pitch = rotMsg.pitch;
      yaw = rotMsg.heading;
    }
    break;
  }
}
