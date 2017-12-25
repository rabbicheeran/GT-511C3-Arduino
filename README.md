# GT-511C3
Fingerprint Scanner Sensor module TTL GT-511C3 Arduino code without using library. Compatible with most of all arduino boards. 


# Reference Repositories
https://github.com/sparkfun/Fingerprint_Scanner-TTL

https://github.com/marc-pelland/fingerprint_gt511c3

# Connection
For a reliable connection if you are using a 5V Arduino microcontroller:

   Fingerprint Scanner (Pin #) <-> Logic Level Converter <-> 5V Arduino w/ Atmega328P
   
     UART_TX (3.3V TTL)(Pin 1) <->     LV1 <-> HV1       <->  RX (pin 8)
     
     UART_RX (3.3V TTL)(Pin 2) <->     LV4 <-> HV4       <->  TX (pin 9)
     
           GND         (Pin 3) <->     GND <-> GND       <->     GND
           
      Vin (3.3V~6V)    (Pin 4) <->        HV             <->      5V
      
                                          LV             <->     3.3V
                                          
# Or use Voltage Division using 680 Ohm and 1.3 kOhm Resistors
When connecting to a microcontroller that uses 5V voltage levels on its pins, a level
converter must be used to reduce the 5V output from the microcontroller to the FPS module
because the FPS module can only handle 3.3V on its UART pins.

A voltage divider consisting of two resistors can be used as a level converter to reduce the
5V incoming signal to 3.3V. The circuit diagram below shows the fingerprint scanner module
connected to an Arduino Uno.

![alt text](https://startingelectronics.org/articles/GT-511C3-fingerprint-scanner-hardware/arduino-UNO-GT-511C3-interface.png)

The circuit below shows the voltage divider from the above circuit diagram. When the Arduino
drives its output pin to 5V, the voltage divider reduces this level to 3.3V so that the
fingerprint module is not over-driven.

![alt text](https://startingelectronics.org/articles/GT-511C3-fingerprint-scanner-hardware/voltage-divider.png)

