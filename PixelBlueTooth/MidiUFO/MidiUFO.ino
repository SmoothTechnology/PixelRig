#include "CommonVariables.h"
#include "mappings.h"
#include "utils.h"
#include "patterns.h"
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

/* Helper functions */

//Input a rateue 0 to 384 to get a color rateue.
//The colours are a transition r - g -b - back to r

int numPatterns = 17;
unsigned int mPattern_to_patternByte(byte incomingByte)
{
  switch (incomingByte) {
    case 0:
      return 68;
      break;
    case 1:
      return 69;
      break;
    case 2:
      return 70;
      break;
    case 3:
      return 62;
      break;
    case 4:
      return 63;
      break;
    case 5:
      return 64;
      break;
    case 6:
      return 65;
      break;
    case 7:
      return 66;
      break;
    case 8:
      return 71;
      break;
    case 9:
      return 72;
      break;
    case 10:
      return 73;
      break;
    case 11:
      return 74;
      break;
    case 12:
      return 75;
      break;
    case 13:
      return 76;
      break;
    case 14:
      return 77;
      break;
    case 15:
      return 78;
      break;
    case 16:
      return 79;
      break;
    case 17:
      return 80;
      break;
    default:
      return 0;
      break;
  }


}

void setColors()
{
  color1 = myColor(r1, g1, b1);
  color2 = myColor(r2, g2, b2);
}

void hideAll() {
  for (int i = 0; i < totalLEDs; i++) {
    leds[i] = 0x00000000;
  }
}

void showAll() {
  strip.show();
}


void ledCheck(){
  int f = 0;
  int s = 6;
  while(f < 300){
      for(int i = 0; i < totalLEDs; i++){
        if ((i + f)/s % s < s/2)
        {
          strip.setPixelColor(i, strip.Color(255, 255, 0));
        }
        else
        {
          strip.setPixelColor(i, strip.Color(0, 0, 255));
        }
      }

      strip.show();
      f++;
      Serial.println(f);
      delay(20);
  }

}

byte DecodeVDMXPattern(int value)
{
  int numIncrement = 127/numPatterns;

  for(int i = 0; i < numPatterns; i++)
  {
    if(i*numIncrement <= value && (i+1)*numIncrement > value)
    {
      return i;
    }
  }
  return 0;
}

void SetNewMapping(int value)
{
  if(value == 0)
  {
    mapping = &forward;
  }
  else if(value == 1)
  {
    mapping = &backward;
  }
  else if(value == 2)
  {
    mapping = &snake;
  }
  else if(value == 3)
  {
    mapping = &intoSpace;
  }
  else if(value == 4)
  {
    mapping = &shortSnake;
  }
  else if(value == 5)
  {
    mapping = &longSnake;
  }
  else if(value == 6)
  {
    mapping = &dekonstruktor;
  }
  else if(value > 7)
  {
    mapping = &deskonstruktorIntoSpace;
  }
  //else if(value == 7)
  //{
  //  mapping = &dekonstruktorRando;
  //}
}

void SetUFOByState(int state)
{
  if(state == 0)
  {
    mIndBrightness = 0.8;
    r1 = 0;
    g1 = 255;
    b1 = 0;
    patternByte = mPattern_to_patternByte(13);
    if (patternByte != NULL_PATTERN && patterns[patternByte] != NULL) {
      isOff = false;
      pattern = patterns[patternByte];
      pattern(-2, 0); // On select initialization
    }
    rate = 50;
    SetNewMapping(0);
    r2 = 255;
    g2 = 0;
    b2 = 0;
    setColors();
  }
}


int state = 0;
int maxState = 5;
void OnButtonPress()
{
  state ++;
  if(state>maxState)
    state = 0;

  SetUFOByState(state);
}

void SetInitialState()
{
  state = 0;
  SetUFOByState(state);
}

byte currentCommandBuf [READBUFFERSIZE];
#define THE_BUTTON 2

void setup() {

  pinMode(THE_BUTTON, INPUT);
  digitalWrite(THE_BUTTON, HIGH);
  pinMode(13, OUTPUT);
  Serial.begin(115200);

  strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.println("Begin Check");
  ledCheck();
  Serial.println("Check complete");

  setColors();

  hideAll();
  showAll();

  patterns[62] = &flickerStrobeTwo;
  patterns[63] = &flickerStrobeFour;
  patterns[64] = &totesRandom;
  patterns[65] = &rainbowCycle;
  patterns[66] = &rainbow;
  // 67 = pause
  // 68 = off
  patterns[69] = &solidColor;
  patterns[70] = &gradient;
  patterns[71] = &pulseSine;
  patterns[72] = &pulseSaw;
  patterns[73] = &bounce;
  patterns[74] = &colorWipe;
  patterns[75] = &colorAlternator;
  patterns[76] = &stripe;
  patterns[77] = &colorChase;
  patterns[78] = &colorWipeMeter;
  patterns[79] = &colorWipeMeterGradient;
  patterns[80] = &pulseOnce;

  rate = 122;
  // pattern = &pulseOnce;
  pattern = &gradient;
  mapping = &intoSpace;

  mIndBrightness = 255;
  
  rate = mRate + 1;

  SetInitialState();
}

void loop() {

  if(digitalRead(THE_BUTTON) == 0)
    OnButtonPress();

  int usedRate = 128-rate;
  mCurrentFrameCount += abs(usedRate);
  
  if(mCurrentFrameCount >= NUM_STEPS_PER_FRAME)
  {
    int framesToMove = mCurrentFrameCount/NUM_STEPS_PER_FRAME;
    mCurrentFrameCount = mCurrentFrameCount - framesToMove*NUM_STEPS_PER_FRAME;
    
    frame += usedRate < 0 ? -1*framesToMove : framesToMove;  
  }

  if (frame != lastFrame)
    pattern(-1, 0); // Per frame initialization

  lastFrame = frame;

  for (int i = 0; i < totalLEDs; i++) {

    int j = mapping(frame, i);
    uint32_t color = pattern(frame, j);

    uint8_t r = ((color & 0xFF0000) >> 16);
    uint8_t g = ((color & 0x00FF00) >> 8);
    uint8_t b = ((color & 0x0000FF));

    //TODO change setbrightness only on change
    if (mIndBrightness < 1.0) {
      r *= mIndBrightness;
      g *= mIndBrightness;
      b *= mIndBrightness;
    }

    float whiteDimmer = 0.7;

    if (r == g && g == b) {
      r *= whiteDimmer;
      g *= whiteDimmer;
      b *= whiteDimmer;
    }
    leds[i] = color; 
    strip.setPixelColor(i, strip.Color(r, g, b));
  }

  if (isOff) {
    hideAll();
  }

  showAll();

  if (frame >= MAX_FRAME) {
    frame = 0;
  }

  if (light)
    digitalWrite(13, HIGH);
  else
    digitalWrite(13, LOW);

  light = !light;

}

