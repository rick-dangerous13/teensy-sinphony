#ifndef ILI9341_DISPLAY_H
#define ILI9341_DISPLAY_H

#include <Arduino.h>
#include <SPI.h>

// ILI9341 Display Configuration
#define ILI9341_WIDTH   320
#define ILI9341_HEIGHT  240
#define ILI9341_BPP     16  // 16-bit color (RGB565)

// Teensy 4.x Pin Configuration
#define TFT_CS    10    // Chip Select
#define TFT_DC    9     // Data/Command
#define TFT_RST   8     // Reset
#define TFT_CLK   13    // SPI Clock (SCK)
#define TFT_MOSI  11    // SPI Data In (MOSI)
#define TFT_MISO  12    // SPI Data Out (MISO)

// ILI9341 Commands
#define ILI9341_SOFTRESET       0x01
#define ILI9341_SLEEPIN         0x10
#define ILI9341_SLEEPOUT        0x11
#define ILI9341_PARTIALON       0x12
#define ILI9341_NORMALON        0x13
#define ILI9341_INVERTOFF       0x20
#define ILI9341_INVERTON        0x21
#define ILI9341_GAMMASET        0x26
#define ILI9341_DISPLAYOFF      0x28
#define ILI9341_DISPLAYON       0x29
#define ILI9341_COLADDRSET      0x2A
#define ILI9341_ROWADDRSET      0x2B
#define ILI9341_MEMWRITE        0x2C
#define ILI9341_MEMREAD         0x2E
#define ILI9341_PIXELFORMAT     0x3A
#define ILI9341_FRAMERATECTRL   0xB1
#define ILI9341_DISPLAYFUNC     0xB6
#define ILI9341_ENTRYMODE       0xB7
#define ILI9341_POWERCTRL1      0xC0
#define ILI9341_POWERCTRL2      0xC1
#define ILI9341_VCOMCTRL1       0xC5
#define ILI9341_VCOMCTRL2       0xC7
#define ILI9341_CLPADDRSET      0xF0

// RGB565 Color Definitions
#define COLOR_BLACK     0x0000
#define COLOR_BLUE      0x001F
#define COLOR_RED       0xF800
#define COLOR_GREEN     0x07E0
#define COLOR_CYAN      0x07FF
#define COLOR_MAGENTA   0xF81F
#define COLOR_YELLOW    0xFFE0
#define COLOR_WHITE     0xFFFF
#define COLOR_GRAY      0x8410
#define COLOR_ORANGE    0xFDA0

// Display Buffer - using 16-bit RGB565 format
typedef struct {
    uint16_t *framebuffer;
    uint16_t width;
    uint16_t height;
    uint16_t *dirty_lines;  // Track which lines need updating
    bool is_initialized;
} DisplayBuffer;

// ILI9341 Driver Class
class ILI9341Display {
private:
    DisplayBuffer buffer;
    uint32_t spi_speed;
    
    void writeCommand(uint8_t cmd);
    void writeData(uint8_t data);
    void writeData16(uint16_t data);
    uint8_t readData(void);
    void setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    
public:
    ILI9341Display();
    ~ILI9341Display();
    
    bool initialize();
    void reset();
    void displayOn();
    void displayOff();
    void setRotation(uint8_t rotation);
    
    // Drawing functions
    void fillScreen(uint16_t color);
    void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void drawPixel(uint16_t x, uint16_t y, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    
    // Text functions
    void drawChar(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t size = 1);
    void drawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t size = 1);
    
    // Buffer functions
    void updateDisplay();
    void updateRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    uint16_t* getFramebuffer();
    
    // Utility functions
    uint16_t rgb(uint8_t r, uint8_t g, uint8_t b);
    void setContrast(uint8_t level);
    void setBrightness(uint8_t level);
    
    // Get display dimensions
    uint16_t getWidth() const { return ILI9341_WIDTH; }
    uint16_t getHeight() const { return ILI9341_HEIGHT; }
};

#endif // ILI9341_DISPLAY_H
