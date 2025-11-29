# Quick Start Guide

## Hardware Setup

### ILI9341 Display Pinout
```
Connect to Teensy as follows:
- GND → GND
- VCC → 3.3V
- CS → Pin 10
- RST → Pin 8  
- DC → Pin 9
- MOSI → Pin 11 (SPI MOSI)
- SCK → Pin 13 (SPI CLK)
- MISO → Pin 12 (optional, for read operations)
```

## Software Setup

### Option 1: PlatformIO (Recommended)
```bash
# Install PlatformIO if you haven't already
pip install platformio

# Clone or enter the project directory
cd teensy-sinphony

# Build for Teensy 4.1
platformio run -e teensy41

# Upload to device
platformio run -e teensy41 --target upload

# Monitor serial output
platformio device monitor
```

### Option 2: Arduino IDE
1. Install Teensyduino addon
2. Open `src/main.cpp` in Arduino IDE
3. Select Board: Teensy 4.1 (or your Teensy version)
4. Select Upload Speed: 921600
5. Click Upload
6. Open Serial Monitor (115200 baud) to see output

## First Run

After uploading, you should see:
1. Display initializes (may take a few seconds)
2. A network graph with 7 nodes appears on screen
3. Nodes animate with physics-based movement

## Testing Commands

Open serial monitor at 115200 baud and try:
- `r` - Randomize node positions
- `s` - Stop animation
- `d` - Increase damping (slower movement)
- `u` - Decrease damping (faster movement)
- `+` - Increase spring strength
- `-` - Decrease spring strength

## Customizing the Display

Edit `src/main.cpp`:

### Change number of nodes:
```cpp
int num_nodes = 6;  // Change this value
```

### Adjust spring forces:
```cpp
pazerville->addEdge(i, j, 0.2f, 80.0f);  // Change 0.2f for strength
```

### Modify damping:
```cpp
pazerville->setDamping(0.90f);  // Lower = faster, Higher = slower
```

## Troubleshooting

**Display shows nothing:**
- Check power supply to display (3.3V, ~100mA)
- Verify SPI connections (CS, RST, DC)
- Check serial output for error messages

**Display is garbled:**
- Lower SPI speed (modify `spi_speed` in ili9341_display.cpp)
- Add 0.1µF capacitors near display power pins

**Animation is jerky:**
- Reduce number of nodes
- Increase damping value
- Use Teensy 4.1 for better performance

## Next Steps

- Modify `createTestNetwork()` to create different topologies
- Add keyboard/button input for interactive control
- Create specialized visualizations for CV data
- Add MIDI input processing
