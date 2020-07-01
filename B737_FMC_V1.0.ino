/*
  Name:       Teensy_FMC_R11.ino
  Created:    4/26/20
  Author:     Steven Dobler
*/


//--------- [Functions Descriptions] ----------- [Functions Descriptions] ----------- [Functions Descriptions] -------//

void lcdPowerButton();  // Turns off the LCD display's power

void lcdMenuButton();   // Mimics pressing the LCD Menu button

void lcdPlusButton();   // Mimics pressing the LCD (+) button

void lcdMinusButton();  // Mimics pressing the LCD (-) button

void lcdSAutoButton();  // Mimics pressing the LCD S/Auto button

void keypadClick();     // Generates a "click" each time a keypad button is pressed.  Also used when LCD mode times out

void checkEncoder();    // Used to check the rotary encoder to control the backlight tactile button's LED brightness
   
void ledsOff();         // Used to turn off the MSG, EXEC, CALL, FAIL and OFST LEDs     

void ledBlink();        // Used to flash all the LEDs indicating the LCD Setting mode 

void lcdModeTimeOut();  // Once in the LCD Setting mode, this is used to time out that mode if there is no activity

void lessThan();        /* lessThan()   When a < character is present in the string read from the serial it 
                           indicates that the next character is to be used to turn off a particular LED. 
                           After the < character the next character defines the LED as follows:
                                 M = MSG  LED
                                 E = EXEC LED
                                 C = CALL LED
                                 F = FAIL LED
                                 O = OFST LED
                                 Z = All LEDs On
                        */
  
void moreThan();        /* When a > character is present in the string read from the serial it indicates that
                           the next character is to be used to turn off a particular LED. After the > character
                           the next character defines the LED as follows:
                                M = MSG  LED
                                E = EXEC LED
                                C = CALL LED
                                F = FAIL LED
                                O = OFST LED
                                X = All LEDs Off
                        */  

//----- [Libraries] ------- [Libraries] ------- [Libraries] ------- [Libraries] ------- [Libraries] -----//
  
    #include <Keypad.h>     // used for the FMC keypad matrix
  
    #include "Countimer.h"              // timer library used for the LCD timeout
    Countimer lcdTimer;                 // Instance of timer used for LCD timeout

    #include <NoDelay.h>                // NoDelay Library https://github.com/M-tech-Creations/NoDelay
                                        // Used for detecting encoder push button, tap, double tap and hold events
    noDelay LEDtime(200, ledBlink);    // Creates a noDelay varible set to 1000ms, will call ledBlink function
    
 
/*
  ButtonEvents - An Arduino library for catching tap, double-tap and press-and-hold events for buttons.

        Written by Edward Wright (fasteddy@thewrightspace.net)
        Available at https://github.com/fasteddy516/ButtonEvents

        Utilizes the Bounce2 library by Thomas O. Fredericks
        Available at https://github.com/thomasfredericks/Bounce2
 */
    #include <ButtonEvents.h>  // Library for the encoder push button events (tap, double tap and hold)


//--- [Variable Assignments] --- [Variable Assignments] --- [Variable Assignments] --- [Variable Assignments] ----//
    int ledState = LOW;
    int fmcState = HIGH; // start up mode is the normal FMC


// Variables used to blink the MSG, EXEC, CALL, FAIL and OFST LEDs to show the FMC is in LCD control state
    long BLINK_INTERVAL = 200;              // Interval at which to blink LED (milliseconds)
    unsigned long LEDpreviousMillis = 0;    // Will store last time LED was updated
    unsigned long LEDCurrentMillis = 0;
    int ledFlag = 0;
   
// Define the number or rows and columns in the keypad array
    const   byte ROWS = 8;  // The CDU Keypad has 8 ROW Wires
    const   byte COLS = 9;  // The CDU Keypad has 9 COL Wires


// CodeIn variable hold the number from the keypad when a button is pressed
    char  CodeIn;

    int   OFF = 1;          // For the 5 FMC LEDs
    int   ON = 0;           // For the 5 FMC LEDs 

    int Encoder_Switch_State;

//------- [Button Instance] -------- [Button Instance] --------- [Button Instance] --------- [Button Instance] --------//

/* Create an instance of the ButtonEvents class for the encoder pushbutton.  The encoder pushbutton
   is used to control the LCD panel by emulating the phycial pushbuttons that are on the small
   circuit board that comes with the LCD driver board.  The phycial buttons are: PWR, MENU, +, -, and 
   S/AUTO.
*/
    ButtonEvents encoderButton;    

//-------- [Keypad ROWS & COL pin Assignments] ------- [Keypad ROWS & COL pin Assignments] ----------//

// Teensy Programming ROW Number:         R1, R2, R3, R4, R5, R6, R7, R8
// Teensy Programming ROW Pin Names:      D6, B7, D0, D1, D2, D3, D4, D5
// Teensy Socket ROW Pin Names:           9,  2,  3,  4,  5,  6,  7,  8
                byte rowPins[ROWS] =    { 6,  27, 0,  1,  2,  3,  4,  5 };      // Teensy pin programming numbers

// Teensy Programming COL Number:         C1, C2, C3, C4, C5, C6, C7, C8, C9
// Teensy Programming COL Pin Names:      B4, B3, B2, B0, E7, E6, F0, F1, F5    
// Teensy Socket COL Number:              37, 36, 35, 33, 32, 31, 28, 27, 23
                byte colPins[COLS] =    { 24, 23, 22, 20, 19, 18, 38, 39, 43 }; // Teensy pin programming numbers

                
// Using the keypad.h library the FMC switch matrix is populated from 1 to 69 and includes all keypad buttons

