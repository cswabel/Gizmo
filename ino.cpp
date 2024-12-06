#include <Bela.h>
#include <libraries/BelaArduino/Arduino.h>
#include <libraries/BelaArduino/Wire.h>
#include "vl53l5cx_arduino.h"
#include "debugger.hpp"
#include <libraries/Adafruit_NeoPixel/Adafruit_NeoPixel.h>
#include <libraries/Adafruit_NeoPixel/matrix.hpp>
#include <iostream>
#include <cmath>

// --- Constants and Macros ---
#define FALLING 2
#define LOW     0
#define HIGH    1
#define INPUT   0
#define OUTPUT  1
#define NUMPIXELS      64 // Total number of pixels in the NeoPixel matrix (8x8 grid)
static const uint8_t INT_PIN = 0;   // Interrupt pin for the VL53L5CX sensor
static const uint8_t LPN_PIN = 0;   // Low-power mode pin for the VL53L5CX sensor
static const uint8_t INTEGRAL_TIME_MS = 10; // Integration time for sensor measurements

// Servo pins for pan-tilt mechanism
#define TILT_PIN 1 // D0 for tilt servo
#define PAN_PIN 0  // D1 for pan servo

// Servo angle limits and initial positions
float panAngle = 90.0;  // Initial pan position (neutral)
float tiltAngle = 90.0; // Initial tilt position (neutral)
const float MIN_ANGLE = 0.0;  // Minimum servo angle
const float MAX_ANGLE = 180.0; // Maximum servo angle

// --- Helper Functions ---

/**
 * Maps a servo angle to a PWM duty cycle and writes it to the specified pin.
 * @param pin The pin connected to the servo.
 * @param angle The desired angle (0-180 degrees).
 */
void setServoAngle(int pin, float angle) {
    float dutyCycle = map(angle, MIN_ANGLE, MAX_ANGLE, 5, 10); // Map angle to 5-10% duty cycle
    pwmWrite(pin, dutyCycle * 0.01, 50); // Write 50 Hz PWM signal
}

// VL53L5CX sensor object setup
static VL53L5CX_Arduino _sensor(LPN_PIN, INTEGRAL_TIME_MS, VL53L5CX::RES_8X8_HZ_1, &Wire, 0x29);
static volatile bool _gotInterrupt = false; // Flag for interrupt-based data readiness

// NeoPixel matrix setup
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEO_GRB + NEO_KHZ800); // NeoPixel object for 8x8 grid

// Matrix to store distance data from VL53L5CX sensor
int distanceMatrix[8][8];

/**
 * Converts HSB (Hue, Saturation, Brightness) values to RGB values.
 * @param hue The hue value (0-360 degrees).
 * @param saturation The saturation value (0-1).
 * @param brightness The brightness value (0-1).
 * @param r Output: red channel (0-255).
 * @param g Output: green channel (0-255).
 * @param b Output: blue channel (0-255).
 */
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

// --- Setup Function ---
/**
 * Initializes the VL53L5CX sensor, NeoPixel matrix, and servos.
 */
void setup() {
    Serial.begin(115200); // Begin serial communication
    delay(4000); // Wait for setup to stabilize
    Debugger::printf("Serial begun!\n");

    pinMode(INT_PIN, INPUT); // Set interrupt pin mode
    Wire.begin(); // Initialize I2C communication
    delay(1000);

    Debugger::printf("Starting...\n\n");
    delay(1000);

    _sensor.begin(); // Start the VL53L5CX sensor
    pixels.begin();  // Initialize NeoPixel matrix
    randomSeed(analogRead(0)); // Seed random number generator

    // Initialize servo pins
    pinMode(TILT_PIN, OUTPUT);
    pinMode(PAN_PIN, OUTPUT);

    // Set initial servo positions
    setServoAngle(TILT_PIN, tiltAngle);
    setServoAngle(PAN_PIN, panAngle);
}

// --- Main Loop ---
/**
 * Main loop reads sensor data, updates the NeoPixel matrix with distances,
 * and adjusts the servo positions based on the closest detected object.
 */
void loop() {
    // Wait for sensor data to be ready
    while (!_sensor.dataIsReady()) {
        delay(10);
    }

    // Read distance data from the sensor
    _sensor.readData();

    // Populate the distance matrix with sensor readings
    for (auto i = 0; i < _sensor.getPixelCount(); i++) {
        int row = int(i / 8);
        int column = i % 8;

        if (_sensor.getTargetDetectedCount(i) > 0) {
            // Clamp distance to a maximum of 2000 mm
            distanceMatrix[row][column] = std::min(_sensor.getDistanceMm(i), 2000);
        } else {
            // If no target is detected, set distance to 0
            distanceMatrix[row][column] = 0;
        }
    }

    // Update NeoPixel matrix colors based on distances
    for (int i = 0; i < NUMPIXELS; i++) {
        int row = int(i / 8);
        int column = i % 8;

        float hue = map(distanceMatrix[7 - row][column], 0, 2000, 359, 0); // Map distance to hue
        float saturation = 1; // Full saturation
        float brightness = 0.015; // Dim brightness
        int r, g, b;
        HSBtoRGB(hue, saturation, brightness, r, g, b);

        // Set pixel color
        pixels.setPixelColor(i, pixels.Color(r, g, b));
    }
    pixels.show(); // Apply NeoPixel updates

    // Calculate row and column sums to find the closest object
    int rowSums[8] = {0};
    int colSums[8] = {0};
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            rowSums[r] += distanceMatrix[r][c];
            colSums[c] += distanceMatrix[r][c];
        }
    }

    // Find row and column with the smallest sum
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

    // Map row and column indices to servo angles
    tiltAngle = map(minRow, 7, 0, MIN_ANGLE, MAX_ANGLE);
    panAngle = map(minCol, 0, 7, MIN_ANGLE, MAX_ANGLE);

    // Update servo positions
    setServoAngle(TILT_PIN, tiltAngle);
    setServoAngle(PAN_PIN, panAngle);
}


/*
We have built on the library provided by simon d levy for the vl53l5cx sensor https://github.com/simondlevy/VL53L5CX/ .
We have also used the Adafruit NeoPixel library for the neopixel matrix https://github.com/adafruit/Adafruit_NeoPixel .
For the HSB to RGB conversion we have used the bing chat suggested code https://www.bing.com/videos/search?q=hsb+to+rgb+conversion+code&docid=608034073073&mid=3D
With thanks to Teresa pelinski for help with the matrix code for the neopixels and to chat gpt for help with the matrix summing for the servos.

https://www.arduinolearning.com/code/ws2812-8x8-64-led-matrix-arduino-examples.php we used this code to help us with the neopixel matrix
*/