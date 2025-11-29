# Bill of Materials - Pazerville ILI9341 Teensy Project

## Required Components

### Microcontroller
| Item | Part | Qty | Notes |
|------|------|-----|-------|
| MCU | Teensy 4.1 | 1 | Recommended for best performance (600 MHz) |
| MCU | Teensy 4.0 | 1 | Alternative, good performance (400 MHz) |
| MCU | Teensy 3.6 | 1 | Alternative, lower performance (180 MHz) |

### Display
| Item | Part | Qty | Notes |
|------|------|-----|-------|
| TFT | ILI9341 320x240 | 1 | 2.4" or 2.8" common sizes |
| | SPI Interface | - | Built into display module |
| | 3.3V Logic | - | Required, display operates at 3.3V |

### Passive Components
| Item | Part Value | Qty | Purpose |
|------|-----------|-----|---------|
| C | 0.1µF Capacitor | 2 | Power supply decoupling |
| C | 100µF Capacitor | 1 | Main power filtering (optional) |
| R | 10kΩ Resistor | 1 | Optional: Pull-up for MISO |
| | Level Shifter IC | 1 | If using 5V supply (optional) |

### Connectors & Cabling
| Item | Type | Qty | Notes |
|------|------|-----|-------|
| Cable | Jumper wires (F-F) | 8+ | For breadboard connections |
| Cable | USB Micro-B | 1 | Teensy programming/power |
| Cable | Hookup wire | - | For soldered connections |

### Power Supply
| Item | Spec | Notes |
|------|------|-------|
| Supply | 3.3V DC | 500mA minimum |
| Supply | 5V DC (alt) | If using level shifter |
| Source | USB Port | Teensy can be powered from USB |

---

## Assembly Instructions

### 1. Prepare Breadboard Layout

```
Teensy 4.1 Top View (USB facing up)
┌─────────────────────┐
│ Pin 11    Pin 12    │  ← SPI Pins
│ Pin 13             │  ← SCK
│ Pin 10             │  ← CS
│ Pin 9              │  ← DC
│ Pin 8              │  ← RST
│ GND, 3.3V          │  ← Power
└─────────────────────┘
```

### 2. Connect Power Supply

1. Connect 3.3V to Teensy VIN
2. Connect GND to Teensy GND
3. Add 0.1µF capacitor across 3.3V and GND near display
4. (Optional) Add 100µF capacitor across power

### 3. Connect ILI9341 Display

**SPI Connections (Required)**
- Display MOSI → Teensy Pin 11
- Display SCK → Teensy Pin 13
- Display MISO → Teensy Pin 12 (optional)
- Display CS → Teensy Pin 10
- Display DC → Teensy Pin 9
- Display RST → Teensy Pin 8
- Display GND → Teensy GND
- Display VCC → 3.3V

### 4. Verify Connections

Wiring Checklist:
- [ ] Display VCC connected to 3.3V
- [ ] Display GND connected to GND
- [ ] SPI pins connected (MOSI, SCK, MISO)
- [ ] Control pins connected (CS, DC, RST)
- [ ] Capacitors installed on power pins
- [ ] USB cable connected to computer

---

## Purchasing Guide

### Recommended Vendors

**Teensy**
- Source: PJRC (pjrc.com)
- Cost: $20-25
- Availability: Usually in stock

**ILI9341 Display**
- Sources: Amazon, eBay, AliExpress, Adafruit
- Cost: $10-20
- Sizes: 2.4", 2.8" (both work equally)
- Note: Some modules have different pinouts - verify before purchase

**Level Shifter** (if needed)
- Part: 74LVC245 or similar
- Sources: Amazon, Adafruit
- Cost: $2-5
- Only needed if using 5V logic

**Capacitors & Resistors**
- Sources: Digi-Key, Mouser, local electronics shop
- Cost: <$1
- Part number: Generic 0.1µF, 100µF ceramic/electrolytic

---

## Assembly Time Estimate

| Task | Time |
|------|------|
| Component gathering | 10 min |
| Breadboard layout | 15 min |
| Power connections | 5 min |
| SPI connections | 10 min |
| Control pin connections | 5 min |
| Verification/testing | 10 min |
| **Total** | **~55 minutes** |

---

## Cost Breakdown