byte keys[ROWS][COLS] = {

    /*               COL1      COL2       COL3       COL4       COL5     COL6     COL7     COL8     COL9
                      |         |          |           |         |        |        |        |        |
         Switch-> [ SW01  ] [ SW09   ] [  SW17  ] [  SW25   ] [ SW33 ] [ SW41 ] [ SW49 ] [ SW57 ] [ SW65 ]
    ROW1 Name  -> [ LSK-1 ] [ RSK-3  ] [   DES  ] [   FIX   ] [   F  ] [  N   ] [  V   ] [ CLR  ] [   5  ] Row 0 */
                  {   1,        2,         3,         4,         5,       6,       7,      8,         9 }, // Keypad Row 1

    /*   Switch-> [ SW02  ] [ SW10   ] [  SW18  ] [  SW26   ] [ SW34 ] [ SW42 ] [ SW50 ] [ SW58 ] [ SW66 ]
    ROW2 Name  -> [ LSK-2 ] [ RSK-4  ] [  MENU  ] [PREV PAGE] [   G  ] [  O   ] [  W   ] [  .   ] [  6   ] Row 1 */
                  {   10,      11,         12,        13,         14,     15,      16,      17,      18  }, // Keypad Row 2

    /*   Switch-> [ SW03  ] [ SW11   ] [  SW19  ] [  SW27   ] [ SW35 ] [ SW43 ] [ SW51 ] [ SW59 ] [ SW67 ]
    ROW3 Name  -> [ LSK-3 ] [ RSK-5  ] [  LEGS  ] [NEXT PAGE] [   H  ] [  P   ] [  X   ] [  0   ] [  1   ] Row 2  */
                  {   19,      20,         21,        22,        23,      24,      25,      26,      27 }, // Keypad Row 3

    /*   Switch-> [ SW04  ] [ SW12   ] [  SW20  ] [  SW28   ] [ SW36 ] [ SW44 ] [ SW52 ] [ SW60 ] [ SW68 ]
    ROW4 Name  -> [ LSK-4 ] [ RSK-6  ] [DEP ARR ] [    A    ] [   I  ] [  Q   ] [  Y   ] [ +/-  ] [  2   ] Row 3  */
                  {   28,      29,        30,         31,        32,      33,      34,     35,       36  }, // Keypad Row 4

    /*   Switch-> [ SW05  ] [ SW13   ] [  SW21  ] [  SW29   ] [ SW37 ] [ SW45 ] [ SW53 ] [ SW61 ] [ SW69 ]
    ROW5 Name  -> [ LSK-5 ] [INIT REF] [  HOLD  ] [    B    ] [  J   ] [  R   ] [  Z   ] [  7   ] [  3   ] Row 4  */
                  {   37,      38,         39,         40,       41,      42,      43,      44,      45  }, // Keypad Row 5

    /*   Switch-> [ SW06  ] [ SW14   ] [  SW22  ] [  SW30   ] [ SW38 ] [ SW46 ] [ SW54 ] [ SW62 ] [ **** ]
    ROW6 Name  -> [ LSK-6 ] [  RTE   ] [  PROG  ] [    C    ] [  K   ] [  S   ] [  SP  ] [  8   ] [ **** ] Row 5  */
                  {   46,      47,         48,         49,       50,      51,      52,      53,      54 }, // Keypad Row 6

    /*   Switch-> [ SW07  ] [ SW15   ] [  SW23  ] [  SW31   ] [ SW39 ] [ SW47 ] [ SW55 ] [ SW63 ] [ **** ]
    ROW7 Name  -> [ RSK-1 ] [  CLB   ] [  EXEC  ] [    D    ] [  L   ] [  T   ] [ DEL  ] [  9   ] [ **** ] Row 6  */
                  {   55,      56,        57,          58,       59,       60,      61,     62,       63 }, // Keypad Row 7

    /*   Switch-> [ SW08  ] [ SW16   ] [  SW24  ] [  SW32   ] [ SW40 ] [ SW48 ] [ SW56 ] [ SW64 ] [ **** ]
    ROW8 Name  -> [ RSK-2 ] [  CRZ   ] [N1 LIMIT] [    E    ] [  M   ] [  U   ] [  /   ] [  4   ] [ **** ] Row 7  */
                  {   64,      65,        66,          67,       68,      69,      70,      71,      72 }  // Keypad Row 8

}; // End of keypad matrix / array

/* The  designation [ **** ] means there is no key switch in that position, however, 222 (arbitrary number) is placed in the
matrix as a placeholder.  The keypad.h library doesn't like zeros in the matrix.
*/
    

