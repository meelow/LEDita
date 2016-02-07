#include "FastLED.h"
#include <Bridge.h>
#include <Console.h>

FASTLED_USING_NAMESPACE

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    6
//#define CLK_PIN   4
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    120
CRGB leds[NUM_LEDS];

#define DEFAULT_FRAMES_PER_SECOND  60

CRGBPalette16 currentPalette;

void setup() {
  delay(3000); // 3 second delay for recovery
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(128);
  
  for(uint8_t i=0; i<255; i++)
  {
   fill_rainbow( leds, NUM_LEDS, i, 1);
   FastLED.show();
  }
  
  SetupChristmasPalette();
  
  Bridge.begin();
  Console.begin();
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };
//SimplePatternList gPatterns = { sinelon, confetti, barControl };
SimplePatternList gModes = { confetti, rainbow, sinelon, juggle, bpm };
//SimplePatternList gPatterns = { barControl, rainbow };

uint8_t gCurrentModeNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t gBrightness=128;
uint8_t gRotary1=60;
uint8_t gXYpad1=0;
uint8_t gXYpad2=0;
uint8_t gPush1=0;
uint8_t gPush2=0;
uint8_t gPush3=0;


unsigned int gShowEveryNMillis = 1000/DEFAULT_FRAMES_PER_SECOND;
  
void loop()
{
  unsigned long currentMillis = millis();
  static unsigned long timeOfLastProcessing=0;
  
  // Input
  //  read OSC data from bridge and adapt these input values
  EVERY_N_MILLISECONDS(100) { updateFromBridge(); }
  
  // Processing
  //  do computation on the global variables and input values then paint into the buffer 'leds'
  if( (currentMillis-timeOfLastProcessing) > gShowEveryNMillis )  
  { 
    timeOfLastProcessing = currentMillis;  // remember this processing
  	gHue+=1;
	
	// boost speed a little so that rainbow looks faster:
	if( gShowEveryNMillis<10) gHue+=2;
	
	// fill the 'led' buffer:
    gModes[gCurrentModeNumber]();
  }
  
  // Output
  //  adapt current brightness and paint the buffer 'leds' on the string
  EVERY_N_MILLISECONDS(10) 
  { 
    FastLED.setBrightness(gBrightness);
    FastLED.show();  
  }
 }

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void setMode(uint8_t number)
{
  gCurrentModeNumber = (number) % ARRAY_SIZE( gModes);
  
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 1);
  // static uint8_t lHue=0;
  // lHue+=1;
  // fill_rainbow( leds, NUM_LEDS, lHue, 1);
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
    uint8_t led = random16(NUM_LEDS);
    leds[led-2] += CRGB::White;
    leds[led-1] += CRGB::White;
    leds[led] += CRGB::White;
    leds[led+1] += CRGB::White;
    leds[led+2] += CRGB::White;
  }
}

