/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <string.h>
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

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

int colorState = 0;
int maxColorState = 3;
void OnSetColorState(boolean Up, boolean change = true)
{

  if(change)
  {
    if(Up)
      colorState++;
    else
      colorState--;
  }

  if(colorState > maxColorState)
    colorState = 0;
  else if(colorState < 0)
    colorState = maxColorState;

  if(colorState == 0)
  {
    r1 = 255;
    g1 = 0;
    b1 = 0;
    r2 = 0;
    g2 = 255;
    b2 = 0;
  }
  else if(colorState == 1)
  {
    r1 = 0;
    b1 = 255;
    g1 = 0;
    r2 = 255;
    g2 = 0;
    b2 = 255;
  }
  else if(colorState == 2)
  {
    r1 = 0;
    g1 = 255;
    b1 = 0;
    r2 = 0;
    g2 = 255;
    b2 = 255;
  }
  else if(colorState == 3)
  {
    r1 = 255;
    g1 = 255;
    b1 = 0;
    r2 = 255;
    b2 = 100;
    g2 = 30;
  }

  setColors();
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
  while(f < 100){
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
    mIndBrightness = 1.0;
    r1 = 0;
    g1 = 255;
    b1 = 0;
    patternByte = mPattern_to_patternByte(13);
    if (patternByte != NULL_PATTERN && patterns[patternByte] != NULL) {
      isOff = false;
      pattern = patterns[patternByte];
      pattern(-2, 0); // On select initialization
    }
    rate = 100;
    SetNewMapping(0);
    r2 = 255;
    g2 = 0;
    b2 = 0;
    setColors();
  }
  else if(state == 1)
  {
    mIndBrightness = 1.0;
    r1 = 255;
    g1 = 0;
    b1 = 255;
    patternByte = mPattern_to_patternByte(6);
    if (patternByte != NULL_PATTERN && patterns[patternByte] != NULL) {
      isOff = false;
      pattern = patterns[patternByte];
      pattern(-2, 0); // On select initialization
    }
    rate = 140;
    SetNewMapping(0);
    r2 = 0;
    g2 = 0;
    b2 = 255;
    setColors();
  }
  else if(state == 2)
  {
    mIndBrightness = 1.0;
    patternByte = mPattern_to_patternByte(2);
    if (patternByte != NULL_PATTERN && patterns[patternByte] != NULL) {
      isOff = false;
      pattern = patterns[patternByte];
      pattern(-2, 0); // On select initialization
    }
    rate = 100;
    SetNewMapping(0);
  }
  else if(state == 3)
  {
    mIndBrightness = 1.0;
    patternByte = mPattern_to_patternByte(1);
    if (patternByte != NULL_PATTERN && patterns[patternByte] != NULL) {
      isOff = false;
      pattern = patterns[patternByte];
      pattern(-2, 0); // On select initialization
    }
    rate = 100;
    SetNewMapping(0);
  }

  OnSetColorState(false, false);
}

int state = 0;
int maxState = 3;
void OnButtonPress()
{
  Serial.print("Cur State: ");
  Serial.println(state);
  SetUFOByState(state);
}



void SetInitialState()
{
  state = 0;
  SetUFOByState(state);
}

byte currentCommandBuf [READBUFFERSIZE];
#define THE_BUTTON 2

/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    MODE_LED_BEHAVIOUR        LED activity, valid options are
                              "DISABLE" or "MODE" or "BLEUART" or
                              "HWUART"  or "SPI"  or "MANUAL"
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         1
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
// Adafruit_BluefruitLE_UART ble(BLUEFRUIT_HWSERIAL_NAME, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];