String KeyName[] = {

    /*               COL1      COL2       COL3       COL4       COL5     COL6     COL7     COL8     COL9
                      |         |          |           |         |        |        |        |        |
         Switch-> [ SW01  ]   [ SW09  ]  [  SW17 ] [  SW25   ] [ SW33 ] [ SW41 ] [ SW49 ] [ SW57 ] [ SW65 ]
    ROW1 Name  -> [ LSK-1 ]   [ RSK-3 ]  [  DES  ] [   FIX   ] [   F  ] [  N   ] [  V   ] [ CLR  ] [   5  ] Row 0 */
                  "[LSK-1]",  "[RSK-3]",  "[DES]",    "[FIX]",  "[ F ]", "[ N ]", "[ V ]", "[CLR]",  "[ 5 ]" , // Populate Array Row 1

    /*   Switch-> [ SW02  ]   [ SW10   ]  [  SW18  ] [  SW26   ] [ SW34 ] [ SW42 ] [ SW50 ] [ SW58 ] [ SW66 ]
    ROW2 Name  -> [ LSK-2 ]   [ RSK-4  ]  [  MENU  ] [PREV PAGE] [   G  ] [  O   ] [  W   ] [  .   ] [  6   ] Row 1  */
                  "[LSK-2]",  "[RSK-4]",  "[MENU]", "[PREV PAGE]","[ G ]", "[ O ]", "[ W ]", "[ . ]",  "[ 6 ]", // Populate Array Row 2

    /*   Switch-> [ SW03  ]   [ SW11   ]  [  SW19  ] [  SW27   ] [ SW35 ] [ SW43 ] [ SW51 ] [ SW59 ] [ SW67 ]
    ROW3 Name  -> [ LSK-3 ]   [ RSK-5  ]  [  LEGS  ] [NEXT PAGE] [   H  ] [  P   ] [  X   ] [  0   ] [  1   ] Row 2  */
                  "[LSK-3]",  "[RSK-5]",  "[LEGS]", "[NEXT PAGE]","[ H ]","[ P ]", "[ X ]", "[ 0 ]",  "[ 1 ]", // Populate Array Row 3

    /*   Switch-> [ SW04  ]  [ SW12   ]   [  SW20  ]  [  SW28  ] [ SW36 ] [ SW44 ] [ SW52 ] [ SW60 ] [ SW68 ]
    ROW4 Name  -> [ LSK-4 ]  [ RSK-6  ]   [DEP ARR ]  [   A    ] [  I   ] [  Q   ] [  Y   ] [ +/-  ] [  2   ] Row 3  */
                 "[LSK-4]",  "[RSK-6]",  "[DEP ARR]",  "[ A ]",  "[ I ]", "[ Q ]", "[ Y ]", "[ +/- ]",  "[ 2 ]"  , // Populate Array Row 4

    /*   Switch-> [ SW05  ]   [  SW13  ]  [  SW21  ] [  SW29   ] [ SW37 ] [ SW45 ] [ SW53 ] [ SW61 ] [ SW69 ]
    ROW5 Name  -> [ LSK-5 ]   [INIT REF]  [  HOLD  ] [    B    ] [  J   ] [  R   ] [  Z   ] [  7   ] [  3   ] Row 4  */
                  "[LSK-5]", "[INIT REF]","[HOLD]",    "[ B ]",  "[ J ]", "[ R ]", "[ Z ]", "[ 7 ]", "[ 3 ]"  , // Populate Array Row 5

    /*   Switch-> [ SW06  ]  [  SW14  ]  [  SW22  ] [  SW30   ] [ SW38 ] [ SW46 ] [ SW54 ] [ SW62 ] [ **** ]
    ROW6 Name  -> [ LSK-6 ]  [  RTE   ]  [  PROG  ] [    C    ] [  K   ] [  S   ] [  SP  ] [  8   ] [ **** ] Row 5  */
                  "[LSK-6]",  "[RTE]",    "[PROG]",   "[ C ]",  "[ K ]", "[ S ]",  "[SP]", "[ 8 ]",  "[  ]", // Populate Array Row 6

    /*   Switch-> [ SW07  ]  [  SW15  ] [  SW23  ] [  SW31   ] [ SW39 ] [ SW47 ] [ SW55 ] [ SW63 ] [ **** ]
    ROW7 Name  -> [ RSK-1 ]  [  CLB   ] [  EXEC  ] [    D    ] [  L   ] [  T   ] [ DEL  ] [  9   ] [ **** ] Row 6  */
                  "[RSK-1]",  "[CLB]",   "[EXEC]",   "[ D ]",  "[ L ]", "[ T ]", "[DEL]", "[ 9 ]",  "[  ]" , // Populate Array Row 7

    /*   Switch-> [ SW08  ]  [ SW16   ] [  SW24  ] [  SW32   ] [ SW40 ] [ SW48 ] [ SW56 ] [ SW64 ] [ **** ]
    ROW8 Name  -> [ RSK-2 ]  [  CRZ   ] [N1 LIMIT] [    E    ] [  M   ] [  U   ] [  /   ] [  4   ] [ **** ] Row 7  */
                  "[RSK-2]", "[CRZ]",  "[N1 LIMIT]", "[ E ]",  "[ M ]", "[ U ]", "[ / ]", "[ 4 ]",  "[  ]"  // Populate Array Row 8
};


String AeroSoft[] = {

    /*               COL1      COL2       COL3       COL4       COL5     COL6     COL7     COL8     COL9
                      |         |          |           |         |        |        |        |        |
         Switch-> [ SW01  ] [ SW09   ] [  SW17  ] [  SW25   ] [ SW33 ] [ SW41 ] [ SW49 ] [ SW57 ] [ SW65 ]
    ROW1 Name  -> [ LSK-1 ] [ RSK-3  ] [   DES  ] [   FIX   ] [   F  ] [  N   ] [  V   ] [ CLR  ] [   5  ] Row 0  */
                     "1",      "9",       "204",     "211",     "70",     "78",   "86",   "101",    "53" , // Populate Array Row 1

    /*   Switch-> [ SW02  ] [ SW10   ] [  SW18  ] [  SW26   ] [ SW34 ] [ SW42 ] [ SW50 ] [ SW58 ] [ SW66 ]
    ROW2 Name  -> [ LSK-2 ] [ RSK-4  ] [  MENU  ] [PREV PAGE] [   G  ] [  O   ] [  W   ] [  .   ] [  6   ] Row 1  */
                     "2",     "10",       "205",     "212",     "71",    "79",    "87",    "46",    "54"  , // Populate Array Row 2

    /*   Switch-> [ SW03  ] [ SW11   ] [  SW19  ] [  SW27   ] [ SW35 ] [ SW43 ] [ SW51 ] [ SW59 ] [ SW67 ]
    ROW3 Name  -> [ LSK-3 ] [ RSK-5  ] [  LEGS  ] [NEXT PAGE] [   H  ] [  P   ] [  X   ] [  0   ] [  1   ] Row 2  */
                     "3",     "11",       "206",     "213",     "72",    "80",    "88",     "48",   "49"  , // Populate Array Row 3

    /*   Switch-> [ SW04  ] [ SW12   ] [  SW20  ] [  SW28   ] [ SW36 ] [ SW44 ] [ SW52 ] [ SW60 ] [ SW68 ]
    ROW4 Name  -> [ LSK-4 ] [ RSK-6  ] [DEP ARR ] [    A    ] [   I  ] [  Q   ] [  Y   ] [ +/-  ] [  2   ] Row 3  */
                     "4",     "12",      "207",      "65",      "73",    "81",    "89",   "100",    "50"  , // Populate Array Row 4

    /*   Switch-> [ SW05  ] [ SW13   ] [  SW21  ] [  SW29   ] [ SW37 ] [ SW45 ] [ SW53 ] [ SW61 ] [ SW69 ]
    ROW5 Name  -> [ LSK-5 ] [INIT REF] [  HOLD  ] [    B    ] [  J   ] [  R   ] [  Z   ] [  7   ] [  3   ] Row 4  */
                     "5",     "200",      "208",      "66",     "74",    "82",    "90",    "55",    "51"  , // Populate Array Row 5

    /*   Switch-> [ SW06  ] [ SW14   ] [  SW22  ] [  SW30   ] [ SW38 ] [ SW46 ] [ SW54 ] [ SW62 ] [ **** ]
    ROW6 Name  -> [ LSK-6 ] [  RTE   ] [  PROG  ] [    C    ] [  K   ] [  S   ] [  SP  ] [  8   ] [ **** ] Row 5  */
                     "6",     "201",      "209",      "67",     "75",    "83",    "32",    "56",    "222" , // Populate Array Row 6

    /*   Switch-> [ SW07  ] [ SW15   ] [  SW23  ] [  SW31   ] [ SW39 ] [ SW47 ] [ SW55 ] [ SW63 ] [ **** ]
    ROW7 Name  -> [ RSK-1 ] [  CLB   ] [  EXEC  ] [    D    ] [  L   ] [  T   ] [ DEL  ] [  9   ] [ **** ] Row 6  */
                     "7",     "202",     "214",       "68",     "76",    "84",   "102",    "57",    "222" , // Populate Array Row 7

    /*   Switch-> [ SW08  ] [ SW16   ] [  SW24  ] [  SW32   ] [ SW40 ] [ SW48 ] [ SW56 ] [ SW64 ] [ **** ]
    ROW8 Name  -> [ RSK-2 ] [  CRZ   ] [N1 LIMIT] [    E    ] [  M   ] [  U   ] [  /   ] [  4   ] [ **** ] Row 7  */
                     "8",     "203",     "210",       "69",     "77",    "85",    "47",    "52",    "222"  // Populate Array Row 8
};


