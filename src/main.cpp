#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <Adafruit_MCP23017.h>
#include <Rotary.h>
#include <RotaryEncOverMCP.h>

#define OLED_SCK D5
#define OLED_SDA D7
#define OLED_RST D6
#define OLED_DC D8
#define OLED_CS D0

#define BLACK 0x0000
#define WHITE 0xFFFF

void RotaryEncoderChanged(bool antiClock, int id);

Adafruit_SSD1331 display = Adafruit_SSD1331(OLED_CS, OLED_DC, OLED_RST);
Adafruit_MCP23017 mcp;
Adafruit_MCP23017 *mcpr = &mcp;
RotaryEncOverMCP rotaryEncoder = RotaryEncOverMCP(&mcp, 7, 6, &RotaryEncoderChanged);

int value = 0;
int curValue = -1;
int presses = 0;
int curPresses = -1;
int butStatus = 0;
int curButStatus = -1;

void setup()
{
  Serial.begin(9600);
  Serial.println("Initializing OLED...");

  Wire.begin(D2, D1);
  Wire.beginTransmission(0x20);
  int res = Wire.endTransmission();
  Serial.print("Result: ");
  Serial.println(res);

  mcp.begin();
  display.begin();
  Serial.println("Done!");

  rotaryEncoder.init();
  display.fillScreen(BLACK);

  // mcpr->pullUp(5, 1);
  mcpr->pinMode(5, INPUT);

  display.fillScreen(BLACK);
  display.setTextColor(WHITE);
  display.setTextSize(1);
}

void draw()
{
  if (curValue != value || curPresses != presses)
  {
    Serial.println("Painting...");

    display.setCursor(2, 1);
    display.print("Rotary ");
    if (curValue != value)
    {
      display.setTextColor(BLACK);
      display.print(curValue);
      curValue = value;
      display.setTextColor(WHITE);
      display.setCursor(2, 1);
      display.print("Rotary ");
    }
    display.print(value);

    display.setCursor(2, 11);
    display.print("Presses ");
    if (curPresses != presses)
    {
      display.setTextColor(BLACK);
      display.print(curPresses);
      curPresses = presses;
      display.setTextColor(WHITE);
      display.setCursor(2, 11);
      display.print("Presses ");
    }
    curPresses = presses;
    display.print(presses);
  }
}

void loop()
{
  rotaryEncoder.poll();
  int butStatus = mcpr->digitalRead(5);
  if (curButStatus != butStatus)
  {
    curButStatus = butStatus;
    if (butStatus == 0)
    {
      Serial.println("Button pressed");
      presses++;
    }
  }

  draw();
}

void RotaryEncoderChanged(bool antiClock, int id)
{
  Serial.println("Encoder " + String(id) + ": " + (antiClock ? String("left") : String("right")));
  value += antiClock ? -1 : 1;
}
