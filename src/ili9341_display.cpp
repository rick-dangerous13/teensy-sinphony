#include "../include/ili9341_display.h"

// Constructor
ILI9341Display::ILI9341Display() {
    buffer.width = ILI9341_WIDTH;
    buffer.height = ILI9341_HEIGHT;
    buffer.framebuffer = nullptr;
    buffer.dirty_lines = nullptr;
    buffer.is_initialized = false;
    spi_speed = 40000000;  // 40 MHz SPI speed
}

// Destructor
ILI9341Display::~ILI9341Display() {
    if (buffer.framebuffer) {
        delete[] buffer.framebuffer;
    }
    if (buffer.dirty_lines) {
        delete[] buffer.dirty_lines;
    }
}

// Write command to display
void ILI9341Display::writeCommand(uint8_t cmd) {
    digitalWrite(TFT_DC, LOW);
    SPI.transfer(cmd);
}

// Write data byte to display
void ILI9341Display::writeData(uint8_t data) {
    digitalWrite(TFT_DC, HIGH);
    SPI.transfer(data);
}

// Write 16-bit data to display
void ILI9341Display::writeData16(uint16_t data) {
    digitalWrite(TFT_DC, HIGH);
    SPI.transfer16(data);
}

// Read data from display
uint8_t ILI9341Display::readData(void) {
    digitalWrite(TFT_DC, HIGH);
    return SPI.transfer(0);
}

// Set the address window for drawing
void ILI9341Display::setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    // Column address set
    writeCommand(ILI9341_COLADDRSET);
    writeData16(x0);
    writeData16(x1);
    
    // Row address set
    writeCommand(ILI9341_ROWADDRSET);
    writeData16(y0);
    writeData16(y1);
}

// Initialize the display
bool ILI9341Display::initialize() {
    pinMode(TFT_CS, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    pinMode(TFT_RST, OUTPUT);
    
    // Allocate framebuffer
    buffer.framebuffer = new uint16_t[ILI9341_WIDTH * ILI9341_HEIGHT];
    if (!buffer.framebuffer) {
        return false;
    }
    
    // Allocate dirty line tracking
    buffer.dirty_lines = new uint16_t[ILI9341_HEIGHT];
    if (!buffer.dirty_lines) {
        return false;
    }
    
    // Initialize SPI
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    
    // Reset display
    reset();
    
    digitalWrite(TFT_CS, LOW);
    
    // Software reset
    writeCommand(ILI9341_SOFTRESET);
    delay(150);
    
    // Sleep out
    writeCommand(ILI9341_SLEEPOUT);
    delay(150);
    
    // Pixel format set (16-bit RGB565)
    writeCommand(ILI9341_PIXELFORMAT);
    writeData(0x55);  // 16-bit per pixel
    
    // Frame rate control (Normal mode)
    writeCommand(ILI9341_FRAMERATECTRL);
    writeData(0x00);
    writeData(0x1B);
    
    // Display function control
    writeCommand(ILI9341_DISPLAYFUNC);
    writeData(0x0A);
    writeData(0x82);
    writeData(0x27);
    writeData(0x00);
    
    // Entry mode set
    writeCommand(ILI9341_ENTRYMODE);
    writeData(0x07);
    
    // Power control 1
    writeCommand(ILI9341_POWERCTRL1);
    writeData(0x10);
    
    // Power control 2
    writeCommand(ILI9341_POWERCTRL2);
    writeData(0x10);
    
    // VCOM control 1
    writeCommand(ILI9341_VCOMCTRL1);
    writeData(0x45);
    writeData(0x15);
    
    // VCOM control 2
    writeCommand(ILI9341_VCOMCTRL2);
    writeData(0x90);
    
    // Invert on
    writeCommand(ILI9341_INVERTON);
    
    // Display on
    displayOn();
    
    // Clear screen
    fillScreen(COLOR_BLACK);
    
    buffer.is_initialized = true;
    
    digitalWrite(TFT_CS, HIGH);
    
    return true;
}

// Reset the display
void ILI9341Display::reset() {
    digitalWrite(TFT_RST, HIGH);
    delay(100);
    digitalWrite(TFT_RST, LOW);
    delay(100);
    digitalWrite(TFT_RST, HIGH);
    delay(100);
}

// Turn display on
void ILI9341Display::displayOn() {
    digitalWrite(TFT_CS, LOW);
    writeCommand(ILI9341_DISPLAYON);
    digitalWrite(TFT_CS, HIGH);
}

// Turn display off
void ILI9341Display::displayOff() {
    digitalWrite(TFT_CS, LOW);
    writeCommand(ILI9341_DISPLAYOFF);
    digitalWrite(TFT_CS, HIGH);
}

// Set display rotation
void ILI9341Display::setRotation(uint8_t rotation) {
    digitalWrite(TFT_CS, LOW);
    writeCommand(ILI9341_ENTRYMODE);
    
    switch (rotation) {
        case 0:
            writeData(0x07);
            break;
        case 1:
            writeData(0x03);
            break;
        case 2:
            writeData(0x06);
            break;
        case 3:
            writeData(0x04);
            break;
    }
    
    digitalWrite(TFT_CS, HIGH);
}

// Fill entire screen with a color
void ILI9341Display::fillScreen(uint16_t color) {
    fillRect(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, color);
}

// Fill a rectangle with a color
void ILI9341Display::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if ((x + w - 1) >= ILI9341_WIDTH) {
        w = ILI9341_WIDTH - x;
    }
    if ((y + h - 1) >= ILI9341_HEIGHT) {
        h = ILI9341_HEIGHT - y;
    }
    
    setAddressWindow(x, y, x + w - 1, y + h - 1);
    
    digitalWrite(TFT_CS, LOW);
    writeCommand(ILI9341_MEMWRITE);
    
    uint32_t pixels = (uint32_t)w * h;
    
    while (pixels--) {
        writeData16(color);
    }
    
    digitalWrite(TFT_CS, HIGH);
}