String PMDG_LEFT[] = {

    /*               COL1      COL2       COL3       COL4       COL5     COL6     COL7     COL8     COL9
                      |         |          |           |         |        |        |        |        |
         Switch-> [ SW01  ] [ SW09   ] [  SW17  ] [  SW25   ] [ SW33 ] [ SW41 ] [ SW49 ] [ SW57 ] [ SW65 ]
    ROW1 Name  -> [ LSK-1 ] [ RSK-3  ] [   DES  ] [   FIX   ] [   F  ] [  N   ] [  V   ] [ CLR  ] [   5  ]  Row 0  */
                   "70166",  "70174",   "70181",   "70190",    "70210", "70218", "70226", "70234", "70197", // Populate Array Row 1

    /*   Switch-> [ SW02  ] [ SW10   ] [  SW18  ] [  SW26   ] [ SW34 ] [ SW42 ] [ SW50 ] [ SW58 ] [ SW66 ]
    ROW2 Name  -> [ LSK-2 ] [ RSK-4  ] [  MENU  ] [PREV PAGE] [   G  ] [  O   ] [  W   ] [  .   ] [  6   ]  Row 1  */
                   "70167",  "70175",    "70183",  "70192",    "70211", "70219", "70227", "70202", "70197", // Populate Array Row 2

    /*   Switch-> [ SW03  ] [ SW11   ] [  SW19  ] [  SW27   ] [ SW35 ] [ SW43 ] [ SW51 ] [ SW59 ] [ SW67 ]
    ROW3 Name  -> [ LSK-3 ] [ RSK-5  ] [  LEGS  ] [NEXT PAGE] [   H  ] [  P   ] [  X   ] [  0   ] [  1   ]  Row 2  */
                   "70168",  "70176",   "70184"    "70192",    "70212", "70220", "70228", "70203", "70193", // Populate Array Row 3

    /*   Switch-> [ SW04  ] [ SW12   ] [  SW20  ] [  SW28   ] [ SW36 ] [ SW44 ] [ SW52 ] [ SW60 ] [ SW68 ]
    ROW4 Name  -> [ LSK-4 ] [ RSK-6  ] [DEP ARR ] [    A    ] [   I  ] [  Q   ] [  Y   ] [ +/-  ] [  2   ]  Row 3  */
                   "70169",  "70177",   "70181",   "70205",    "70213", "70221", "70229", "70204", "70194", // Populate Array Row 4

    /*   Switch-> [ SW05  ] [ SW13   ] [  SW21  ] [  SW29   ] [ SW37 ] [ SW45 ] [ SW53 ] [ SW61 ] [ SW69 ]
    ROW5 Name  -> [ LSK-5 ] [INIT REF] [  HOLD  ] [    B    ] [  J   ] [  R   ] [  Z   ] [  7   ] [  3   ]  Row 4  */
                   "70170",  "70178",   "70186",   "70206",    "70214", "70222", "70230", "70199", "70195", // Populate Array Row 5

    /*   Switch-> [ SW06  ] [ SW14   ] [  SW22  ] [  SW30   ] [ SW38 ] [ SW46 ] [ SW54 ] [ SW62 ] [ **** ]
    ROW6 Name  -> [ LSK-6 ] [  RTE   ] [  PROG  ] [    C    ] [  K   ] [  S   ] [  SP  ] [  8   ] [ **** ]  Row 5  */
                   "70171",  "70178",   "70187",   "70207",    "70215", "70223", "70231", "70200",  "222",  // Populate Array Row 6

    /*   Switch-> [ SW07  ] [ SW15   ] [  SW23  ] [  SW31   ] [ SW39 ] [ SW47 ] [ SW55 ] [ SW63 ] [ **** ]
    ROW7 Name  -> [ RSK-1 ] [  CLB   ] [  EXEC  ] [    D    ] [  L   ] [  T   ] [ DEL  ] [  9   ] [ **** ]  Row 6  */
                   "70172",  "70180",   "70188",   "70208",    "70216", "70224", "70232", "70201",  "222",  // Populate Array Row 7

    /*   Switch-> [ SW08  ] [ SW16   ] [  SW24  ] [  SW32   ] [ SW40 ] [ SW48 ] [ SW56 ] [ SW64 ] [ **** ]
    ROW8 Name  -> [ RSK-2 ] [  CRZ   ] [N1 LIMIT] [    E    ] [  M   ] [  U   ] [  /   ] [  4   ] [ **** ]  Row 7  */
                   "70173",  "70181",   "70189",   "70209",    "70217", "70225", "70233", "70196",  "222"   // Populate Array Row 8
};



// Now that the row pins, column pins, number fo rows and number of columns have been defined, create the array
    Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//-------- [LED pin Assignments] ------- [LED pin Assignments] ------- [LED pin Assignments] ----------//

// Teensy MGS LED Programming Pin Name:     PC3
// Teensy MGS LED Socket Pin Number:        16
    const int LED_CDU_MSG = 13;

// Teensy EXEC LED Programming Pin Name:    PC4
// Teensy EXEC LED Socket Pin Number:       17
    const int LED_CDU_EXEC = 14;

// Teensy DSPY LED Programming Pin Name:    PC5
// Teensy DSPY LED Socket Pin Number:       18   
    const int LED_CDU_CALL = 15;

// Teensy FAIL LED Programming Pin Name:    PC6
// Teensy FAIL LED Socket Pin Number:       19
    const int LED_CDU_FAIL = 16;

// Teensy OFST LED Programming Pin Name:    PC7
// Teensy OFST LED Socket Pin Number:       20
    const int LED_CDU_OFST = 17;


