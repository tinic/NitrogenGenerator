# Nitrogen Generator Project

## Project Overview

This project implements a custom nitrogen generator for home brewing applications. It extracts nitrogen from ambient air using membrane separation technology, providing a cost-effective alternative to commercial N2 cylinders for serving non-carbonated beverages like cold brew coffee.

## Key Features

- **Membrane-based nitrogen separation**: Uses MNH-1512A membrane to extract 99.5%+ pure nitrogen from air
- **Automated control system**: Custom STM32-based controller manages pressure, solenoids, and system state
- **LCD display**: Real-time monitoring of system status and pressures
- **Safety features**: Pressure monitoring, fault detection, and automatic shutoff
- **Target output**: 50L/h (2CFH) nitrogen at 100 PSI
- **Power efficient**: Only runs when nitrogen is needed

## Architecture

### Hardware Components
1. **Controller**: STM32G030 microcontroller on custom PCB
2. **Display**: 192x64 monochrome LCD (ST7525 driver)
3. **Sensors**: Two pressure transducers (0-200 PSI range)
4. **Actuators**: Two 24V solenoid valves
5. **Power**: 24V input with 3.3V regulation
6. **Air system**: Membrane, filters, needle valves, check valve

### Software Architecture
- **Firmware**: C++ running on STM32 (bare metal with HAL)
- **Display library**: Constixel for graphics rendering
- **Control logic**: State machine managing refill cycles
- **Safety monitoring**: Continuous pressure and fault checking

## Control Logic

The system operates in cycles:
1. When output pressure < 75 PSI, start refill cycle
2. Open input solenoid after 10s delay (soft start)
3. Open output solenoid once membrane pressure > 75 PSI
4. Close both solenoids when output reaches 100 PSI
5. Monitor for fault conditions continuously

### Safety Features
- Pressure range monitoring (-5 to 130 PSI limits)
- Automatic shutoff on fault conditions
- Hysteresis timers to prevent oscillation
- Input pressure limits to protect membrane

## Project Structure

```
/Users/turo/nitrogengenerator/
├── CubeMX/                 # STM32 firmware project
│   ├── Core/               # STM32 HAL and main code
│   ├── constixel/          # Graphics library
│   ├── mcp.cpp/h           # Main control program logic
│   ├── screen.cpp/h        # Display driver and UI
│   └── CMakeLists.txt      # CMake build configuration
├── NitrogenGenerator.*     # KiCad PCB design files
├── PnID.kicad_sch          # Process & Instrumentation Diagram
├── solidworks/             # Mechanical design files
├── docs/                   # Component datasheets
└── images/                 # Documentation images
```

## Building

### Firmware
```bash
cd CubeMX
./build.sh
```

Requirements:
- ARM GCC toolchain (arm-none-eabi-gcc)
- CMake 3.22+
- Python 3 (for font/image conversion)

### PCB
- Open NitrogenGenerator.kicad_pro in KiCad
- Use Makefile for production file generation

## Development Notes

### Adding Features
- Control logic: Modify `mcp.cpp` Slice() function
- Display updates: Edit `screen.cpp` draw_ui() method
- New sensors: Add ADC channels in STM32CubeMX

### Testing
- Use screen_test directory for display development
- Monitor pressures via LCD during operation
- Check duty cycle statistics for efficiency

### Common Tasks
- **Adjust needle valves**: See README.md procedure
- **Change pressure setpoints**: Edit thresholds in mcp.cpp
- **Add status displays**: Modify screen.cpp UI layout

## Important Parameters

- **Input pressure limit**: 120 PSI (membrane rating)
- **Output pressure range**: 75-100 PSI
- **Refill delay**: 10 seconds (soft start)
- **Shutoff delay**: 120 seconds (max refill time)
- **ADC channels**: PA0 (pressure 1), PA1 (pressure 2)
- **SPI**: Display communication
- **GPIO**: Solenoid control pins

## Safety Warnings

⚠️ **High Pressure System**: Always wear safety equipment
⚠️ **Membrane Protection**: Never exceed 120 PSI input
⚠️ **Electrical**: 24V DC present, proper grounding required
⚠️ **Oxygen Depletion**: Ensure adequate ventilation

## Maintenance

- Replace air filters per manufacturer schedule
- Check for leaks regularly
- Monitor nitrogen purity with oxygen sensor
- Clean membrane if efficiency drops