// Draw a single pixel
void ILI9341Display::drawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if ((x < 0) || (x >= ILI9341_WIDTH) || (y < 0) || (y >= ILI9341_HEIGHT)) {
        return;
    }
    
    setAddressWindow(x, y, x, y);
    
    digitalWrite(TFT_CS, LOW);
    writeCommand(ILI9341_MEMWRITE);
    writeData16(color);
    digitalWrite(TFT_CS, HIGH);
}

// Draw a line using Bresenham's algorithm
void ILI9341Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx - dy;
    
    while (true) {
        drawPixel(x0, y0, color);
        
        if ((x0 == x1) && (y0 == y1)) {
            break;
        }
        
        int16_t e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// Draw a rectangle outline
void ILI9341Display::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    drawLine(x, y, x + w - 1, y, color);
    drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
    drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
    drawLine(x, y, x, y + h - 1, color);
}

// Draw a circle using Midpoint Circle Algorithm
void ILI9341Display::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t x = 0;
    int16_t y = r;
    int16_t dp = 1 - r;
    
    while (x <= y) {
        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
        
        if (dp < 0) {
            dp = dp + 2 * x + 3;
        } else {
            dp = dp + 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

// Draw a triangle
void ILI9341Display::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

// Draw a character (simplified 5x7 ASCII)
void ILI9341Display::drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) {
    // Simplified character drawing - just draw a filled rectangle for now
    // Full font implementation would require font data arrays
    fillRect(x, y, 5 * size, 7 * size, bg);
    drawRect(x, y, 5 * size, 7 * size, color);
}

// Draw a string
void ILI9341Display::drawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size) {
    uint16_t pos_x = x;
    
    while (*str) {
        drawChar(pos_x, y, *str, color, bg, size);
        pos_x += 6 * size;
        str++;
    }
}

// Update the entire display from framebuffer
void ILI9341Display::updateDisplay() {
    updateRect(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT);
}

// Update a rectangular region of the display
void ILI9341Display::updateRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    if (!buffer.is_initialized || !buffer.framebuffer) {
        return;
    }
    
    setAddressWindow(x, y, x + w - 1, y + h - 1);
    
    digitalWrite(TFT_CS, LOW);
    writeCommand(ILI9341_MEMWRITE);
    
    for (uint16_t py = y; py < y + h; py++) {
        for (uint16_t px = x; px < x + w; px++) {
            uint16_t pixel = buffer.framebuffer[py * ILI9341_WIDTH + px];
            writeData16(pixel);
        }
    }
    
    digitalWrite(TFT_CS, HIGH);
}

// Get the framebuffer pointer
uint16_t* ILI9341Display::getFramebuffer() {
    return buffer.framebuffer;
}

// Convert RGB values to RGB565 color
uint16_t ILI9341Display::rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Set contrast level (0-255)
void ILI9341Display::setContrast(uint8_t level) {
    digitalWrite(TFT_CS, LOW);
    writeCommand(ILI9341_VCOMCTRL1);
    writeData(level);
    digitalWrite(TFT_CS, HIGH);
}

// Set brightness level (0-255)
void ILI9341Display::setBrightness(uint8_t level) {
    // PWM control on TFT_RST pin if available
    // For now, just adjust contrast
    setContrast(level);
}
