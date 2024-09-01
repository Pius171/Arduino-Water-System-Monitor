# Arduino-Water-System-Monitor
An esp32-based water monitoring system using the Arduino core. Monitors 

- Water level in a reservoir using an ultrasonic sensor
- The state of the two pumps
- logs data to SD card

  The state of the pumps and the volume of water in the reservoir are uploaded to Arduino cloud for remote monitoring.


![image](https://github.com/user-attachments/assets/a33d51c3-e224-459a-b4af-13c3bffb02ab)

Note: This circuit is designed to monitor HIGH voltage pumps

## Detecting when motor trips
By trip I mean the motor control circuit for the motor detected a fault; as a result, cuts off power to the motor. This is detected by contacts K7 of motor 1 thermal overload relay  and K5 for motor 2 thermal overload relay.

## Detecting when the motor is running or not
This is similar to the way a trip is detected, but instead, we use the normally open contacts of the contactor. They are represented by the contacts K7 and K8 in the schematic diagram. 

To read more about how I built this project, click this link: https://techmasterevent.com/project/arduino-water-plant-monitoring-system-using-arduino-iot-cloud
