#include <Bela.h>
#include <libraries/BelaArduino/Arduino.h>
#include <libraries/BelaArduino/Wire.h>
#include "vl53l5cx_arduino.h"
#include "debugger.hpp"
#include <libraries/Adafruit_NeoPixel/Adafruit_NeoPixel.h>
#include <libraries/Adafruit_NeoPixel/matrix.hpp>
#include <iostream>
#include <cmath>

#define FALLING 2
#define LOW     0
#define HIGH    1
#define INPUT   0
#define OUTPUT  1
#define NUMPIXELS      64
static const uint8_t INT_PIN = 0;
static const uint8_t LPN_PIN = 0;
static const uint8_t INTEGRAL_TIME_MS = 10;

// Servo pins
#define TILT_PIN 0 // D0 for tilt servo
#define PAN_PIN 1  // D1 for pan servo

// Servo angle limits
float panAngle = 90.0;  // Initial pan position
float tiltAngle = 90.0; // Initial tilt position
const float MIN_ANGLE = 0.0;
const float MAX_ANGLE = 180.0;

// Helper function: Set servo PWM
void setServoAngle(int pin, float angle) {
    float dutyCycle = map(angle, MIN_ANGLE, MAX_ANGLE, 5, 10); // Map angle to 5-10% duty cycle
    analogWrite(pin, dutyCycle * 0.01, 50); // 50 Hz PWM
}



//vlc53l5cx
static VL53L5CX_Arduino _sensor(LPN_PIN, INTEGRAL_TIME_MS, VL53L5CX::RES_8X8_HZ_1, &Wire, 0x29);
static volatile bool _gotInterrupt = false;

//neopixel setup
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEO_GRB + NEO_KHZ800);
// Interrupt handler
/*static void interruptHandler() {
    _gotInterrupt = true;
}*/
//Matrix:)
int distanceMatrix[8][8];

// Previous state for edge detection
//int previousState = HIGH;

// Edge detection
/*void checkInterrupt(BelaContext *context, int pin, void (*handler)()) {
    int currentState = digitalRead(context, 0, pin);
    if (previousState == HIGH && currentState == LOW) {
        handler();
    }
    previousState = currentState;
}*/

// HSBtoRGB
void HSBtoRGB(float hue, float saturation, float brightness, int &r, int &g, int &b) {
    float chroma = brightness * saturation;
    float h_prime = hue / 60.0;
    float x = chroma * (1 - fabs(fmod(h_prime, 2) - 1));
    float m = brightness - chroma;

    float r1, g1, b1;

    if (0 <= h_prime && h_prime < 1) {
        r1 = chroma;
        g1 = x;
        b1 = 0;
    } else if (1 <= h_prime && h_prime < 2) {
        r1 = x;
        g1 = chroma;
        b1 = 0;
    } else if (2 <= h_prime && h_prime < 3) {
        r1 = 0;
        g1 = chroma;
        b1 = x;
    } else if (3 <= h_prime && h_prime < 4) {
        r1 = 0;
        g1 = x;
        b1 = chroma;
    } else if (4 <= h_prime && h_prime < 5) {
        r1 = x;
        g1 = 0;
        b1 = chroma;
    } else if (5 <= h_prime && h_prime < 6) {
        r1 = chroma;
        g1 = 0;
        b1 = x;
    } else {
        r1 = 0;
        g1 = 0;
        b1 = 0;
    }

    r = (r1 + m) * 255;
    g = (g1 + m) * 255;
    b = (b1 + m) * 255;
}


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
// Initialize servos
pinMode(TILT_PIN, OUTPUT);
pinMode(PAN_PIN, OUTPUT);

// Set initial servo positions
setServoAngle(TILT_PIN, tiltAngle);
setServoAngle(PAN_PIN, panAngle);


// Loop
void loop() {
    
    /*if (INT_PIN == 0 || _gotInterrupt) {
        _gotInterrupt = false;
*/
        while (!_sensor.dataIsReady()) {
            delay(10);
        }

        _sensor.readData();

        for (auto i = 0; i < _sensor.getPixelCount(); i++) {
            int row = int(i/8);
            int column = i%8;
            //Debugger::printf("Zone: %2d %2d %2d , ",i,row,column);

            
            Debugger::printf("Zone: %2d, Nb targets: %2u, Ambient: %4lu Kcps/spads, ",
                             i, _sensor.getTargetDetectedCount(i), _sensor.getAmbientPerSpad(i));

            if (_sensor.getTargetDetectedCount(i) > 0) {
                Debugger::printf("Target status: %3u, Distance: %4d mm\n",
                                 _sensor.getTargetStatus(i), _sensor.getDistanceMm(i));
                                 
                                 
	                
	            if (_sensor.getDistanceMm(i)>2000){
	                distanceMatrix[row][column]=2000;
	
	            }else{
	                distanceMatrix[row][column] = _sensor.getDistanceMm(i);
	            }
            
            } else {
                Debugger::printf("Target status: 255, Distance: No target\n");
            }
        
        Debugger::printf("\n");

        }//}

    
    // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  
    for(int i=0;i<NUMPIXELS;i++)
    {
        int row = int(i/8);
        int column = i%8;
      

        float hue = map(distanceMatrix[7-row][column],0,2000,359,0); // Example hue value

        float saturation = 1; // Example saturation value
        float brightness = 0.05; // Example brightness value
        int r, g, b;
        
        HSBtoRGB(hue, saturation, brightness, r, g, b);

       // std::cout << "RGB: (" << r << ", " << g << ", " << b << ") H:  " << hue << std::endl;
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(r,g,b)); // Moderately bright green color.
        
        pixels.show(); // This sends the updated pixel color to the hardware.
        //delay(100);
  }
  // Calculate row and column sums
int rowSums[8] = {0};
int colSums[8] = {0};
for (int r = 0; r < 8; r++) {
    for (int c = 0; c < 8; c++) {
        rowSums[r] += distanceMatrix[r][c];
        colSums[c] += distanceMatrix[r][c];
    }
}

// Find row and column with the smallest sum (closest object)
int minRow = 0;
int minCol = 0;
for (int i = 1; i < 8; i++) {
    if (rowSums[i] < rowSums[minRow]) {
        minRow = i;
    }
    if (colSums[i] < colSums[minCol]) {
        minCol = i;
    }
}

// Update servo angles
tiltAngle = map(minRow, 0, 7, MIN_ANGLE, MAX_ANGLE);
panAngle = map(minCol, 0, 7, MIN_ANGLE, MAX_ANGLE);

setServoAngle(TILT_PIN, tiltAngle);
setServoAngle(PAN_PIN, panAngle);

// delay(1000);
}