/* Overview
    This is a program for a B737 FMC (Flight Management Computer) / CDU (Control Display Unit).  The
    processor used in this design is a Teensy++ 2.0 due to the Arduino code compatibility and the number
    of I/O pins needed for this design.

    There a several features that this design has including:
        - Circuity board designed to work with the Hispapanel B737 FMC bezel and buttons
        - Designed for the 5" LCD ZJ050NA-08C
        - Supports to Drive boards
            - KYV-N5-V1     (Preferred due to on board power, VGA & HDMI connectors)
            - GDN-D567AT    (Secondary due to power and VGA connectors on separate cables)
        - Support for multiple Flight Simiulator Avionics Suites - selectable via on board DIP switches
            - AeroSoft (only pilot supported)
            - PMDG (selectable Pilot and Copilot)
            - ProSim (selectable Pilot and Copilot)
            - Diagnostics mode (Displays all values for each keys)
        - Open source code with ability to modify values of keys for other / Future Avionics Suites
        - Works with .lau scripts documented in comments at the end of this program
        - Ability to control the 5" LCD settings using the Rotary Encoder
        - All 69 pushbuttons are backlighted & brightness controlled by the Rotary Encoder
        - Audibale "click" for keypad keys (selectable)
        - EXEC LED pluggable into the circuit board
        - All components silkscreen labeled with values and polarity
        - Switches silkscreen with names of keys to help when populating plastic keycaps
        - On board +12V connector to be used with "Y" cable to power the LEDs and the LCD Drive Board
 */
 /*
 To be used with the design at this location: https://easyeda.com/SteveDobler/fmc_hispapanel

                                    [ CDU physical layout of keypad ]
          _______________________________________________________________________________________
         |                                                                                       |
         |            _______________________________________________________________            |
         |           |                                                               |           |
         |  [LSK-1]  |                                                               |  [RSK-1]  |
         |  [SW01]   |                                                               |  [SW07]   |
         |           |                                                               |           |
         |  [LSK-2]  |                                                               |  [RSK-2]  |
         |  [SW02]   |                                                               |  [SW08]   |
         |           |                                                               |           |
         |  [LSK-3]  |                                                               |  [RKS-3]  |
         |  [SW03]   |                                                               |  [SW09]   |
         |           |                                                               |           |
         |  [LSK-4]  |                                                               |  [RSK-4]  |
         |  [SW04]   |                                                               |  [SW10]   |
         |           |                                                               |           |
         |  [LSK-5]  |                                                               |  [RSK-5]  |
         |  [SW05]   |                                                               |  [SW11]   |
         |           |                                                               |           |
         |  [LSK-6]  |                                                               |  [RSK-6]  |
         |  [SW06]   |_______________________________________________________________|  [SW12]   |
         |                                                                                       |
         |                                                                     [Encoder]         |
         |                                                                     [   O   ]         |
         |                                                                     [  SW70 ]         |
         |          [INIT  REF]  [   RTE   ]  [  CLB  ]  [  CRZ  ]  [  DES  ]                    |
         |          [   SW13  ]  [  SW14   ]  [  SW15 ]  [  SW16 ]  [  SW17 ]     LED            |
         |                                                                        (1)            |
         |          [  MENU   ]  [  LEGS   ]  [DEP ARR]  [  HOLD ]  [  PROG ]   [ EXEC ]         |
         |          [  SW18  ]   [  SW19   ]  [  SW20 ]  [  SW21 ]  [  SW22 ]   [ SW23 ]         |
         |                                                                                       |
         |          [N1  LIMIT]  [   FIX   ]  [ A  ]   [ B  ]   [ C  ]   [ D  ]  [ E  ]          |
         |          [   SW24  ]  [  SW25   ]  [SW28]   [SW29]   [SW30]   [SW31]  [SW32]          |
         |                                                                                       |
         |          [PREV PAGE]  [NEXT PAGE]  [ F  ]   [ G  ]   [ H  ]   [ I  ]  [ J  ]          |
         |     |C|  [  SW26  ]   [  SW27   ]  [SW33]   [SW34]   [SW35]   [SW36]  [SW37]          |
         | LED |A|                                                                       |M| LED |
         | (3) |L|  [ 1  ]   [ 2  ]   [ 3  ]  [ K  ]   [ L  ]   [ M  ]   [ N  ]  [ O  ]  |S| (5) |
         |     |L|  [SW67]   [SW68]   [SW69]  [SW38]   [SW39]   [SW40]   [SW41]  [SW42]  |G|     |
         |                                                                                       |
         |     |F|  [ 4  ]   [  5 ]   [ 6  ]  [ P  ]   [ Q ]    [ R  ]   [ S  ]  [ T  ]  |O|     |
         | LED |A|  [SW64]   [SW65]   [SW66]  [SW43]   [SW44]   [SW45]   [SW46]  [SW47]  |F| LED |
         | (4) |I|                                                                       |S| (6) |
         |     |L|  [ 7  ]   [  8 ]   [ 9  ]  [ U  ]   [ V  ]   [ W  ]   [ X  ]  [ Y  ]  |T|     |
         |          [SW61]   [SW62]   [SW63]  [SW48]   [SW49]   [SW50]   [SW51]  [SW52]          |
         |                                                                                       |
         |          [ .  ]   [  0 ]   [+/- ]  [ Z  ]   [ SP ]   [DEL ]   [ /  ]  [CLR ]          |
         |          [SW58]   [SW59]   [SW60]  [SW53]   [SW54]   [SW55]   [SW56]  [SW57]          |
         |_______________________________________________________________________________________|

 keypad.h library keypad array layouT. These are the array's rows and columns, not the keypad's physical layout

       { (R,C)...................................(R,C)} R=Row, C=Column
       { (1,1)(1,2)(1,3)(1,4)(1,5)(1,6)(1,7)(1,8)(1,9)}
       { (2,1)...................................(2,9)}
       { (3,1)...................................(3,9)}
       { (4,1)...................................(4,9)}
       { (5,1)...................................(5,9)}
       { (6,1)...................................(6,9)}
       { (7,1)...................................(7,9)}
       { (8,1)(8,2)(8,3)(8,4)(8,5)(8,6)(8,7)(8,8)(8,9)}
 */

 /*
   A Teensy++ 2.0 module is used for this project. The view below is the top of the Teensy ++2.0 module.
   The numbers inside the dashed box below are the physical module socket pin numbers. The numbers just
   outsided the dashed box are the Teensy programming reference numbers assigned to the pins

                           ____________________________
                          |         |  USB  |          |
                      GND | 1       |       |       40 | +5V
             PWM  PB7  27 | 2       |_______|       39 | 26  PB6  PWM
        PWM INT0  PD0   0 | 3                       38 | 25  PB5  PWM
        PWM INT1  PD1   1 | 4                       37 | 24  PB4  PWM
         RX INT2  PD2   2 | 5                       36 | 23  PB3
         TX INT3  PD3   3 | 6                       35 | 22  PB2
                  PD4   4 | 7                       34 | 21  PB1
                  PD5   5 | 8     Teensy++ 2.0      33 | 20  PB0
        (LED PIN) PD6   6 | 9                       32 | 19  PE7
                  PD7   7 | 10                      31 | 18  PE6
                  PE0   8 | 11                      30 | GND
                  PE1   9 | 12                      29 | AREF
                  PC0  10 | 13                      28 | 38  PF0  A0
                  PC1  11 | 14  PA4 32 O  O 28 PA0  27 | 39  PF1  A1
                  PC2  12 | 15  PA5 33 O  O 28 PA1  26 | 40  PF2  A2
                  PC3  13 | 16  PA6 34 O  O 28 PA2  25 | 41  PF3  A3
             PWM  PC4  14 | 17  PA7 35 O  O 28 PA3  24 | 42  PF4  A4
             PWM  PC5  14 | 18                      23 | 43  PF5  A5
             PWM  PC6  15 | 19                      22 | 44  PF6  A6
                  PC7  16 | 20       O  O  O        21 | 45  PF7  A7
                          | __________________________ |
                                     +  G  R
                                     5  N  S
                                     V  D  T
 */

 /*
   This view shows the schematic signal names for each pin on the Teensy++ 2.0 Module.  N/C = No Connection

                           ____________________________
                          |         |  USB  |          |
                      GND | 1       |       |       40 | +5V
                       R2 | 2       |_______|       39 | DIM_CTRL
                       R3 | 3                       38 | LCD_PLUS
                       R4 | 4                       37 | C1
                       R5 | 5                       36 | C2
                       R6 | 6                       35 | C3
                       R7 | 7                       34 | LCD_PWR
                       R8 | 8     Teensy++ 2.0      33 | C4
                       R1 | 9                       32 | C5
                 LCD_AUTO | 10                      31 | C6
                 LCD_MENU | 11                      30 | GND
                 LCD_PLUS | 12                      29 | N/C
                       SW | 13                      28 | C7
                     ROTA | 14                      27 | C8
                     ROTB | 15                      26 | DS_1
                  LED_MSG | 16                      25 | DS_2
                 LED_EXEC | 17                      24 | DS_3
                 LED_CALL | 18                      23 | C9
                 LED_FAIL | 19                      22 | BUZZER
                 LED_OFST | 20                      21 | N/C
                          | __________________________ |

 */

 /*

 Programming the LCD Display

     This FMC has the capability of adjusting settings on the LCD display using the encoder
     push button and clockwise and counter clockwise rotation of the encoder.

     To enter the LCD settings mode, press and hold the encoder pushbutton for about 3 seconds.
     After 3 seconds the all the LEDs will start to flash indicating that you are in the LCD
     settings mode.  In addition, the LCD settings menu will be displayed.  Once this menu
     is displayed you can use the encoder to mimics the physical pressing of the LCD disply
     controller board buttons.  Below are the equivalent of those buttons.

     Note: If none of the below actions are taken the flashing LEDs will time out and the
     unit will go back into the FMC mode.

         Menu     Press encoder push button
         (+) 	 Encoder rotate right
         (-)  	 Encoder rotate left
         S/Auto 	 Press the encoder push button twice quickly
         Power    Once in the LCD setting mode press and hold
 */

