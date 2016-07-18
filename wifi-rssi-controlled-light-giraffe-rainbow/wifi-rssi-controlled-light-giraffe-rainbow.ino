/*
 * Source code of the wireless intensity controlled light for Design
 * student Federica Gasperat.
 *
 * A project in collaboration with makerspace - Faculty of Computer Science 
 * at the Free University of Bozen-Bolzano.
 * 
 *                                                                         
 *    m  a  k  e  r  s  p  a  c  e  .  i  n  f  .  u  n  i  b  z  .  i  t  
 *                                                                         
 *   8888888888888888888888888888888888888888888888888888888888888888888888
 *                                                                         
 *                  8                                                      
 *                  8                                                      
 *   YoYoYo. .oPYo. 8  .o  .oPYo. YoYo. .oPYo. 8oPYo. .oPYo. .oPYo. .oPYo. 
 *   8' 8' 8 .oooo8 8oP'   8oooo8 8  `  Yb..`  8    8 .oooo8 8   `  8oooo8 
 *   8  8  8 8    8 8 `b.  8.  .  8      .'Yb. 8    8 8    8 8   .  8.  .  
 *   8  8  8 `YooP8 8  `o. `Yooo' 8     `YooP' 8YooP' `YooP8 `YooP' `Yooo' 
 *                                             8                           
 *                                             8                           
 *                                                                         
 *   8888888888888888888888888888888888888888888888888888888888888888888888
 *                                                                         
 *    c  o  m  p  u  t  e  r    s  c  i  e  n  c  e    f  a  c  u  l  t  y 
 *                                                                         
 *                                                                         
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Julian Sanin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <FastLED.h>

#include <ESP8266WiFi.h>
#include <limits.h>
#include <stdint.h>

/*******************************************************************************
                       ,----.
                      /      \
                     | WS2811 |      
                    _|        |
 Adafruit          |__________|
 Huzzah ESP8266       | | | |
                  DIN | | | | DOUT
 5, DOUT  o-----------' | | '------o
                   VCC  | |
                    +---' |
                         ===
                         GND

(created by AACircuit v1.28.6 beta 04/19/05 www.tech-chat.de)
*******************************************************************************/

enum ESP8266Configuration {
  LED_PIN = 5,
  LED_COUNT = 9,
  LED_COLOR = CRGB::White,
  LED_VOLTAGE = 3,   // Use HUZZAH onboard 3.3V voltage regulator.
  LED_CURRENT = 500, // Max. LED current from 3.3V regulator in mA.
  LED_HUE_COUNT = 4,
  LED_START_HUE = 0,
  LED_HUE_CHANGE = (256 / LED_HUE_COUNT),
  RSSI_MIN = -80, // dBm
  RSSI_MAX = -20, // dBm
  RSSI_MAX_LEVEL = 8
};

// Return RSSI or INT_MIN.
int32_t getRSSI() {
  byte available_networks = WiFi.scanNetworks();
  int rssi = INT_MIN;
  for (int network = 0; network < available_networks; network++) {
    int new_rssi = WiFi.RSSI(network);
    if(rssi < new_rssi) {
      rssi = new_rssi;
    }
  }
  return rssi;
}

void blinkBuiltinLed() {
  static bool isInitialized = false;
  if(!isInitialized) {
    pinMode(LED_BUILTIN, OUTPUT);
    isInitialized = true;
  }
  digitalWrite(LED_BUILTIN, LOW);
  delay(1);
  digitalWrite(LED_BUILTIN, HIGH);
}

CRGB leds[LED_COUNT];

// set light bar length from 0 to 3.
void setLightBar(uint8_t intensity) {
  FastLED.clear();
  if(intensity >= LED_COUNT) {
    intensity = LED_COUNT;
  }
  fill_rainbow(&leds[1], intensity, LED_START_HUE, LED_HUE_CHANGE);
  FastLED.show();
}

// set intensity from 0 to 7.
void setLightIntensity(uint8_t intensity) {
  fill_solid(leds, 1, LED_COLOR);
  int brightness = (1 << (intensity + 1)) - 1;
  if (brightness > 255) {
    brightness = 255;
  }
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void setup() {
  FastLED.addLeds<WS2811, LED_PIN>(leds, LED_COUNT);
  FastLED.setCorrection(Typical8mmPixel);
  FastLED.setTemperature(Tungsten40W);
  FastLED.setMaxPowerInVoltsAndMilliamps(LED_VOLTAGE, LED_CURRENT);
  fill_solid(leds, LED_COUNT, LED_COLOR);
  fill_rainbow(&leds[1], LED_HUE_COUNT, LED_START_HUE, LED_HUE_CHANGE);
  FastLED.setBrightness(100);
  FastLED.show();
  //pinMode(LED_BUILTIN, OUTPUT);
  //Serial.begin(115200);
}

void loop() {
  int32_t rssi = getRSSI();
  blinkBuiltinLed();
  /*
  Serial.print("Signal strength: ");
  Serial.print(rssi);
  Serial.println("dBm");
  */
  if(rssi != INT_MIN) {
    //int intensity = (uint8_t)(map(rssi, RSSI_MIN, RSSI_MAX, RSSI_MAX_LEVEL, 0)); // Inverted
    int intensity = (uint8_t)(map(rssi, RSSI_MIN, RSSI_MAX, 0, RSSI_MAX_LEVEL));   // Normal
    //int length = (uint8_t)(map(rssi, RSSI_MIN, RSSI_MAX, LED_HUE_COUNT, 0)); // Inverted
    int length = (uint8_t)(map(rssi, RSSI_MIN, RSSI_MAX, 0, LED_HUE_COUNT));   // Normal
    setLightBar(length);
    setLightIntensity(intensity);
  }
}
