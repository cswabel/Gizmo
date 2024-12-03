# Servo-Controlled Distance Mapping with NeoPixel Visualization

This project combines a VL53L5CX Time-of-Flight (ToF) sensor, NeoPixel LEDs, and servo motors to create an interactive distance-mapping system. The setup visualizes distance data on an 8x8 NeoPixel matrix, maps the closest object to servo movements, and provides a colorful, dynamic display.

## Features

- **Distance Sensing:**  
  Utilizes the VL53L5CX ToF sensor in an 8x8 multi-zone configuration to measure distances up to 2 meters with high precision.

- **NeoPixel Visualization:**  
  Maps distance data onto an 8x8 NeoPixel matrix, using color gradients (HSB to RGB conversion) to represent proximity, where closer objects appear in warmer colors.

- **Servo Movement:**  
  Two servos (pan and tilt) dynamically adjust based on the closest detected object's position in the 8x8 grid, aligning the servos to "track" the object.

- **Real-Time Processing:**  
  Processes and visualizes distance data in real-time, making it suitable for interactive installations.

---

## Hardware Requirements

1. **VL53L5CX ToF Sensor**  
   Configured for 8x8 multi-zone ranging, communicates via I2C.

2. **Adafruit NeoPixel 8x8 Matrix**  
   Displays distance data with customizable RGB colors.

3. **Two Servo Motors**  
   (e.g., SG90 or MG90S) for pan and tilt motion, controlled via PWM signals.

4. **Bela Platform**  
   Provides the computational power for real-time audio and physical computing tasks.

5. **Power Supply**  
   Ensure adequate power for NeoPixel LEDs and servos (e.g., 5V, 2A supply).

---

## Software Overview

### Key Libraries Used

1. **Bela Libraries**  
   - `Bela.h`: For real-time computing.  
   - `BelaArduino`: Enables Arduino-like programming.  
   - `Wire.h`: Handles I2C communication with the VL53L5CX sensor.

2. **VL53L5CX Driver**  
   Communicates with the ToF sensor to retrieve distance data.

3. **Adafruit NeoPixel**  
   Manages RGB LED control and matrix visualization.

4. **Custom Helper Functions**  
   - **HSB to RGB Conversion:** Converts hue, saturation, and brightness to RGB for dynamic color mapping.  
   - **Servo Angle Mapping:** Maps grid positions to servo angles and generates appropriate PWM signals.

---

## Code Flow

### 1. **Setup Phase**  
- Initializes the VL53L5CX sensor, NeoPixel matrix, and servos.
- Sets up I2C communication for distance data retrieval.
- Positions servos at a default starting angle of 90°.

### 2. **Distance Mapping**  
- Reads data from the VL53L5CX sensor, storing it in a 2D matrix.
- Maps distance values to hues for NeoPixel visualization.

### 3. **Visualization**  
- Updates the NeoPixel matrix with colors representing distance values:
  - Red for closer objects.
  - Blue for farther objects.

### 4. **Servo Control**  
- Calculates row and column sums to identify the closest object.
- Maps the closest object's grid position to servo angles:
  - **Row** adjusts tilt.
  - **Column** adjusts pan.
- Updates servos in real-time to track the object's position.

---

## How to Use

### 1. **Hardware Setup**
   - Connect the VL53L5CX sensor to the Bela board via I2C.
   - Connect the NeoPixel matrix and ensure it has sufficient power.
   - Attach the servos to Bela's PWM-enabled pins (e.g., `D0` for tilt and `D1` for pan).

### 2. **Build and Run**
   - Clone this repository to your Bela environment.
   - Compile and run the program using Bela's build tools.

### 3. **Expected Output**
   - The NeoPixel matrix displays a dynamic color gradient based on distance data.
   - The servos move to align with the closest detected object's position on the grid.

---

## Customization

- **Adjusting Distance Limits:**  
  Modify the `map` function's input range in the code to change distance thresholds for visualization.
  
- **NeoPixel Brightness:**  
  Adjust the brightness variable in `HSBtoRGB()` to dim or brighten the LED display.

- **Servo Speed:**  
  Tune the delay values or increments in the servo movement logic for smoother or faster tracking.

---

## Future Enhancements

- Add recording and playback functionality for servo movements and NeoPixel patterns.
- Integrate additional sensors for richer interaction.
- Optimize performance for larger NeoPixel matrices or higher refresh rates.

---

## Author

This project leverages the Bela platform for real-time physical computing and audio processing. It showcases interactive distance visualization and dynamic servo control in a compact and modular system.
