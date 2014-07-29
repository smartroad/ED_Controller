#include <EEPROM.h>
#include <Adafruit_NeoPixel.h>
#include <Keypad.h>

#define PIN 12

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(25, PIN, NEO_RGB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

uint32_t orange = strip.Color(251,77,3);
uint32_t blue = strip.Color(21,149,178);
uint32_t red = strip.Color(255,0,0);
uint32_t green = strip.Color(84,255,3);
uint32_t yellow = strip.Color(255,200,0);

#define dim 4 // how much dimmer the 'dim' colours are

uint32_t dorange = strip.Color(251/dim,77/dim,0);
uint32_t dyellow = strip.Color(255/dim,200/dim,0);
uint32_t dred = strip.Color(255/dim,0,0);
uint32_t dblue = strip.Color(21/dim,149/dim,178/dim);
uint32_t dgreen = strip.Color(84/dim,255/dim,3/dim);

byte panelbrightness = 5; // default panel brightness - over ridden in eeprom
byte panelbright = 255; // variable to store current brightness levels


const byte ROWS = 5; //four rows
const byte COLS = 6; //four columns
//define the symbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'0','1','2','3','4','5'},
  {'6','7','8','9','A','B'},
  {'C','D','E','F','G','H'},
  {'I','J','K','L','M','N'},
  {'O','P','Q','R','S','T'},
};
byte rowPins[ROWS] = {4, 3, 2, 1, 0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {10, 9, 8, 7, 6, 5}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

byte flightmode = 0; //0 = normal flight : 1 = combat mode

uint32_t ledsFlight[24] = {yellow, yellow, yellow, blue,   blue,   blue, // colours for normal flight mode
                           orange, orange, orange, blue,   blue,   blue,
                           orange, orange, orange, blue,   blue,   blue,
                                   green,  red,    orange, orange, orange,
                                                           orange, red};
                               
uint32_t ledsCombat[24] = {dyellow, dyellow, dyellow, red,    red,    red, // colours for combat flight mode
                           dorange, dorange, dorange, red,    red,    red,
                           dorange, dorange, dorange, red,    orange, red,
                                    dgreen,  dred,    orange, yellow, orange,
                                                              yellow, dblue};
                                                   
                                                   
#define idledelay 300000 // how long before entering idle/low power 300000=5 minutes
unsigned long idletime; // time last button pressed
byte idlemode = 0; // 0 = not idle : 1 = idle


void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  panelbrightness = EEPROM.read(1); // read stored brightness levels
  
  // quick test to check all colous are working
  colorWipe(strip.Color(255,0,0), 10); // Red
  colorWipe(strip.Color(0,255,0), 10); // Green
  colorWipe(strip.Color(0,0,255), 10); // Blue
  
  delay(250);
  Keyboard.begin();
}

void loop() {
  char keypressed = customKeypad.getKey(); // see if key has been pressed
  
  if (keypressed != NO_KEY) {
    keyreturn(keypressed); // return the corresponding key pressed to USB (unless backlight or combat buttons
    idletime = millis(); // store last time button pressed
    idlemode = 0; // set idle mode as 'not idle'
  }
  
  if (flightmode && !idlemode) {                    // if not idle and in combat mode display combat colours
    for(uint16_t i=0; i<strip.numPixels(); i++) {   // and set panel to active panel brightness
      strip.setPixelColor(i, ledsCombat[i]);
    }
    strip.setBrightness(panelbright);
    strip.show();
  }
  else if (!flightmode && !idlemode) {              // if not idle and in normal flight mode display flight colours
    for(uint16_t i=0; i<strip.numPixels(); i++) {   // and set panel to active panel brightness
      strip.setPixelColor(i, ledsFlight[i]);
    }
    strip.setBrightness(panelbright);
    strip.show();
  }
  
  
  
  
  if (millis() - idletime >= idledelay && !idlemode) { // check if timeout has happened for idle time
    strip.setBrightness(8);                            // if it has reduce brightness to save power
    strip.show();
    idlemode = 1;
  }
  
  
  
  
  switch (panelbrightness) { // levels of brightness
    case 0:
      panelbright = 32;
      break;
    case 1:
      panelbright = 64;
      break;
    case 2:
      panelbright = 128;
      break;
    case 3:
      panelbright = 255;
      break;
  }
}




