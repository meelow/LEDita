#include "FastLED.h"
#include <Bridge.h>
#include <Console.h>

FASTLED_USING_NAMESPACE

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    6
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    120

// helper macro:
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// global array of LED pixels for FastLED library:
CRGB leds[NUM_LEDS];				

// settings for default and current framerate: 
const int DEFAULT_FRAMES_PER_SECOND=60;
unsigned int gShowEveryNMillis = 1000/DEFAULT_FRAMES_PER_SECOND;
  
// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gModes = { confetti, palette, bpm, tuneup, sinelon, juggle };


// global variables representing the input (get filled in updateFromBridge()):
uint8_t gCurrentModeNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; 				// rotating "base color" used by many of the patterns
uint8_t gBrightness=128;
uint8_t gRotary1=60;
uint8_t gXYpad1=0;
uint8_t gXYpad2=0;
uint8_t gPush1=0;
uint8_t gPush2=0;
uint8_t gPush3=0;

// predefined color schemes used in some animations (e.g. palette() cycles through the color scheme)
CRGBPalette16 currentPalette;     // changed by setCurrentPalette() in updateFromBridg()
TBlendType    currentBlending;    // currently always LINEARBLEND

void setup() {
  delay(3000); // 3 second delay for recovery
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // Show boot-up animation:
  FastLED.setBrightness(128);
  for(uint8_t i=0; i<255; i++)
  {
   fill_rainbow( leds, NUM_LEDS, i, 1);
   FastLED.show();
  }
  
  Bridge.begin();
  Console.begin();
  
  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}

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

void setMode(uint8_t number)
{
  gCurrentModeNumber = (number) % ARRAY_SIZE( gModes);
  
}

void setCurrentPalette(uint8_t index)
{
  // changing the current palette
  switch(index)
  {
    case 0:
      currentPalette = RainbowColors_p;
      break;
    case 1:
      currentPalette = RainbowStripeColors_p;
      break;
    case 2:
      currentPalette = CloudColors_p;
      break;
    case 3:
      currentPalette = PartyColors_p;
      break;
    case 4:
      currentPalette = HeatColors_p ;
      break;
    case 5:
      currentPalette = ForestColors_p ;
      break;
    case 6:
      currentPalette = OceanColors_p ;
      break;
    case 7:
      currentPalette = LavaColors_p ;
      break;
    case 8:
      currentPalette = HeatColors_p  ;
      break;
    case 9:
      currentPalette = Rainbow_gp;
    default:
      currentPalette = RainbowColors_p;
      break;
  }
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
  
  // read palette index:
  Bridge.get("palette", bridgeValueStr, stringSize);
	int palette = atoi(bridgeValueStr);
	if( palette>=0 && palette<255 )
	{
	  setCurrentPalette(palette);
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


void palette()
{
  //setCurrentPalette(map( gRotary1, 0, 120, 0, 9));
	for( int i = 0; i < NUM_LEDS; i++) {
		leds[i] = ColorFromPalette( currentPalette, gHue+i, 255, currentBlending);
	}
}


void confetti() 
{
  // The led strip is divided into a fixed number of compartments containing some LEDs. 
  // Randomly a compartment starts fading in and out again. Randomness controllable by gRotary1
  // Color of compartment can be choosen with xyPad1 and xyPad2
  const uint8_t cCompartments=NUM_LEDS;
  const uint8_t cCompartmentLength=NUM_LEDS/cCompartments;
  const uint8_t cFadeAmmount=10;
  static uint8_t lValueOfCompartment[cCompartments];
 
  // fade all existing compartments
  fadeToBlackBy( leds, NUM_LEDS, cFadeAmmount);
 
  // increase light in each compartmet until max, then switch off
  for( uint8_t currentCompartment=0; currentCompartment<cCompartments; currentCompartment++ )
  {
    if( lValueOfCompartment[currentCompartment] != 0 )
	  lValueOfCompartment[currentCompartment]+=cFadeAmmount;
    if( lValueOfCompartment[currentCompartment] > 200 )
	  lValueOfCompartment[currentCompartment] = 0;	  
  }
  
  // paint the compartments
  for( uint8_t currentCompartment=0; currentCompartment<cCompartments; currentCompartment++ )
  {
    if( lValueOfCompartment[currentCompartment] != 0 )
	{
	  for( uint8_t currentPixel=0; currentPixel<cCompartmentLength; currentPixel++)
	  {
	    uint8_t compartmentStartPixel = currentCompartment * cCompartmentLength;
	    leds[compartmentStartPixel+currentPixel] = CHSV( gXYpad1, gXYpad2, lValueOfCompartment[currentCompartment]);
	  }
	}
  }

  // randomly select new compartments to 'grow' in light, but make sure at least 1 compartment is lit
  bool atLeastOneLit=false;
  for( uint8_t currentCompartment=0; currentCompartment<cCompartments; currentCompartment++ )
  {
    uint8_t compartmentMiddlePixel = (currentCompartment * cCompartmentLength) + cCompartmentLength/2;
    if( leds[compartmentMiddlePixel] )
	  atLeastOneLit=true;
  }	  
  if( (atLeastOneLit==false) || (random8(120) < gRotary1) ) // shall we light one compartment?
  {
	  uint8_t currentCompartment = random8(cCompartments);
	  uint8_t middlePixelOfCompartment = (currentCompartment*cCompartmentLength)+cCompartmentLength/2;
	  if( ! leds[middlePixelOfCompartment] ) // make sure the compartment is not lit before
	    lValueOfCompartment[currentCompartment] = 1;
  }
}

void tuneup()
{
  for(uint8_t i=0; i<NUM_LEDS; i++)
  {
    leds[i] = CRGB(0,0,0);
  }
  
  // light up some pixels always:
  uint8_t pixelsToLightUp=(NUM_LEDS/120)*gRotary1;
  
  // the speed of the sine wave:
  const uint8_t bpm=map(gXYpad1,0,255,30,120);
  
  // the legth of the led stripe that will react to the beet:
  const uint8_t numberOfPixelsToBeat = map(gXYpad2,0,255,NUM_LEDS/2,0);
  pixelsToLightUp += beatsin8(bpm,0,numberOfPixelsToBeat);
  
  // light up all pixels that should be light
   const uint8_t cFadeAmmount=50;
   fadeToBlackBy( leds, NUM_LEDS, cFadeAmmount);
  
  // display some pixels permanently, controlled by gRotary1=[0..120]
  uint8_t pixelsToLightUp=(NUM_LEDS/120)*gRotary1;
  if( pixelsToLightUp>NUM_LEDS ) 
    pixelsToLightUp=NUM_LEDS;
  for( uint8_t i=0; i<pixelsToLightUp; i++ )
  {
	// spread out the whole palette of the light up pixels:
    uint8_t paletteIndex = map( i, 0, pixelsToLightUp, 0, 256);
	leds[i] = ColorFromPalette( currentPalette, paletteIndex, gBrightness );
  }
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
    leds[i] = ColorFromPalette(currentPalette, gHue+(i*2), beat-gHue+(i*10));
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

