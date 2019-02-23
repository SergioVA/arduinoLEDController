#if !defined(common_h)
#define common_h

#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define DATA_PIN    5
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    240
#define NUM_BUTTONS 9
#define POWER_BUTTON_PIN A2
#define POWER_LED_PIN 10
#define SCENE_BUTTON_LED_PIN 9

CRGB leds[NUM_LEDS];
int frameNr;


int buttonPins[NUM_BUTTONS] = {2, 3, 4, 6, 7, 8, 11, 12, 13};
bool buttons[NUM_BUTTONS];

#define STROBE_SPEED 4

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120


uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


#endif // common_h