void keyreturn(char in) { // return the key pressed via USB unless backlight or combat buttons
  switch (in) {
    case '0':
      Keyboard.press('1');
      delay(50);
      Keyboard.release('1');
      break;
    
    case '1':
      Keyboard.press('2');
      delay(50);
      Keyboard.release('2');
      break;
    
    case '2':
      Keyboard.press('3');
      delay(50);
      Keyboard.release('3');
      break;
    
    case '3':
      if (flightmode) {
        Keyboard.press('u');
        delay(50);
        Keyboard.release('u');
      }
      else {
        Keyboard.press('c');
        delay(50);
        Keyboard.release('c');
      }
      break;
      
      case '4':
      if (flightmode) {
        Keyboard.press('t');
        delay(50);
        Keyboard.release('t');
      }
      else {
        Keyboard.press(KEY_PAGE_UP);
        delay(50);
        Keyboard.release(KEY_PAGE_UP);
      }
      break;
      
      case '5':
      if (flightmode) {
        Keyboard.press('g');
        delay(50);
        Keyboard.release('g');
      }
      else {
        Keyboard.press('j');
        delay(50);
        Keyboard.release('j');
      }
      break;
      
      case '6':
      Keyboard.press('q');
      delay(50);
      Keyboard.release('q');
      break;
      
      case '7':
      Keyboard.press('w');
      delay(50);
      Keyboard.release('w');
      break;
      
      case '8':
      Keyboard.press('e');
      delay(50);
      Keyboard.release('e');
      break;
      
      case '9':
      if (flightmode) {
        Keyboard.press('i');
        delay(50);
        Keyboard.release('i');
      }
      else {
        Keyboard.press('l');
        delay(50);
        Keyboard.release('l');
      }
      break;
      
      case 'A':
      if (flightmode) {
        Keyboard.press('h');
        delay(50);
        Keyboard.release('h');
      }
      else {
        Keyboard.press(KEY_PAGE_DOWN);
        delay(50);
        Keyboard.release(KEY_PAGE_DOWN);
      }
      break;
      
      case 'B':
      if (flightmode) {
        Keyboard.press('o');
        delay(50);
        Keyboard.release('o');
      }
      else {                    // add to brightness level and reset if beyond 3
        panelbrightness++;
        if (panelbrightness >= 4) panelbrightness = 0;
        EEPROM.write(1, panelbrightness); // store current level in eeprom for next power up.
      }
      break;
      
      case 'C':
      Keyboard.press('a');
      delay(50);
      Keyboard.release('a');
      break;
      
      case 'D':
      Keyboard.press('s');
      delay(50);
      Keyboard.release('s');
      break;
      
      case 'E':
      Keyboard.press('d');
      delay(50);
      Keyboard.release('d');
      break;
      
      case 'F':
      if (flightmode) {
        Keyboard.press('k');
        delay(50);
        Keyboard.release('k');
      }
      else {
        Keyboard.press(KEY_HOME);
        delay(50);
        Keyboard.release(KEY_HOME);
      }
      break;
      
      case 'G':
      if (flightmode) {
        Keyboard.press(KEY_RETURN);
        delay(50);
        Keyboard.release(KEY_RETURN);
      }
      else {
        Keyboard.press(KEY_INSERT);
        delay(50);
        Keyboard.release(KEY_INSERT);
      }
      break;
      
      case 'H':
      if (flightmode) {
        Keyboard.press('y');
        delay(50);
        Keyboard.release('y');
      }
      else {
        Keyboard.press(KEY_END);
        delay(50);
        Keyboard.release(KEY_END);
      }
      break;
      
      case 'J':
      Keyboard.press(' ');
      delay(50);
      Keyboard.release(' ');
      break;
      
      case 'K':
      Keyboard.press(KEY_ESC);
      delay(50);
      Keyboard.release(KEY_ESC);
      break;
      
      case 'L':
      if (flightmode) {
        Keyboard.press('b');
        delay(50);
        Keyboard.release('b');
      }
      else {
        Keyboard.press(KEY_LEFT_ARROW);
        delay(50);
        Keyboard.release(KEY_LEFT_ARROW);
      }
        
      break;
      
      case 'M':
      if (flightmode) {
        Keyboard.press('m');
        delay(50);
        Keyboard.release('m');
      }
      else {
        Keyboard.press(KEY_UP_ARROW);
        delay(50);
        Keyboard.release(KEY_UP_ARROW);
      }
        
      break;
      
      case 'N':
      if (flightmode) {
        Keyboard.press('n');
        delay(50);
        Keyboard.release('n');
      }
      else {
        Keyboard.press(KEY_RIGHT_ARROW);
        delay(50);
        Keyboard.release(KEY_RIGHT_ARROW);
        
      }
      break;
      
      case 'S':
      if (flightmode) {
        Keyboard.press('v');
        delay(50);
        Keyboard.release('v');
      }
      else {
        Keyboard.press(KEY_DOWN_ARROW);
        delay(50);
        Keyboard.release(KEY_DOWN_ARROW);
        
      }
      break;
      
      case 'T':
      flightmode = 1 - flightmode; // change flightmode
      break;
  }
}

















// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}