//-------- [Keypad Dimmer Pin Assignment] -------- [Keypad Dimmer Pin Assignment]------- [Keypad Dimmer Pin Assignment]-------//

    // Teensy Backlight Programming Pin Name:   PB6
    // Teensy Backlight Socket Pin Number:       39
        const int BACK_LIGHT_PWM = 26;

//-------- [Encoder Pin Assignment] -------- [Encoder Pin Assignment]------- [Encoder Pin Assignment]-------//

    // Assign Constant names and Teensy Pin Numbers for the EC11E18244A5 Rotary Encoder used to dim the pushbutton LEDs
    // There are 3 pins on the one side of the encoder with names:  ROTA, GND, ROTB
    // Teensy Rotary Encoder Programming Pin Name:    ROTA
    // Teensy Rotary Encoder Socket Pin Number:       14
        const int EN_ROTB_Pin = 11;  // Analog input pin that the potentiometer is attached to

    // Teensy Rotary Encoder Programming Pin Name:    ROTB
    // Teensy Rotary Encoder Socket Pin Number:       15
        const int EN_ROTA_Pin = 12; // Analog output pin that the LED is attached to

    // Assign Constant names and Teensy Pin Numbers for the EC11E18244A5 Rotary Encoder's pushbutton switch
    // There are 2 pins on the other side of the encoder with names:  SW, GND
    // Teensy Rotary Encoder Programming Pin Name:    SW
    // Teensy Rotary Encoder Socket Pin Number:       13
        const int EN_SW_Pin = 10;


//-------- [DIP Switch Pin Assignment] -------- [DIP Switch Pin Assignment]------- [DIP Switch Pin Assignment]-------//

    // Teensy Code DIP Code Select Switch #1  Pin Name:     PF2
    // Teensy Code DIP Code Switch #1 Socket  Pin Number:   40
        const int CODE_SELECT_SW_1 = 40;

    // Teensy Code DIP Code Select Switch #1  Pin Name:     PF4
    // Teensy Code DIP Code Switch #2 Socket  Pin Number    42
        const int CODE_SELECT_SW_2 = 42;

    // Teensy Code DIP Code Select Switch #1  Pin Name:     PF3
    // Teensy Code DIP Code Switch #2 Socket  Pin Number    41
        const int CODE_SELECT_SW_3 = 41;


    // Teensy LCD Power Output Button to LCD Pin Name:      B1
    // Teensy LCD Power Output Button Socket  Pin Number:   21
        const int LCD_Power = 21;

    // Teensy LCD Menu Output Button to LCD Pin Name:       E0
    // Teensy LCD Menu Output Button Socket  Pin Number:    8
        const int LCD_Menu = 8;

    // Teensy LCD Plus Output Button to LCD Pin Name:       E1
    // Teensy LCD Plus Output Button Socket  Pin Number:    9
        const int LCD_Plus = 9;

    // Teensy LCD Minus Output Button to LCD Pin Name:      B5
    // Teensy LCD Minus Output Button Socket  Pin Number:   25
        const int LCD_Minus = 25;

    // Teensy LCD S/Auto Output Button to LCD Pin Name:     D7
    // Teensy LCD S/Auto Output Button Socket  Pin Number:  7
        const int LCD_S_AUTO = 7;

    // Teensy Piezo Buzzer for Keypad "click" Pin Name:     F6
    // Teensy Piezo Buzzer for Keypad "click" Pin Number:   44
        const int BUZZER = 44;
            
// Brightness of the backlighted pushbuttons can range from 0 to 255. Adjust as desired
        int brightness = 50;

// These variables are used to determine if the rotary encoder is turned clockwise or counterclockwise
        int encoderCurrentStateCLK;
        int encoderPreviousStateCLK;

// String variable to hold the encoder message clockwise "CW" or counterclockwise "CCW"
        String encdir = "";

// There are 4 DIP switches used to define various functions
        int DIP_SW_1;     // KeyCap Name 
        int DIP_SW_2;     // Keypad Number    
        int DIP_SW_3;     // Aerosoft FSUIPC Value
        int DIP_SW_4;     // Select FMC Pilot or Copilot


//----- [setup] ------- [setup] ------- [setup] ------- [setup] ------- [setup] ------- [setup]-----//

