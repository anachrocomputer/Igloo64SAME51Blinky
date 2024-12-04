# Igloo64SAME51Blinky #

Firmware for Atmel/Microchip ATSAME51J20A microcontroller (ARM) on Igloo64 prototyping board.
The board has nine big LEDs, and this code blinks them.

## Connections ##

| LED | SAM E51 pin | Name |
|-----|-------------|------|
| D1  |     1       | PA00 |
| D2  |     60      | PB31 |
| D3  |     59      | PB30 |
| D4  |     51      | PA27 |
| D5  |     50      | PB23 |
| D6  |     46      | PA25 |
| D7  |     41      | PA20 |
| D8  |     37      | PA18 |
| D9  |     32      | PA15 |

GPIO pin assignments are by physical proximity on the PCB.

## Compiling ##

Code written in MPLAB X V6.05 and compiled with 'xc32' compiler V4.30.


