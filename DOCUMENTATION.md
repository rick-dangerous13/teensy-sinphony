# Pazerville Module with ILI9341 Display

A high-resolution Pazerville module implementation for Teensy microcontrollers using an ILI9341 320x240 TFT display.

## Features

- **High-Resolution Display**: 320x240 pixel ILI9341 TFT display
- **Force-Directed Graph Visualization**: Interactive physics-based network rendering
- **Real-Time Simulation**: Fast physics updates with spring forces and damping
- **Interactive Control**: Serial-based parameter control
- **Optimized for Teensy**: Supports Teensy 3.6, 4.0, and 4.1

## Hardware Requirements

### Microcontroller
- Teensy 3.6, 4.0, or 4.1 (recommended: 4.1 for best performance)

### Display
- ILI9341 320x240 TFT Display Module
- SPI interface
- 3.3V logic levels

### Wiring Diagram

| ILI9341 Pin | Teensy Pin | Function |
|-------------|-----------|----------|
| GND         | GND       | Ground   |
| VCC         | 3.3V      | Power    |
| CS          | 10        | Chip Select |
| RST         | 8         | Reset    |
| DC          | 9         | Data/Command |
| MOSI        | 11        | SPI Data In |
| SCK         | 13        | SPI Clock |
| MISO        | 12        | SPI Data Out (optional) |

**Note**: The ILI9341 operates at 3.3V. If your power supply is 5V, use a level shifter for the data lines.

## Software Installation

### Using PlatformIO (Recommended)

1. Install PlatformIO CLI or VS Code extension
2. Clone this repository
3. Navigate to the project directory
4. Build and upload:

```bash
platformio run -e teensy41 --target upload
```

### Using Arduino IDE

1. Install Teensyduino add-on
2. Copy the source files into an Arduino sketch
3. Update the `#include` paths
4. Select appropriate Teensy board and upload speed
5. Compile and upload

## Project Structure

```
teensy-sinphony/
├── include/
│   ├── ili9341_display.h        # ILI9341 driver header
│   └── pazerville_display.h     # Pazerville module header
├── src/
│   ├── main.cpp                 # Main sketch
│   ├── ili9341_display.cpp      # ILI9341 driver implementation
│   └── pazerville_display.cpp   # Pazerville implementation
├── platformio.ini               # PlatformIO configuration
└── README.md                    # This file
```

## API Documentation

### ILI9341Display Class

Main display driver for the ILI9341 TFT.

**Key Methods:**
- `bool initialize()` - Initialize display and allocate buffers
- `void fillScreen(uint16_t color)` - Fill entire screen with color
- `void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)` - Fill rectangle
- `void drawPixel(uint16_t x, uint16_t y, uint16_t color)` - Draw single pixel
- `void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)` - Bresenham line
- `void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)` - Circle outline
- `void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)` - Rectangle outline
- `void drawTriangle(...)` - Triangle outline
- `void updateDisplay()` - Refresh entire display
- `void updateRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h)` - Update region
- `uint16_t rgb(uint8_t r, uint8_t g, uint8_t b)` - Convert RGB to RGB565

**Color Macros:**
```cpp
COLOR_BLACK, COLOR_BLUE, COLOR_RED, COLOR_GREEN
COLOR_CYAN, COLOR_MAGENTA, COLOR_YELLOW, COLOR_WHITE
COLOR_GRAY, COLOR_ORANGE
```

### PazervilleDisplay Class

Force-directed graph visualization and physics engine.

**Key Methods:**
- `bool initialize()` - Initialize Pazerville renderer
- `void addNode(float x, float y, float mass, uint16_t color, uint8_t radius)` - Add graph node
- `void addEdge(int node1, int node2, float spring_constant, float rest_length)` - Connect nodes
- `void update()` - Run physics simulation step
- `void draw()` - Render graph to display
- `void setDamping(float d)` - Set velocity damping (0.8-0.99)
- `void setGravity(float g)` - Set gravitational force
- `void repelNode(int node_id, float force_x, float force_y)` - Push node away
- `void attractToPoint(int node_id, float target_x, float target_y, float strength)` - Pull node toward point
- `void randomizePositions()` - Reset node positions randomly
- `void resetSimulation()` - Clear all velocities

## Simulation Parameters

### Spring-Mass System

The physics simulation uses a spring-mass system with:
- **Spring Force**: `F = k * (distance - rest_length)`
- **Damping**: Reduces velocity by factor each frame
- **Gravity**: Constant downward acceleration
- **Boundary Constraints**: Nodes bounce off screen edges

### Configuration

Adjust in `main.cpp`:
```cpp
float spring_strength = 0.15f;      // Spring stiffness
float damping_factor = 0.92f;       // Velocity decay (0.8-0.99)
```

## Serial Commands

Connect via USB serial at 115200 baud. Supported commands:

| Command | Action |
|---------|--------|
| `r` | Randomize node positions |
| `s` | Stop simulation (reset velocities) |
| `d` | Increase damping |
| `u` | Decrease damping |
| `+` | Increase spring strength |
| `-` | Decrease spring strength |

Example:
```bash
screen /dev/ttyACM0 115200
```

## Performance Optimization

### Frame Rate
- Teensy 4.1: ~45-60 FPS with 7 nodes
- Teensy 4.0: ~30-40 FPS with 5 nodes
- Teensy 3.6: ~15-25 FPS with 3 nodes

### Tips for Better Performance
- Reduce number of nodes (max 8 in current config)
- Increase damping to reduce calculations
- Lower spring strength
- Use `updateRect()` instead of `updateDisplay()` for partial updates
- Compile with `-O2` optimization flag

## Display Color Format

Colors use **RGB565** format (16-bit):
- 5 bits for red (0-31)
- 6 bits for green (0-63)
- 5 bits for blue (0-31)

Convert from RGB888:
```cpp
uint16_t color = tft->rgb(255, 128, 0);  // Orange
```

## Troubleshooting

### Display Not Showing
- Check SPI wiring (SCK, MOSI, MISO)
- Verify CS, DC, RST pins are correct
- Ensure 3.3V power supply with adequate current (100mA+)

### Garbled Display
- Reduce SPI speed in `ili9341_display.cpp`
- Add decoupling capacitor (0.1µF) near display power pins

### Physics Simulation Slow
- Reduce number of nodes/edges
- Increase `time_step` in main.cpp
- Decrease spring force
- Increase damping

## References

- [Teensy Documentation](https://www.pjrc.com/teensy/)
- [ILI9341 Datasheet](https://www.displayfuture.com/Display/PDF/ILI9341.pdf)
- [Force-Directed Graph Layouts](https://en.wikipedia.org/wiki/Force-directed_graph_drawing)
- [Ornament and Crime Project](https://ornament-and-crime.org/)

## License

This code is provided for educational and commercial use.

## Author

Adapted from Ornament and Crime module for Teensy + ILI9341 Display