void setup()
{
    // Start serial Port 9600 baud rate
        Serial.begin(9600);

    // Configure the 5 LED pins as Output
        pinMode(LED_CDU_MSG,  OUTPUT);
        pinMode(LED_CDU_EXEC, OUTPUT);
        pinMode(LED_CDU_CALL, OUTPUT);
        pinMode(LED_CDU_FAIL, OUTPUT);
        pinMode(LED_CDU_OFST, OUTPUT);
   
    // Set the 5 LEDs to a known state (OFF)
        digitalWrite(LED_CDU_MSG,  OFF);
        digitalWrite(LED_CDU_EXEC, ON); // THIS GOES THROUGH A TRANSITOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
        digitalWrite(LED_CDU_CALL, OFF);
        digitalWrite(LED_CDU_FAIL, OFF);
        digitalWrite(LED_CDU_OFST, OFF);

    // Configure the 5 pins that control the LCD Drive Board as outputs
        pinMode(LCD_Power, OUTPUT);
        pinMode(LCD_Menu, OUTPUT);
        pinMode(LCD_Plus, OUTPUT);
        pinMode(LCD_Minus, OUTPUT);
        pinMode(LCD_S_AUTO, OUTPUT);

    // Set the 5 pins that control the LCD Drive Board (The base of 2N2222 transistors turning them off)
        digitalWrite(LCD_Power,  0);
        digitalWrite(LCD_Menu,   0);
        digitalWrite(LCD_Plus,   0);
        digitalWrite(LCD_Minus,  0);
        digitalWrite(LCD_S_AUTO, 0);

    // Configure the pin to the piezo buuzzer as output
        pinMode(BUZZER, OUTPUT);
       
    // Pins used for the 3 DIP switches used to select the code sent to the serial port when keys are pressed
        pinMode(CODE_SELECT_SW_1, INPUT_PULLUP);
        pinMode(CODE_SELECT_SW_2, INPUT_PULLUP);
        pinMode(CODE_SELECT_SW_3, INPUT_PULLUP);

    // Pins used for the Rotary Encoder pins
        pinMode(EN_ROTA_Pin, INPUT_PULLUP);
        pinMode(EN_ROTB_Pin, INPUT_PULLUP);

    // This is the pushbutton switch on the rotary encoder used ot turn all the LEDS ON or OFF
        pinMode(EN_SW_Pin, INPUT_PULLUP);

    // This is output pin used to control the brightness of the backlighted keypad pushbuttons
        pinMode(BACK_LIGHT_PWM, OUTPUT);
    
    // Read the value of the rotary encoder used late in the checkEncoder()
        encoderPreviousStateCLK = digitalRead(EN_ROTA_Pin);

    // Write to the pin that controls the backlight LEDs in each pushbutton
        analogWrite(BACK_LIGHT_PWM, brightness);

    // attach our ButtonEvents instance to the button pin
        encoderButton.attach(EN_SW_Pin);        

    // If your button is connected such that pressing it generates a high signal on the pin, you need to
    // specify that it is "active high"
       // encoderButton.activeHigh();  is this needed?

    // If your button is connected such that pressing it generates a low signal on the pin, you can specify
    // that it is "active low", or don't bother, since this is the default setting anyway.
        encoderButton.activeLow();

    // By default, the raw signal on the input pin has a 35ms debounce applied to it.  You can change the
    // debounce time if necessary.
        encoderButton.debounceTime(15); //apply 15ms debounce

    // The double-tap detection window is set to 150ms by default.  Decreasing this value will result in
    // more responsive single-tap events, but requires really fast tapping to trigger a double-tap event.
    // Increasing this value will allow slower taps to still trigger a double-tap event, but will make
    // single-tap events more laggy, and can cause taps that were meant to be separate to be treated as
    // double-taps.  The necessary timing really depends on your use case, but I have found 150ms to be a
    // reasonable starting point.  If you need to change the double-tap detection window, you can do so
    // as follows:

        encoderButton.doubleTapTime(250); // set double-tap detection window to 250ms

    // The hold duration can be increased to require longer holds before an event is triggered, or reduced to
    // have hold events trigger more quickly.
        encoderButton.holdTime(1000); // require button to be held for 2000ms before triggering a hold event

    // initialize the arduino serial port and send a welcome message
        Serial.begin(9600);
        
    // This timer is used when in the LCD mode. if there is no activity on the encoder it will 
    // time out and go back to the FMC mode automatically
        lcdTimer.setCounter(0, 0, 11, lcdTimer.COUNT_DOWN, lcdModeTimeOut);  // set the timer for 15 seconds
        threeBeeps();

} // end of setup()

/*In the matrix layout the follwoing items are shown :

Switch Number   See keyboard schematic
Key Name        This is the legend engraved on the plastic keycap
Value           The number that will be returned by the keypad.h library when the key is pressed.
                This is for the AeroSorft Avionics Software  https://aerosoft.com.au/aerosoft_australia/home.html
*/


//----- [Loop] ------- [Loop] ------- [Loop] ------- [Loop] ------- [Loop] ------- [Loop]-----//

void loop() {
    // When the encoder pushbutton is held for 3 seconds it sets the ledFlag = 1
         if (ledFlag == 1)
            {
                LEDtime.fupdate();  //will check if set time has past and if so will run set function
            }

    // Read Code DIP Switches that are used to control various functions on the FMC
    /*     
            
            DIP_SW_1
                ON  = AeroSoft FMC
                OFF = Debug Mode - Outputs all information for all configurations on serial monitor
            
            DIP_SW_2
                ON  = PMDG FMC - Pilot
                OFF = PMDG FMC - CoPilot

            DIP_SW_3
                ON  = ProSIM FMC - Pilot
                OFF = ProSIM FMC - Copilot
    
    */
        DIP_SW_1 = digitalRead(CODE_SELECT_SW_1);
        DIP_SW_2 = digitalRead(CODE_SELECT_SW_2);
        DIP_SW_3 = digitalRead(CODE_SELECT_SW_3);

    // Check the keypad for button pressed

    byte key = kpd.getKey();

    if (key != NO_KEY)
    {
        //Serial.println("");  // Insert blank line before next print statement

        keypadClick();

        if (DIP_SW_1 == 0) // AeroSoft value only for .lau program running on FSX
        {
            key = key - 1;  // The KeyName and Aerosoft arrays start at "0"
            Serial.println(AeroSoft[key]);
        }

        if (DIP_SW_2 == 0) // Debug - Print values from all arrays
        {
            key = key - 1;  // The KeyName and Aerosoft arrays start at "0"
            Serial.print("FMC Key Name:            ");
            Serial.println(KeyName[key]);

            key = key + 1;
            Serial.print("Keypad Array Number:   ");
            Serial.println(key);

            key = key - 1;  // The KeyName and Aerosoft arrays start at "0"
            Serial.print("AeroSoft Key Value:       ");
            Serial.println(AeroSoft[key]);

            key = key - 1;
            Serial.print("PMDG Left Value:         ");
            Serial.println(PMDG_LEFT[key]);
        }

        if (DIP_SW_3 == 0)
        {
            
        }           
    }

    // Start getting characters from the FSX lua script

    if (Serial.available() > 0)
    {
        CodeIn = getChar(); // read it by the "char getChar" routine

  // The < character signifies that the lua script wants to turn on an LED
        if (CodeIn == '<')
        {
            lessThan(); //Identifier character it "<" goto LESSTHAN void
        }
        // The > character signifies that the lua script wants to turn on an LED                    
        if (CodeIn == '>')
        {
            moreThan(); //Identifier character it ">" goto MORETHAN void
        }

 if (CodeIn == '#')
        {
            lcdButtons(); //Identifier character it "#" goto lcdControl void
        }
    } // end of if serial.available()


  // Check for rotary encoder
  
    if (fmcState == HIGH)
    {
        checkEncoder();
    }
    else
    {
        lcdControl();
    }


  // The update() method returns true if an event or state change occurred.  It serves as a passthru
  // to the Bounce2 library update() function as well, so it will stll return true if a press/release
  // is detected but has not triggered a tap/double-tap/hold event
  // The event() method returns tap, doubleTap, hold or none depending on which event was detected
  // the last time the update() method was called.  The following code accomplishes the same thing
  // we did in the 'Basic' example, but I personally prefer this arrangement.    

    if (encoderButton.update() == true)
    {
        //-------- [Encoder Button Events] ---------- [Encoder Button Events] ---------- [Encoder Button Events] ----------//

        switch (encoderButton.event())
        {

        //----- [Encoder Button Single Tap] ------- [Encoder Button Single Tap] ------- [Encoder Button Single Tap] -------//

        case (tap):
            {
                keypadClick();
                if (fmcState == LOW)
                {
                    lcdMenuButton();
                    Serial.println("");  // Insert blank line before next print statement
                    Serial.println("Encoder Button Single Tap");
                    lcdTimer.restart(); // resets the blinking LCDs so the FMC doesn't timeout and go back to FMC mode
                }
                break;
            } // end case (tap)

        //----- [Encoder Button Double Tap] ------- [Encoder Button Double Tap] ------- [Encoder Button Double Tap] -------//

        case (doubleTap):
            {
                lcdSAutoButton();
                Serial.println("");  // Insert blank line before next print statement
                Serial.println("Encoder Button Double tap");             
                lcdTimer.restart(); // resets the blinking LCDs so the FMC doesn't timeout and go back to FMC mode
                break;
            } // end case (doubleTap)

    //----- [Encoder Button Hold] ----- [Encoder Button Hold] ------ [Encoder Button Hold] ----- [Encoder Button Hold] -----//

        case (hold):
            {
                Serial.println("");  // Insert blank line before next print statement
                Serial.println("Encoder Button Held Down");
                fmcState = LOW;           
                keypadClick();
                lcdMenuButton();
                ledFlag = 1;
                Serial.println("    - Make the LEDs FLASH");
                lcdTimer.start();
                Serial.println("    - LCD Mode Timer out Timer Started");
                break;
            } // end case (hold)

        default:
            break;

        } // end switch(myButton.event() 

    } //if (myButton.update()

    lcdTimer.run();

}// end of void loop


 //----- [getChar()] ------- [getChar()] ------- [getChar()] ------- [getChar()]-----//


