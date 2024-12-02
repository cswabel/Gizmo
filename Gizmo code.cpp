#include <Bela.h>
#include <libraries/BelaArduino/Arduino.h>
#include <libraries/BelaArduino/Wire.h>
#include "vl53l5cx_arduino.h"
#include "debugger.hpp"
#include <libraries/Adafruit_NeoPixel/Adafruit_NeoPixel.h>
#include <libraries/Adafruit_NeoPixel/matrix.hpp>

#define FALLING 2
#define LOW     0
#define HIGH    1
#define INPUT   0
#define OUTPUT  1
#define NUMPIXELS      64
static const uint8_t INT_PIN = 0;
static const uint8_t LPN_PIN = 0;
static const uint8_t INTEGRAL_TIME_MS = 10;

//vlc53l5cx
static VL53L5CX_Arduino _sensor(LPN_PIN, INTEGRAL_TIME_MS, VL53L5CX::RES_8X8_HZ_1, &Wire, 0x29);
static volatile bool _gotInterrupt = false;

//neopixel setup
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEO_GRB + NEO_KHZ800);
// Interrupt handler
//static void interruptHandler() {
 //   _gotInterrupt = true;
//}
//Matrix:)
int distanceMatrix[8][8];

// Previous state for edge detection
int previousState = HIGH;

// Edge detection
void checkInterrupt(BelaContext *context, int pin, void (*handler)()) {
    int currentState = digitalRead(context, 0, pin);
    if (previousState == HIGH && currentState == LOW) {
        handler();
    }
    previousState = currentState;
}

// Constants


// Setup
void setup() {
    Serial.begin(115200);
    delay(4000);
    Debugger::printf("Serial begun!\n");

    pinMode(INT_PIN, INPUT);
    Wire.begin();
    //Wire.setClock(400000);
    delay(1000);

    Debugger::printf("Starting...\n\n");
    delay(1000);

    _sensor.begin();
     pixels.begin(); // This initializes the NeoPixel library.
     randomSeed(analogRead(0));
}

// Loop
void loop() {
    if (INT_PIN == 0 || _gotInterrupt) {
        _gotInterrupt = false;

        while (!_sensor.dataIsReady()) {
            delay(10);
        }

        _sensor.readData();

        for (auto i = 0; i < _sensor.getPixelCount(); i++) {
            int row = int(i/_sensor.getPixelCount());
            int column = i%8;
            distanceMatrix[row][column] = _sensor.getDistanceMm(i);
            Debugger::printf("Zone: %2d, Nb targets: %2u, Ambient: %4lu Kcps/spads, ",
                             i, _sensor.getTargetDetectedCount(i), _sensor.getAmbientPerSpad(i));

            if (_sensor.getTargetDetectedCount(i) > 0) {
                Debugger::printf("Target status: %3u, Distance: %4d mm\n",
                                 _sensor.getTargetStatus(i), _sensor.getDistanceMm(i));
            } else {
                Debugger::printf("Target status: 255, Distance: No target\n");
            }
        }
        Debugger::printf("\n");


    }
    // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
    int rndRedValue = random(0,5);
    int rndGreenValue = random(0,5);
    int rndBlueValue = random(0,5);
    for(int i=0;i<NUMPIXELS;i++)
    {
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(rndRedValue,rndGreenValue,rndBlueValue)); // Moderately bright green color.
        pixels.show(); // This sends the updated pixel color to the hardware.
        //delay(delayval);
  }
    delay(1000);
}