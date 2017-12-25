# Reference Repositories
https://github.com/sparkfun/Fingerprint_Scanner-TTL

https://github.com/marc-pelland/fingerprint_gt511c3

# GT-511C3
Fingerprint Scanner Sensor module TTL GT-511C3 Arduino code. Compatible with all arduino boards. 

# Connection
For a reliable connection if you are using a 5V Arduino microcontroller:

   Fingerprint Scanner (Pin #) <-> Logic Level Converter <-> 5V Arduino w/ Atmega328P
     UART_TX (3.3V TTL)(Pin 1) <->     LV1 <-> HV1       <->  RX (pin 8)
     UART_RX (3.3V TTL)(Pin 2) <->     LV4 <-> HV4       <->  TX (pin 9)
           GND         (Pin 3) <->     GND <-> GND       <->     GND
      Vin (3.3V~6V)    (Pin 4) <->        HV             <->      5V
                                          LV             <->     3.3V
                                          
# Or use Voltage Division w/ 3x 10kOhm Resistors
Otherwise, you could use 3x 10kOhm resistors to divide the voltage from a 5V Arduino down to 3.3V FPS similar to the
"Uni-Directional" application circuit on our old logic level converter
[ https://cdn.sparkfun.com/assets/b/0/e/1/0/522637c6757b7f2b228b4568.png ]:

    Voltage Divider         <-> Fingerprint Scanner(Pin #) <-> Voltage Divider <-> 5V Arduino w/ Atmega328P
                            <-> UART_TX (3.3V TTL) (Pin 1) <->                 <->       RX (pin 4)
  GND <-> 10kOhm <-> 10kOhm <-> UART_RX (3.3V TTL) (Pin 2) <->      10kOhm     <->       TX (pin 5)
          GND               <->        GND         (Pin 3) <->       GND       <->        GND
                            <->    Vin (3.3V~6V)   (Pin 4) <->                 <->        5V
			    
Note: You can add the two 10kOhm resistors in series for 20kOhms.
