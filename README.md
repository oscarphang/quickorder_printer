# quickorder_printer

This repository contains the code for an Arduino printer that receives print orders from an MQTT server and prints the details of the orders on a thermal printer.

## Dependencies

* Arduino IDE
* Adafruit Thermal Printer Library
* PubSubClient Library

## Setup

* Clone this repository to your local machine.
* Open the quickorder.ino file in the Arduino IDE.
* Make sure you have installed the required libraries mentioned above.
* Connect your Arduino board to your computer via USB.
* Select the correct board and port in the Arduino IDE.
* Upload the code to your Arduino board.
* Connect the thermal printer to your Arduino board according to the wiring diagram provided in the code comments.
* Connect your Arduino board to an MQTT server that publishes print orders with the following details:
  + Item name
  + Quantity
  + Order time
* Once your Arduino board is connected to the MQTT server, it will receive print orders and print the details on the thermal printer.
Configuration

In the config.h file, you will find the following configuration settings:

* mqtt_user: Your MQTT Host Username.
* mqtt_pass: Your MQTT Host password.
* mqtt_server: The IP address or hostname of the MQTT server.
* mqtt_port: The port number of the MQTT server.
* mqtt_listen_topic_text2print: The MQTT topic to subscribe to for print orders.
You will need to update these settings with your own values before uploading the code to your Arduino board.

## Usage

* At the first boot up, the board will act as a hotspot to allow you to choose which wifi to connect and prompt for the credential to connect, we need to choose the wifi without 5g to connect to the wifi smoothly.
* Once you have uploaded the code to your Arduino board and connected it to the thermal printer and MQTT server, the printer will automatically start printing print orders as they are received from the server.

## Contributing

If you find any bugs or issues with this code, feel free to create a pull request or issue. Contributions are always welcome!

## License

This code is released under the MIT License. See the LICENSE file for more information.