/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  //while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit App Controller Example"));
  Serial.println(F("-----------------------------------------"));




  pinMode(THE_BUTTON, INPUT);
  digitalWrite(THE_BUTTON, HIGH);
  pinMode(13, OUTPUT);
  Serial.begin(115200);

  strip.setBrightness(255);
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



  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }


  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set Bluefruit to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));

}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
  /* Wait for new data to arrive */
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len > 0)
  {

    // /* Got a packet! */
    // printHex(packetbuffer, len);

    // Color
    if (packetbuffer[1] == 'C') {
      uint8_t red = packetbuffer[2];
      uint8_t green = packetbuffer[3];
      uint8_t blue = packetbuffer[4];
      Serial.print ("RGB #");
      if (red < 0x10) Serial.print("0");
      Serial.print(red, HEX);
      if (green < 0x10) Serial.print("0");
      Serial.print(green, HEX);
      if (blue < 0x10) Serial.print("0");
      Serial.println(blue, HEX);
    }

    // Buttons
    if (packetbuffer[1] == 'B') {
      uint8_t buttnum = packetbuffer[2] - '0';
      boolean pressed = packetbuffer[3] - '0';
      Serial.print ("Button "); Serial.print(buttnum);
      if (pressed) {
        Serial.println(" pressed");
      } else {
        Serial.println(" released");
        
        if(buttnum == 1){
          state = 0;
          OnButtonPress();
        }
        else if(buttnum == 2)
        {
          state = 1;
          OnButtonPress();
        }
        else if(buttnum == 3)
        {
          state = 2;
          OnButtonPress();
        }
        else if(buttnum == 4)
        {
          state = 3;
          OnButtonPress();
        }
        else if(buttnum == 8){
          rate += 10;
          if(rate > 255)
            rate = 255;
        }
        else if(buttnum == 7){
          rate -= 10;
          if(rate < 0)
            rate = 0;
        }
        else if(buttnum == 5)
        {
          OnSetColorState(true);
        }
        else if(buttnum == 6)
        {
          OnSetColorState(false);
        }
      }
    }

    // GPS Location
    if (packetbuffer[1] == 'L') {
      float lat, lon, alt;
      lat = parsefloat(packetbuffer+2);
      lon = parsefloat(packetbuffer+6);
      alt = parsefloat(packetbuffer+10);
      Serial.print("GPS Location\t");
      Serial.print("Lat: "); Serial.print(lat, 4); // 4 digits of precision!
      Serial.print('\t');
      Serial.print("Lon: "); Serial.print(lon, 4); // 4 digits of precision!
      Serial.print('\t');
      Serial.print(alt, 4); Serial.println(" meters");
    }

    // Accelerometer
    if (packetbuffer[1] == 'A') {
      float x, y, z;
      x = parsefloat(packetbuffer+2);
      y = parsefloat(packetbuffer+6);
      z = parsefloat(packetbuffer+10);
      Serial.print("Accel\t");
      Serial.print(x); Serial.print('\t');
      Serial.print(y); Serial.print('\t');
      Serial.print(z); Serial.println();
    }

    // Magnetometer
    if (packetbuffer[1] == 'M') {
      float x, y, z;
      x = parsefloat(packetbuffer+2);
      y = parsefloat(packetbuffer+6);
      z = parsefloat(packetbuffer+10);
      Serial.print("Mag\t");
      Serial.print(x); Serial.print('\t');
      Serial.print(y); Serial.print('\t');
      Serial.print(z); Serial.println();
    }

    // Gyroscope
    if (packetbuffer[1] == 'G') {
      float x, y, z;
      x = parsefloat(packetbuffer+2);
      y = parsefloat(packetbuffer+6);
      z = parsefloat(packetbuffer+10);
      Serial.print("Gyro\t");
      Serial.print(x); Serial.print('\t');
      Serial.print(y); Serial.print('\t');
      Serial.print(z); Serial.println();
    }

    // Quaternions
    if (packetbuffer[1] == 'Q') {
      float x, y, z, w;
      x = parsefloat(packetbuffer+2);
      y = parsefloat(packetbuffer+6);
      z = parsefloat(packetbuffer+10);
      w = parsefloat(packetbuffer+14);
      Serial.print("Quat\t");
      Serial.print(x); Serial.print('\t');
      Serial.print(y); Serial.print('\t');
      Serial.print(z); Serial.print('\t');
      Serial.print(w); Serial.println();
    }

  }


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

    if(state == 3 && colorState == 3)
      strip.setPixelColor(i, strip.Color(0,0,0,255));
    else
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
