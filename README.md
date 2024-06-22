# Simple Clock using DS1307 RTC Module (compatible with DS3231)

This Arduino project implements a simple clock using the DS1307 RTC module, with compatibility 
also extended to the DS3231 module. It features custom implementations of UART and I2C libraries 
for enhanced control and efficiency in communication with peripherals.

## Project Details

### Memory Usage
- **RAM**: 0.8% (used 16 bytes from 2048 bytes)
- **Flash**: 1.9% (used 628 bytes from 32256 bytes)

## Features

- Displays real-time clock (RTC) data (hours, minutes, seconds) on the Serial Monitor.
- Supports both DS1307 and DS3231 RTC modules.
- Custom UART library for optimized serial communication.
- Custom I2C library for efficient interfacing with RTC modules.

## Components Used

- Arduino board (tested with Atmega328p)
- DS1307 or DS3231 RTC module
- Serial Monitor or any UART terminal

## Installation and Usage

1. **Hardware Setup:**
   - Connect the DS1307 or DS3231 RTC module to the Arduino board using the I2C interface.

2. **Software Setup:**
   - Clone or download this repository.
   - Open the Arduino IDE and load the main sketch file (e.g., `RTc_Clock_Custom_lib_Arduino_Env.ino`).

3. **Upload the Sketch:**
   - Select the correct board and COM port from the Arduino IDE.
   - Upload the sketch to your Arduino board.

4. **Monitor Output:**
   - Open the Serial Monitor in the Arduino IDE (set baud rate to 9600).
   - You should see the current time displayed in the format: `HH:MM:SS`.

## Custom Libraries

- **UART Library**: Provides tailored functions for serial communication, enhancing reliability and performance.
- **I2C Library**: Optimized for interfacing with DS1307 and DS3231 RTC modules, ensuring accurate timekeeping.

## Contributing

Contributions are welcome! For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.
