# BLE Shutter Button

A project to create a Bluetooth Low Energy (BLE) shutter button for controlling devices like cameras or smartphones.

## Features

- BLE connectivity for wireless control.
- Compact and portable design.
- Customizable button actions (focus, shutter, and hold).
- Modular code structure with separate files for button handling and BLE logic.

## Structure
```
BLE-Shutter-Button/
├── .gitignore
├── platformio.ini
├── README.md
├── .pio/
├── .vscode/
├── include/
│   └── button_handling.h
├── lib/
├── src/
│   └── BLE-Shutter-Button.cpp
│   └── button_handling.cpp
└── test/
```

## Requirements

- [PlatformIO](https://platformio.org/) installed.
- Compatible microcontroller with BLE support (e.g., ESP32).
- BT1818 Bluetooth remote

## Installation

1. Clone this repository:
    ```bash
    git clone https://github.com/yourusername/BLE-Shutter-Button.git
    ```
2. Open the project in PlatformIO.
3. Configure the `platformio.ini` file as needed.
4. Build and upload the firmware to your microcontroller.

## Usage

1. Power on the device.
2. Pair BT1818 with ESP32 via BLE.
3. Press the button to trigger the following actions:
   - **press iOS Button (Short Press, <50ms)**: Focus action.
   - **press Android Button (Medium Press, 50ms–350ms)**: Shutter action.
   - **hold Android Button (Long Press, >350ms)**: Hold Shutter action.

## Code Highlights

- **Button Handling**:
  - Modularized in `button_handling.h` and `button_handling.cpp`.
  - Handles focus, shutter, and hold shutter actions based on button press duration.
  - Includes helper functions for setting and clearing output pins.

- **BLE Logic**:
  - Scans for a target BLE device (`BT1818`) and connects automatically.
  - Handles BLE notifications and processes button states.
  - Includes retry logic for BLE scanning if the target device is not found.

## License

This project is licensed under the [MIT License](LICENSE).

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

## Acknowledgments

- Thanks to the open-source community for tools and libraries.
- Inspired by similar BLE projects.
