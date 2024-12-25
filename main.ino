#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <EEPROM.h>

#define MAX 10
#define MAX_CANDLE 50

#define PIN 12
#define NUM 150

/**              *
 *   1  2  3  4  |  5  6  7  8
 *   \  \  \  \  |  /  /  /  /
 *    \  \  \  \ | /  /  /  /
 *     \  \  \  \|/  /  /  /
 *      \  \  \  |  /  /  /
 *       \  \  \ | /  /  /
 *        \  \  \|/  /  /
 *         \  \  |  /  /
 *          \  \ | /  /
 *           \  \|/  /
 *            \  |  /
 *             \ | /
 *              \|/
 *               |
 *      _________|_________
 *      
 * Base:
 *  Right:  0
 *  Left:  19
 * Stem:
 *  Bottom: 20
 *  Top:    40
 *  LED:    41
 * 1:
 *  Bottom: 128
 *  Top:    109
 *  LED:    108
 * 2:
 *  Bottom: 92
 *  Top:    106
 *  LED:    107
 * 3:
 *  Bottom: 64
 *  Top:    55
 *  LED:    54
 * 4:
 *  Bottom: 48
 *  Top:    52
 *  LED:    53
 * 5:
 *  Bottom: 47
 *  Top:    43
 *  LED:    42
 * 6:
 *  Bottom: 65
 *  Top:    74
 *  LED:    75
 * 7:
 *  Bottom: 91
 *  Top:    77
 *  LED:    76
 * 8:
 *  Bottom: 129
 *  Top:    148
 *  LED:    149
 *  
 * Bottom of 1 and 8 at stem: 2
 * Bottom of 2 and 7 at stem: 6
 * Bottom of 3 and 6 at stem: 11
 * Bottom of 4 and 5 at stem: 16
 */
#define BASE_1_8_AT_STEM 2
#define BASE_2_7_AT_STEM 6
#define BASE_3_6_AT_STEM 11
#define BASE_4_5_AT_STEM 16

#define BOTTOM_STEM 20
#define LEN_STEM    21
#define STEP_STEM    1
#define LED_STEM    41

#define BOTTOM_1 128
#define LEN_1     20
#define STEP_1    -1
#define LED_1    108

#define BOTTOM_2  92
#define LEN_2     15
#define STEP_2     1
#define LED_2    107

#define BOTTOM_3  64
#define LEN_3     10
#define STEP_3    -1
#define LED_3     54 

#define BOTTOM_4  48
#define LEN_4      5
#define STEP_4     1
#define LED_4     53

#define BOTTOM_5  47
#define LEN_5      5
#define STEP_5    -1
#define LED_5     42

#define BOTTOM_6  65
#define LEN_6     10
#define STEP_6     1
#define LED_6     75

#define BOTTOM_7  91
#define LEN_7     15
#define STEP_7    -1
#define LED_7     76

#define BOTTOM_8 129
#define LEN_8     20
#define STEP_8     1
#define LED_8    149

// days go from left to right :facepalm:
const int bottomForDay[9] = {
  0,
  BOTTOM_8,
  BOTTOM_7,
  BOTTOM_6,
  BOTTOM_5,
  BOTTOM_4,
  BOTTOM_3,
  BOTTOM_2,
  BOTTOM_1
};

const int lenForDay[9] = {
  0,
  LEN_8,
  LEN_7,
  LEN_6,
  LEN_5,
  LEN_4,
  LEN_3,
  LEN_2,
  LEN_1
};

const int stepForDay[9] = {
  0,
  STEP_8,
  STEP_7,
  STEP_6,
  STEP_5,
  STEP_4,
  STEP_3,
  STEP_2,
  STEP_1
};

const int ledForDay[9] = {
  0,
  LED_8,
  LED_7,
  LED_6,
  LED_5,
  LED_4,
  LED_3,
  LED_2,
  LED_1
};

const int baseAtStemForDay[9] = {
  0,
  BASE_1_8_AT_STEM,
  BASE_2_7_AT_STEM,
  BASE_3_6_AT_STEM,
  BASE_4_5_AT_STEM,
  BASE_4_5_AT_STEM,
  BASE_3_6_AT_STEM,
  BASE_2_7_AT_STEM,
  BASE_1_8_AT_STEM
};

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

byte eepromDay;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // read eeprom day
  eepromDay = EEPROM.read(0);
  eepromDay = eepromDay + 1;
  if (eepromDay >= 9) {
    eepromDay = 1;
  }
  delay(100);
  EEPROM.write(0, eepromDay);
}

void loop() {
  menorahForDay(eepromDay);
}

uint32_t colorForIndexAndIter(int index, int iter) {
  const uint32_t slowdown = 10;
  iter = iter / slowdown;
  uint32_t colorWhite = strip.Color(MAX, MAX, MAX);
  uint32_t colorYellow = strip.Color(MAX, MAX * 3 / 5, 0);
  if (iter % 4 == index % 4) {
    return colorYellow;
  }
  return colorWhite;
}

void menorahForDay(int day) {
  // first, clear
  uint32_t black = strip.Color(0, 0, 0);
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, black);
  }
  strip.show();
  uint32_t colorWhite = strip.Color(MAX, MAX, MAX);
  uint32_t colorYellow = strip.Color(MAX, MAX * 3 / 5, 0);
  uint32_t colorRed = strip.Color(MAX_CANDLE, 0, 0);
  uint32_t colorGreen = strip.Color(0, MAX_CANDLE, 0);
  for (int iter = 0; iter < 2000; iter++) {
    // base is white with yellow dots
    for (int i = 0; i < 10; i++) {
      strip.setPixelColor(9 - i, colorForIndexAndIter(i, iter));
      strip.setPixelColor(10 + i, colorForIndexAndIter(i, iter));
    }
    // stem is white with yellow dots
    for (int i = 0; i < LEN_STEM; i++) {
      strip.setPixelColor(BOTTOM_STEM + i * STEP_STEM, colorForIndexAndIter(i, iter));
    }
    for (int tmpDay = 1; tmpDay <= day; tmpDay++) {

    }
    for (int tmpDay = 1; tmpDay <= 8; tmpDay++) {
      if (tmpDay <= day) {
        for (int i = 0; i < lenForDay[tmpDay]; i++) {
          strip.setPixelColor(bottomForDay[tmpDay] + i * stepForDay[tmpDay], colorForIndexAndIter(i + baseAtStemForDay[tmpDay], iter));
        }
        strip.setPixelColor(ledForDay[tmpDay], colorGreen);
      } else {
        for (int i = 0; i < lenForDay[tmpDay]; i++) {
          strip.setPixelColor(bottomForDay[tmpDay] + i * stepForDay[tmpDay], WheelWithMax(i + baseAtStemForDay[tmpDay] + iter, MAX));
        }
        strip.setPixelColor(ledForDay[tmpDay], colorRed);
      }
    }
    strip.setPixelColor(41, colorGreen);
    strip.show();
    delay(10);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to MAX to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t WheelWithMax(byte WheelPos, byte Max) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color((255 - WheelPos * 3) * Max / 255, 0, WheelPos * 3 * Max / 255);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3 * Max / 255, (255 - WheelPos * 3) * Max / 255);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3 * Max / 255, (255 - WheelPos * 3) * Max / 255, 0);
}
uint32_t Wheel(byte WheelPos) {
  return WheelWithMax(WheelPos, MAX);
}