#include <FastLED.h>
#include <Arduino.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

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

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  //Configure scene button pins
  pinMode(SCENE_BUTTON_LED_PIN, OUTPUT);
  for(size_t i = 0; i < NUM_BUTTONS; i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  //Configure power button pins
  pinMode(POWER_BUTTON_PIN, INPUT_PULLUP);
  pinMode(POWER_LED_PIN, OUTPUT);

  frameNr = 0;
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, white, magenta, blue, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically

  frameNr = (frameNr + 1) % FRAMES_PER_SECOND;
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void strobe()
{
  CRGB color = ((frameNr % STROBE_SPEED) < STROBE_SPEED / 2) ? CRGB(255,255,255) : CRGB(0,0,0);
  fill_solid(leds, NUM_LEDS, color);
}

void magenta()
{
  fill_solid(leds, NUM_LEDS, CRGB(211, 0, 234));
}

void blue()
{
  fill_solid(leds, NUM_LEDS, CRGB(0, 211, 234));
}

void strobeRainbow()
{
  if((frameNr % STROBE_SPEED) < STROBE_SPEED / 2)
  {
    fill_rainbow(leds, NUM_LEDS, gHue, 7);
  }
  else
  {
    fill_solid(leds, NUM_LEDS, CRGB(0,0,0));
  }
}

void white()
{
  fill_solid(leds, NUM_LEDS, CRGB(255, 255, 255));
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

bool isButtonPressed(int index) {
  return buttons[index];
}

void scanButtons() {
  for(size_t i = 0; i < NUM_BUTTONS; i++) {
    //Inverted logic since we are using normally open contacts and pullup registers, buttons short to ground
    buttons[i] = (digitalRead(buttonPins[i]) == LOW);
  }
}
