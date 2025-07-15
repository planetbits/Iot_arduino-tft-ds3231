#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <RTClib.h>

#define BLACK   0x0000
#define WHITE   0xFFFF
#define YELLOW  0xFFE0
#define CYAN    0x07FF

MCUFRIEND_kbv tft;
RTC_DS3231 rtc;

unsigned long lastUpdate = 0;  // for timing
const unsigned long updateInterval = 10000; // 10 seconds

void setup() {
  Serial.begin(9600);
  delay(1000);

  uint16_t ID = tft.readID();
  Serial.print("TFT ID: 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3 || ID == 0xFFFF || ID == 0x0000) {
    ID = 0x9341;  // most common fallback
    Serial.println("Using fallback ID: 0x9341");
  }

  tft.begin(ID);
  tft.setRotation(1);  // Landscape
  tft.fillScreen(BLACK);

  tft.setTextSize(2);
  tft.setTextColor(CYAN, BLACK);
  tft.setCursor(10, 10);
  tft.print("Digital Clock");

  if (!rtc.begin()) {
    tft.setCursor(10, 50);
    tft.setTextColor(WHITE, BLACK);
    tft.println("RTC Error");
    while (1);
  }

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastUpdate >= updateInterval) {
    lastUpdate = currentMillis;

    DateTime now = rtc.now();

    int hour12 = now.hour() % 12;
    if (hour12 == 0) hour12 = 12;
    const char* ampm = (now.hour() >= 12) ? "PM" : "AM";

    char timeStr[20];
    sprintf(timeStr, "%02d:%02d:%02d %s", hour12, now.minute(), now.second(), ampm);

    char dateStr[20];
    sprintf(dateStr, "%02d-%02d-%04d", now.day(), now.month(), now.year());

    // Clear area before updating
    tft.fillRect(10, 50, 300, 90, BLACK);

    tft.setTextSize(3);
    tft.setTextColor(YELLOW, BLACK);
    tft.setCursor(20, 60);
    tft.print(timeStr);

    tft.setTextSize(2);
    tft.setTextColor(WHITE, BLACK);
    tft.setCursor(20, 100);
    tft.print("Date: ");
    tft.print(dateStr);

    Serial.print("Updated at: ");
    Serial.println(timeStr);
  }

  // Do nothing until next interval
}
