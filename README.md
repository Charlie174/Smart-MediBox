# Wokwi project

This is a Wokwi project. Please edit this README file and add a description of your project.

## Usage

1. Add parts by clicking the blue "+" button in the diagram editor
2. Connect parts by dragging wires between them
3. Click the green play button to start the simulation

# Smart MediBox

## Overview
This project involves the development of a Smart MediBox using an ESP32 microcontroller. The Smart MediBox is designed to provide medication reminders, monitor temperature and humidity, and adjust shading based on light intensity. The system also integrates with MQTT for communication with external services and devices.

## Features
- Medication reminder with buzzer alert
- Temperature and humidity monitoring using DHT sensor
- Light intensity detection with shading adjustment
- MQTT integration for remote control and monitoring
- Schedule-based medication reminders

## Components Used
- ESP32 microcontroller
- DHT22 temperature and humidity sensor
- LDRs (Light Dependent Resistors) for light intensity detection
- Servo motor for shading adjustment
- Buzzer for medication reminders

## Setup Instructions
1. **Hardware Setup**: Connect the components as per the wiring diagram provided.
2. **Software Setup**:
   - Install the necessary libraries using Arduino Library Manager.
   - Update the Wi-Fi credentials in the code to connect to your Wi-Fi network.
   - Configure MQTT broker details in the code.
3. **Upload Code**: Upload the code to the ESP32 board using the Arduino IDE or PlatformIO.
4. **Accessing Data**: Access temperature, humidity, and other data via MQTT or serial monitor.

## Usage
1. **Medication Reminder**: The system provides medication reminders with buzzer alerts based on predefined schedules.
2. **Temperature and Humidity Monitoring**: Monitor temperature and humidity levels in real-time.
3. **Light Intensity Detection**: The system adjusts shading based on light intensity detected by LDRs.
4. **Remote Control**: Control the system remotely via MQTT commands to enable/disable medication reminders and adjust settings.

## Folder Structure
- **src/**: Contains the main Arduino sketch file (`main.ino`) and any additional source files.
- **libraries/**: Includes external libraries used in the project.

## Troubleshooting
- If encountering issues with sensor readings or MQTT communication, check the wiring connections and network configurations.
- Ensure that the libraries used are compatible with the ESP32 board and version.

## Contributing
Contributions to this project are welcome! Feel free to fork the repository, make improvements, and submit pull requests.

## License
This project is licensed under the [MIT License](LICENSE).

## Author
Charlie174 - jpcharlie02@gmail.com


