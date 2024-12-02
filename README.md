# Gizmo
This code integrates multiple libraries to create a project using the Bela platform, VL53L5CX sensor, and Adafruit NeoPixel matrix. Here's what it does:

### Libraries and Setup
- **Bela Libraries**: Used for real-time audio and physical computing.
- **VL53L5CX Sensor**: An 8x8 multizone Time-of-Flight (ToF) sensor for measuring distances.
- **Adafruit NeoPixel**: Controls an 8x8 RGB LED matrix.
- **Debugger**: Provides debug logging via the Bela framework.

---

### **Key Functionalities**

#### **1. Distance Measurement (VL53L5CX)**
- Configures the ToF sensor with an 8x8 resolution and a specific I2C address.
- Reads distance data for each of the 64 zones on the sensor.
- If a distance exceeds 2000 mm (sensor's max reliable range), it caps it at 2000 mm.
- Distance values are stored in a `distanceMatrix[8][8]`.

#### **2. Distance-to-Color Mapping**
- Uses the `HSBtoRGB` function to convert distances into RGB colors:
  - Hue is mapped from 359 (red) to 0 (blue) based on the distance.
  - Saturation is fixed at 1, and brightness at 0.05.
- For each NeoPixel, the RGB values are calculated and sent to the matrix using the `setPixelColor()` function.

#### **3. LED Matrix Display**
- The 8x8 NeoPixel matrix is updated with the computed colors.
- `pixels.show()` sends the updated color data to the hardware.

#### **4. Debugging**
- Prints sensor data (zone, target count, ambient light, distance) to the debugger for each read.

---

### **Functions and Logic**

1. **`setup()`**
   - Initializes the serial interface, sensor, NeoPixel matrix, and sets random seed for the NeoPixel library.

2. **`loop()`**
   - Waits for the sensor to indicate data readiness.
   - Reads the ToF data and populates `distanceMatrix` for each 8x8 zone.
   - Updates NeoPixel colors by mapping distances to hue and converting them to RGB.

3. **`HSBtoRGB()`**
   - Converts HSB (Hue, Saturation, Brightness) values into RGB for display on the NeoPixel matrix.

---

### **Highlights**
- **Real-Time Updates**: Uses Bela's real-time framework to continuously read sensor data and update the NeoPixel matrix.
- **Interactive Display**: Converts spatial distance information into a vivid visual representation using color gradients.
- **Flexible Configuration**: Sensor's resolution and NeoPixel matrix are modular, enabling adaptability for other projects.

---

### **Improvements and Next Steps**
- **Interrupt Handling**: Although interrupt logic is included, it’s commented out. Using interrupts can improve efficiency by triggering reads only when new data is available.
- **Optimization**: Updating all 64 NeoPixels in each loop iteration can be CPU-intensive. Consider grouping updates or using a delay for smoother transitions.
- **Additional Features**: Implement interaction logic (e.g., button-based control for toggling modes or pausing updates). 

Would you like assistance enhancing any specific part of the code?