void confetti() 
{
  const uint8_t cCompartments=20;
  const uint8_t cCompartmentLength=NUM_LEDS/cCompartments;
  static uint8_t lFadeInCompartments[cCompartments];
 
  // fade all existing compartments
  fadeToBlackBy( leds, NUM_LEDS, 20);
 
  // increase light in each compartmet until max, then switch off
  for( uint8_t currentCompartment=0; currentCompartment<cCompartments; currentCompartment++ )
  {
    if( lFadeInCompartments[currentCompartment] != 0 )
	  lFadeInCompartments[currentCompartment]+=20;
    if( lFadeInCompartments[currentCompartment] > 200 )
	  lFadeInCompartments[currentCompartment] = 0;	  
  }
  
  // paint the compartments
  for( uint8_t currentCompartment=0; currentCompartment<cCompartments; currentCompartment++ )
  {
    if( lFadeInCompartments[currentCompartment] != 0 )
	{
	  for( uint8_t currentPixel=0; currentPixel<cCompartmentLength; currentPixel++)
	  {
	    uint8_t compartmentStartPixel = currentCompartment * cCompartmentLength;
	    leds[compartmentStartPixel+currentPixel] = CHSV( gHue, 255, lFadeInCompartments[currentCompartment]);
	  }
	}
  }

  // randomly select new compartments to 'grow' in light  
  if( random8(120) > gRotary1 )
  {
	  uint8_t pos = random8(cCompartments);
	  if( lFadeInCompartments[pos] == 0 )
	    lFadeInCompartments[pos] = 1;
  }
 
   
   
  // random colored speckles that blink in and fade smoothly  
  // if( random8(100) > 25 )
  // {
	  // uint16_t pos = random16(NUM_LEDS);
	  // uint8_t col = random16();
	  // leds[(pos-1)%NUM_LEDS] += ColorFromPalette( currentPalette, col-5, 150, LINEARBLEND);
	  // leds[pos]              += ColorFromPalette( currentPalette, col, 200, LINEARBLEND);
	  // leds[(pos+1)%NUM_LEDS] += ColorFromPalette( currentPalette, col+5, 150, LINEARBLEND);
  // }
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
  //leds[pos] += ColorFromPalette(currentPalette, gHue, 255);
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
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

// This function sets up a palette of purple and green stripes.
void SetupChristmasPalette()
{
    CRGB white = CRGB::White;
    CRGB red  = CHSV( HUE_RED, 255, 255);
	CRGB green = CRGB::Green;
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   white,  white,  black,  black,
                                   red, red, black,  black,
                                   green,  green,  black,  black,
                                   red, red, black,  black );
}

void updateFromBridge()
{
	const uint8_t stringSize=3;
	char bridgeValueStr[stringSize];
	
	// read brightness:
	Bridge.get("brightness",bridgeValueStr, stringSize);
	int brightness = atoi(bridgeValueStr);
	if( brightness>0 && brightness<255 )
	{
	  gBrightness = brightness;
	  Bridge.put("brightness","256");
	} 

	// read speed:
	Bridge.get("fps", bridgeValueStr, stringSize);
	int fps = atoi(bridgeValueStr);
	if( fps==0 || fps<0 ) 
	  fps=DEFAULT_FRAMES_PER_SECOND;
    gShowEveryNMillis = 1000/fps;

	// read mode:
	Bridge.get("state",bridgeValueStr, stringSize);
	int mode = atoi(bridgeValueStr);
	if( mode>=0 && mode<255 )
	{
	  setMode(mode);
	} 	
	
	// read rotary1:
	Bridge.get("rotary1",bridgeValueStr, stringSize);
	int rotary1 = atoi(bridgeValueStr);
	if( rotary1>=0 && rotary1<255 )
	{
	  gRotary1=rotary1;
	} 	

	// read xypad:
	Bridge.get("xypad1",bridgeValueStr, stringSize);
	int xypad1 = atoi(bridgeValueStr);
	if( xypad1>0 && xypad1<255 )
	{
	  gXYpad1 = xypad1;
	} 
	Bridge.get("xypad2",bridgeValueStr, stringSize);
	int xypad2 = atoi(bridgeValueStr);
	if( xypad2>0 && xypad2<255 )
	{
	  gXYpad2 = xypad2;
	} 
	
	// read push buttons:
	Bridge.get("push1",bridgeValueStr, stringSize);
	int push1 = atoi(bridgeValueStr);
	if( push1>0 && push1<256 )
	{
	  gPush1 = push1;
	  Bridge.put("push1","0");
	} 
	Bridge.get("push2",bridgeValueStr, stringSize);
	int push2 = atoi(bridgeValueStr);
	if( push2>0 && push2<256 )
	{
	  gPush2 = push2;
	  Bridge.put("push2","0");
	} 
	Bridge.get("push3",bridgeValueStr, stringSize);
	int push3 = atoi(bridgeValueStr);
	if( push3>0 && push3<=256 )
	{
	  gPush3 = push3;
	  Bridge.put("push3","0");
	} 
}