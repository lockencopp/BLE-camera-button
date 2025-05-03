# BLE Shutter Button

A project to create a Bluetooth Low Energy (BLE) shutter button for controlling devices like cameras or smartphones.

## Features

- BLE connectivity for wireless control.
- Compact and portable design.
- Customizable button actions.

## Structure
```
BLE-Shutter-Button/
├── .gitignore
├── platformio.ini
├── README.md
├── .pio/
├── .vscode/
├── include/
├── lib/
├── src/
│   └── BLE-Shutter-Button.cpp
└── test/
```

## Requirements

- [PlatformIO](https://platformio.org/) installed.
- Compatible microcontroller with BLE support (e.g., ESP32).
- Necessary hardware components (e.g., button, battery).

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
2. Pair it with your smartphone or camera via BLE.
3. Press the button to trigger the shutter.

## License

This project is licensed under the [MIT License](LICENSE).

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests.

## Acknowledgments

- Thanks to the open-source community for tools and libraries.
- Inspired by similar BLE projects.