char getChar() //  The serial buffer routine to get a character
{
    while (Serial.available() == 0); // wait for data
    return ((char)Serial.read());
} // end of getchar routine


//----- [lessThan()] ------- [lessThan()] ------- [lessThan()] ------- [lessThan()]-----//

/* When the flight simulator wants to control the LEDs via a .lua script it first sends
   a less than (<) character followed by a single character as shown below:

    MSG  LED  = Capital Letter M
    EXEC LED  = Capital Letter E
    CALL LED  - Capital Letter D
    FAIL LED  = Capital Letter F
    OFST LED  = Capital Letter O
*/

void lessThan() // For turning LEDs "ON"
{
    char CodeIn = getChar();  // get another character from serial port

        if (CodeIn == 'M')  // found identifier for "M" for MSG LED
            {
                digitalWrite(LED_CDU_MSG, ON); // Turn on LED
            }

        if (CodeIn == 'E') // found identifier for "E" for EXEC LED
            {
                digitalWrite(LED_CDU_EXEC, OFF); // THIS GOES THROUGH A TRANSITOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
            }

        if (CodeIn == 'C') // found identifier for "D" for CALL LED
            {
                digitalWrite(LED_CDU_CALL, ON); // Turn on LED
            }

        if (CodeIn == 'F') // found identifier for "F" for FAIL LED
            {
                digitalWrite(LED_CDU_FAIL, ON); // Turn on LED
            }

        if (CodeIn == 'O') // found identifier for "O" for OFST LED
            {
                digitalWrite(LED_CDU_OFST, ON); // Turn Off LED
            }

        if (CodeIn == 'Z') // found identifier for "Z" to turn ON all LEDS
            {
                digitalWrite(LED_CDU_MSG,  ON);
                digitalWrite(LED_CDU_EXEC, OFF);// THIS GOES THROUGH A TRANSITOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
                digitalWrite(LED_CDU_CALL, ON);
                digitalWrite(LED_CDU_FAIL, ON);
                digitalWrite(LED_CDU_OFST, ON);
            }

         ledState = HIGH; // Set this HIGH so next time the Rotary Encoder Switch is pressed it will turn all LEDs OFF

}  // end of void LESSTHAN() 


//----- [moreThan()] ------- [moreThan()] ------- [moreThan()] ------- [moreThan()] ------- [moreThan()]-----//

/* 
 When the flight simulator wants to control the LEDs via a .lua script it first sends
 a greater than (>) character followed by a single character as shown below:

    MSG  LED  = Capital Letter M
    EXEC LED  = Capital Letter E
    CALL LED  - Capital Letter D
    FAIL LED  = Capital Letter F
    OFST LED  = Capital Letter O
*/

void moreThan()// For turning LEDs "OFF"
{
    char CodeIn = getChar();  // get another character from serial port

         if (CodeIn == 'M')  // found identifier for "M" for MSG LED
            {
                digitalWrite(LED_CDU_MSG, OFF); // Turn off LED
            }

        if (CodeIn == 'E') // found identifier for "E" for EXEC LED
            {
                digitalWrite(LED_CDU_EXEC, ON); // THIS GOES THROUGH A TRANSITOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
            }

        if (CodeIn == 'C') // found identifier for "C" for CALL LED
            {
                digitalWrite(LED_CDU_CALL, OFF); // Turn off LED
            }

        if (CodeIn == 'F') // found identifier for "F" for FAIL LED
            {
                digitalWrite(LED_CDU_FAIL, OFF); // Turn off LED
            }

        if (CodeIn == 'O') // found identifier for "O" for OFST LED
            {
                digitalWrite(LED_CDU_OFST, OFF); // Turn Off LED
            }

        if (CodeIn == 'X') // found identifier for "X" to turn off all LEDS
            {
                digitalWrite(LED_CDU_MSG, OFF);
                digitalWrite(LED_CDU_EXEC, ON);// THIS GOES THROUGH A TRANSITOR TO DRIVE 2 LED SO THE ON / OFF IS REVERSED
                digitalWrite(LED_CDU_CALL, OFF);
                digitalWrite(LED_CDU_FAIL, OFF);
                digitalWrite(LED_CDU_OFST, OFF);

        }
        
        ledState = LOW; // Set this LOW so next time the Rotary Encoder Switch is pressed it will turn all LEDs ON

}  // end of void MORETHAN()


//----- [checkEncoder()] ------- [checkEncoder()] ------- [checkEncoder()] ------- [checkEncoder()] -----//