| Component | Cost | Notes |
|-----------|------|-------|
| Teensy 4.1 | $22 | USB included |
| ILI9341 Display | $12 | With breakout board |
| Jumper Wires | $5 | Pack of 40 |
| Capacitors | $1 | 0.1µF, 100µF |
| USB Cable | $3 | If not included |
| **Total** | **~$43** | Basic setup |

**Optional additions** (+$15-50):
- Level shifter ($3-5)
- Soldering supplies ($10-15)
- Enclosure ($15-30)
- Touch screen adapter ($20-40)

---

## Installation & Testing

### Step 1: Upload Firmware
```bash
platformio run -e teensy41 --target upload
```

### Step 2: Monitor Output
```bash
platformio device monitor --baud 115200
```

Expected output:
```
Initializing Pazerville ILI9341 Display...
Display initialized successfully!
Pazerville initialized successfully!
Setup complete!
FPS: 52
FPS: 54
```

### Step 3: Verify Display
- Screen should show 7-node network graph
- Nodes should animate smoothly
- Screen should not be garbled or blank

### Step 4: Test Serial Commands
Connect to serial monitor at 115200 baud:
- Send `r` → Nodes should randomize positions
- Send `d` → Animation should slow down
- Send `u` → Animation should speed up

---

## Pinout Reference

### Teensy 4.1 SPI Pins (Recommended)
```
Pin 11: MOSI (SPI Data Out from Teensy)
Pin 12: MISO (SPI Data In to Teensy - optional)
Pin 13: SCK  (SPI Clock)
Pin 10: CS   (Chip Select) - custom, configurable
Pin 9:  DC   (Data/Command) - custom, configurable
Pin 8:  RST  (Reset) - custom, configurable
```

### Teensy 3.6 SPI Pins (Alternative)
```
Pin 11: MOSI
Pin 12: MISO (optional)
Pin 13: SCK
```

### Alternative SPI Pins (Teensy 4.x)
```
Primary:   MOSI=11, MISO=12, SCK=13
Secondary: MOSI=7,  MISO=8,  SCK=14
```

---

## Troubleshooting Components

### Display Not Responding

**Symptoms**: No image, garbled display, or black screen

**Check**:
1. [ ] VCC is 3.3V (not 5V!)
2. [ ] GND connection is solid
3. [ ] All SPI pins connected (11, 12, 13)
4. [ ] Control pins not swapped (8, 9, 10)
5. [ ] Capacitors installed on power pins
6. [ ] No loose wires or cold solder joints

### Poor Performance

**Symptoms**: Slow animation, jerky movement

**Possible causes**:
- [ ] Using Teensy 3.6 (slower processor)
- [ ] Too many nodes (reduce to <7)
- [ ] High damping value (reduce to <0.90)
- [ ] SPI speed too low (should be 40MHz)

### Communication Errors

**Symptoms**: Serial monitor shows nothing

**Check**:
1. [ ] USB cable is original/quality
2. [ ] Teensy board is recognized (device manager)
3. [ ] Correct COM port selected
4. [ ] Baud rate is 115200
5. [ ] Board driver installed (Teensyduino)

---

## Pro Tips

1. **Soldering**: If permanence desired, solder connections instead of breadboard
2. **Power**: Use USB power from computer - simpler and more reliable
3. **Isolation**: Keep display away from EMI sources (high-speed logic)
4. **Cooling**: Display doesn't require active cooling, passive is fine
5. **SPI Speed**: 40MHz is stable; go lower if experiencing glitches
6. **Capacitors**: 0.1µF close to power pins; 100µF away is preferred
7. **Testing**: Use oscilloscope to verify SPI signals if troubleshooting

---

## Spare Parts Recommendation

Keep extras on hand:
- Teensy (in case of bricked board): 1
- ILI9341 Display: 1
- USB Cable: 1
- Capacitors (0.1µF, 100µF): 5 each
- Jumper wires: Extra pack

---

## Disposal & Recycling

When hardware reaches end-of-life:
- **Teensy**: Reuse in other projects
- **Display**: Most e-waste recyclers accept
- **Capacitors/Resistors**: Small parts recyclers or trash
- **USB Cable**: Donate or recycle as electronics waste

---

## References

- Teensy Product Page: https://www.pjrc.com/teensy/
- ILI9341 Datasheet: Search online (PDF)
- SPI Protocol: https://en.wikipedia.org/wiki/Serial_Peripheral_Interface

---

**Last Updated**: November 29, 2025
**Project**: Pazerville ILI9341 Teensy Implementation
**Status**: Complete ✅