void checkEncoder()
    {
        encoderCurrentStateCLK = digitalRead(EN_ROTA_Pin);  // Read the current state of inputCLK

        // If the previous and the current state of the inputCLK are different then a pulse has occured
        if (encoderCurrentStateCLK != encoderPreviousStateCLK) 
            {
   
            // If the inputDT state is different than the inputCLK state then 
        // the encoder is rotating counterclockwise
        if (digitalRead(EN_ROTB_Pin) != encoderCurrentStateCLK) {
            brightness = brightness - 5;
            if (brightness < 0)
            {
                brightness = 0;
            }
            encdir = "CCW";
            
        }
        else {
            // Encoder is rotating clockwise
            brightness = brightness + 5;
            if (brightness > 255)
            {
                brightness = 255;
            }
            encdir = "CW";
            
        }
        // This is for debug purposes to see value from the rotary encoder to control the backlight LED brigtness
            Serial.println("");  // Insert blank line before next print statement
            Serial.print("Direction: ");
            Serial.print(encdir);
            Serial.print(" -- Value: ");
            Serial.println(brightness);

        // Write to the pin that controls the backlight LEDs in each pushbutton
            analogWrite(BACK_LIGHT_PWM, brightness);

    }
        // Update previousStateCLK with the current state
            encoderPreviousStateCLK = encoderCurrentStateCLK;
}

//----- [ledsOff()] ------- [ledsOff()] ------- [ledsOff()] ------- [ledsOff()] ------- [ledsOff()] -----//

void ledsOff()
{
    ledState = 0;
   
    digitalWrite(LED_CDU_MSG, 1);
    digitalWrite(LED_CDU_EXEC, 0);        // EXEC LED via transistor
    digitalWrite(LED_CDU_CALL, 1);
    digitalWrite(LED_CDU_FAIL, 1);
    digitalWrite(LED_CDU_OFST, 1);
}

//----- [ledsBlink()] ------- [ledsBlink()] ------- [ledsBlink()] ------- [ledsBlink()] -----//

void ledBlink()
{
    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
      ledState = HIGH;
    else
      ledState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(LED_CDU_MSG, ledState);
    digitalWrite(LED_CDU_EXEC, !ledState);        // EXEC LED via transistor
    digitalWrite(LED_CDU_CALL, ledState);
    digitalWrite(LED_CDU_FAIL, ledState);
    digitalWrite(LED_CDU_OFST, ledState);
}

//----- [lcdModeTimeOut()] ------- [lcdModeTimeOut()] ------- [lcdModeTimeOut()] ------- [lcdModeTimeOut()] -----//

void lcdModeTimeOut()
{
    Serial.println("");  // Insert blank line before next print statement
    Serial.println("LCD Mode Timed Out");
    keypadClick();
    ledFlag = 0;
    ledsOff();
    lcdTimer.stop();
    fmcState = HIGH;
 }

//----- [keypadClick()] ------- [keypadClick()] ------- [keypadClick()] ------- [keypadClick()] -----//

void keypadClick()              // Used to make a sound when keypad buttons are pressed
{
    tone(BUZZER, 750, 30);  
}

void lcdPowerButton()           // Turns ON / OFF the 5" LCD Panel
{
    digitalWrite(LCD_Power, 1);
    delay(50);
    digitalWrite(LCD_Power, 0);
}
void lcdMenuButton()            // Displays LCD Menu on the 5" LCD Panel
{
    digitalWrite(LCD_Menu, 1);
    Serial.println("Menu Button Pressed");
    delay(100);
    digitalWrite(LCD_Menu, 0);
}

void lcdPlusButton()            // Navigates DOWN the LCD Menu on the 5" LCD Panel
{
    digitalWrite(LCD_Plus, 1);
    delay(100);
    digitalWrite(LCD_Plus, 0);
}

void lcdMinusButton()           // Navigates UP the LCD Menu on the 5" LCD Panel
{
    digitalWrite(LCD_Minus, 1);
    delay(100);
    digitalWrite(LCD_Minus, 0);
}

void lcdSAutoButton()           // Used to select video input or Menu BACK button on the 5" LCD Panel
{
    digitalWrite(LCD_S_AUTO, 1);
    delay(100);
    digitalWrite(LCD_S_AUTO, 0);
}



//----- [lcdControl()] ------- [lcdControl()] ------- [lcdControl()] ------- [lcdControl()] -----//

void lcdControl()
{
    encoderCurrentStateCLK = digitalRead(EN_ROTA_Pin);  // Read the current state of inputCLK

    // If the previous and the current state of the inputCLK are different then a pulse has occured
    if (encoderCurrentStateCLK != encoderPreviousStateCLK)
    {

    // If the inputDT state is different than the inputCLK state then 
    // the encoder is rotating counterclockwise
        if (digitalRead(EN_ROTB_Pin) != encoderCurrentStateCLK) {
            lcdPlusButton();
            lcdTimer.restart(); // resets the blinking LCDs so the FMC doesn't timeout and go back to FMC mode

        }
        else {
            // Encoder is rotating clockwise
            lcdMinusButton();
            lcdTimer.restart(); // resets the blinking LCDs so the FMC doesn't timeout and go back to FMC mode
        }
       
    }
    // Update previousStateCLK with the current state
        encoderPreviousStateCLK = encoderCurrentStateCLK;
}

//----- [threeBeeps()] ------- [threeBeeps()] ------- [threeBeeps()] ------- [threeBeeps()] -----//

void threeBeeps()              // Used to make a sound when keypad buttons are pressed
{
    for (int count = 0; count <= 2; count++) 
    {
        tone(BUZZER, 750, 30);
        delay(150);
    }
}

//----- [LcdButtons()] ------- [moreThan()] ------- [moreThan()] ------- [moreThan()] ------- [moreThan()]-----//



void lcdButtons()// For controlling LCD setup - this mimics pushing the buttons on the LCD button board
{
    char CodeIn = getChar();  // get another character from serial port

         if (CodeIn == 'P')  // found identifier for "M" for MSG LED
            {
                lcdPowerButton();
            }

        if (CodeIn == 'M') // found identifier for "E" for EXEC LED
            {
                lcdMenuButton();
            }

        if (CodeIn == '+') // found identifier for "C" for CALL LED
            {
                lcdPlusButton();
            }

        if (CodeIn == '-') // found identifier for "F" for FAIL LED
            {
                lcdMinusButton();
            }

        if (CodeIn == 'S') // found identifier for "O" for OFST LED
            {
                lcdSAutoButton();
            }
        if (CodeIn == 'B') // found identifier for "O" for OFST LED
        {
            ledBlink();
        }
        }
        
  

