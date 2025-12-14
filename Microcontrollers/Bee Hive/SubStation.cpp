final project code substation 

//Final Code for Substation

#include <Wire.h>       
#include <arduino.h>    
//Accelerometer 
//Accelerometer 
#include <avr/sleep.h>  //required for standby mode  
#include <avr/power.h>  //required for standby mode 
#include <avr/io.h>    
//required for XBEE 
#include "SoftwareSerial.h"         
#include "DFRobotDFPlayerMini.h"    
#define CombatIndicator 0x04   
// required for XBEE and DF player mini 
//required for df player mini audio 
//pin 10 
#define ArmedIndicator 0x08   
#define StandbyIndicator 0x10   
#define OnIndicator 0x20   
//pin 11 
//pin 12 
//pin 13 
#define IMU 0x1D        
// MMA8452Q address (accelerometer) 
#define DF_BUSY_PIN 0x40  //pin 6 
//Global Variables  
const int ssRX = 8;           
const int ssTX = 9;           
// Software RX Pin (Default: Pin 2) (XBEE) 
// Software TX Pin (Default: Pin 3) (XBEE) 
const int globalBaud = 9600;  // Baud Rate for Both SoftwareSerial and HardwareSerial (Default: 
9600 bps) (XBEE) 
static const uint8_t PIN_MP3_TX = 4;    
static const uint8_t PIN_MP3_RX = 5;    
volatile char reg;               
//software tx for dfplayer mini 
//software rx for dfplayer mini 
//accelerometer 
volatile int bits[6];            
volatile short Xin, Yin, Zin;    
//accelerometer 
//accelerometer 
char elephantDetectedByPIR;  //PIR 
char elephantDetectedByAccel; 
char elephantDetectedByFreeFall;  //accel 
char elephantDetectedByJostle;  //accel 
unsigned long lastActivity = 0;      
// used for standby 
//const unsigned long INACTIVITY_MS = 1 * 60 * 1000; // used for standby (1 minute timeout) 
const unsigned long INACTIVITY_MS = 30000; // used for standby (30 seconds) 
unsigned long lastCombatActivityTime = 0; 
const unsigned long COMBAT_TIMEOUT_MS = 15000; // 15 seconds, changeable 
 
unsigned long lastPIRMotionTime = 0; 
const unsigned long PIR_TIMEOUT_MS = 1000; 
 
unsigned long lastAccelActivityTime = 0; 
const unsigned long ACCEL_TIMEOUT_MS = 1000;  // 1 second  detection 
 
volatile bool  masterReboot;  
 
int volume = 15; 
 volatile int state = 0; 
 volatile bool unArmDevice; 
 
struct Status{ 
    bool standby; 
    bool on; 
    bool armed; 
    bool combat; 
    bool pir; 
    bool jostle; 
    bool freefall; 
    bool sound; 
}; 
 
SoftwareSerial xbee(ssRX, ssTX);  // Initialize SoftwareSerial for XBee Communication on User
Defined RX/TX Pins 
SoftwareSerial dfPlayer(PIN_MP3_RX, PIN_MP3_TX);  // Initialize SoftwareSerial for DFPlayer 
Communication on User-Defined RX/TX Pins 
DFRobotDFPlayerMini player; 
bool isSoundPlaying = false; 
 
 
 
void transmit(uint8_t sendByte) { // Transmits a byte of information over SoftwareSerial for 
XBee. 
  xbee.write(sendByte); 
  xbee.flush(); 
} 
 
void startSerial(int baud) {  // Starts both SoftwareSerial and HardwareSerial (PC Communication) 
(Default: globalBaud) 
  xbee.begin(baud); 
  Serial.begin(baud); 
  dfPlayer.begin(baud); 
} 
void readReg(unsigned char dev, unsigned char reg, unsigned char num, int data[]) { 
  int i = 0; 
 
  Wire.beginTransmission(dev); 
  Wire.write(reg); 
  Wire.endTransmission(false); 
 
  Wire.requestFrom(dev, num); 
 
  while (Wire.available()) { 
    data[i] = Wire.read(); 
    ++i; 
  } 
} 
void detection(bool power) { 
 
    if (power) { 
        int x =0; 
        if (power ==1 && x ==0){ 
          cli(); 
        EICRA |= (1<<ISC00 | 0<< ISC01);  //Enables the interrupt whenever any change happens 
        EIMSK |= (1 << INT0);             //Enables the interrupt on Pin 2 
        x =1; 
        sei(); //enable interrupts 
        } 
        else if (power ==1 && x ==1){ 
 
        }   
        else{ 
          EIMSK &= (0<<INT0); 
        } 
      } 
} 
 
void accelSetup() { 
 
  Wire.begin(); 
 
  // 
  // CHECK WHO_AM_I 
  // 
  Wire.beginTransmission(IMU); 
  Wire.write(0x0D); 
  Wire.endTransmission(false); 
 
  Wire.requestFrom(IMU, 1); 
  while(Wire.available()) reg = Wire.read(); 
 
  if (reg != 0x2A) { 
 
    return (1); 
  } else { 
  
  } 
 
  // 
  // PUT DEVICE IN STANDBY (required before changing registers) 
  // 
  Wire.beginTransmission(IMU); 
  Wire.write(0x2A); 
  Wire.endTransmission(false); 
 
  Wire.requestFrom(IMU, 1); 
  while(Wire.available()) reg = Wire.read(); 
 
  Wire.beginTransmission(IMU); 
  Wire.write(0x2A); 
  Wire.write(reg & ~(0x01));   // clear ACTIVE bit 
  Wire.endTransmission(); 
 
  // 
  // ±2g RANGE 
  // 
  Wire.beginTransmission(IMU); 
  Wire.write(0x0E); 
  Wire.endTransmission(false); 
 
  Wire.requestFrom(IMU, 1); 
  while(Wire.available()) reg = Wire.read(); 
 
  Wire.beginTransmission(IMU); 
  Wire.write(0x0E); 
  Wire.write(reg & ~(0x03));   // FS=00 = ±2g 
  Wire.endTransmission(); 
 
  // -------------------------------------------------------------------- 
  //                *** FREEFALL DETECTOR CONFIG (NEW) *** 
  // -------------------------------------------------------------------- 
 
  // 
  // 1. FF_MT_CFG (0x15) 
  // ELE=1, OAE=0 (freefall), Z/Y/XEFE = 1 → 0xB8 
  // 
  Wire.beginTransmission(IMU); 
  Wire.write(0x15); 
  Wire.write(0xB8); 
  Wire.endTransmission(); 
 
  // 
  // 2. FF_MT_THS (0x17) 
  // Example threshold ~0.25 g → 4 
  // 
  Wire.beginTransmission(IMU); 
  Wire.write(0x17); 
  Wire.write(0x04); 
  Wire.endTransmission(); 
 
  // 
  // 3. FF_MT_COUNT (0x18) 
  // Debounce count = 10 samples 
  // 
  Wire.beginTransmission(IMU); 
  Wire.write(0x18); 
  Wire.write(0x0A); 
  Wire.endTransmission(); 
 
  // 
  // 4. Enable FF_MT interrupt (INT_EN_FF_MT, register 0x2D) 
  // 
  Wire.beginTransmission(IMU); 
  Wire.write(0x2D); 
  Wire.write(0x04);      // bit2 = freefall/motion interrupt 
  Wire.endTransmission(); 
 
  // 
  // 5. Route interrupt to INT1 pin (0x2E) 
  // 
  Wire.beginTransmission(IMU); 
  Wire.write(0x2E); 
  Wire.write(0x04);      // route FF_MT to INT2 
  Wire.endTransmission(); 
 
  // -------------------------------------------------------------------- 
  //                *** END FREEFALL CONFIG *** 
  // -------------------------------------------------------------------- 
 
  // 
  // BACK TO ACTIVE MODE 
  // 
  Wire.beginTransmission(IMU); 
  Wire.write(0x2A); 
  Wire.endTransmission(false); 
 
  Wire.requestFrom(IMU, 1); 
  while(Wire.available()) reg = Wire.read(); 
 
  Wire.beginTransmission(IMU); 
  Wire.write(0x2A); 
  Wire.write(reg | (0x01));   // ACTIVE = 1 
  Wire.endTransmission(); 
  readReg(IMU, 0x01, 6, bits); 
  Xin = ((short)(bits[0] << 8) | bits[1]) >> 4; 
  Yin = ((short)(bits[2] << 8) | bits[3]) >> 4; 
  Zin = ((short)(bits[4] << 8) | bits[5]) >> 4; 
} 
int accelerometer(int accelOn){ 
 
  if(accelOn) { 
 
  
  short CurX, CurY, CurZ; 
 
  Wire.beginTransmission(IMU); 
      Wire.write(0x16);          // FF_MT_SRC 
      Wire.endTransmission(false); 
 
      Wire.requestFrom(IMU, 1); 
      while(Wire.available()) reg = Wire.read(); 
 
      bool freefall = (reg & 0x80);   // EA bit = 1 when event detected 
 
      // 
      // READ ACCEL 
      // 
      readReg(IMU, 0x01, 6, bits); 
 
      CurX = ((short)(bits[0] << 8) | bits[1]) >> 4; 
      CurY = ((short)(bits[2] << 8) | bits[3]) >> 4; 
      CurZ = ((short)(bits[4] << 8) | bits[5]) >> 4; 
 
    if (abs(CurX - Xin) > 300 || abs(CurY - Yin) > 300 || abs(CurZ - Zin) > 300 || freefall) { 
    lastAccelActivityTime = millis();  // sticky timestamp 
    elephantDetectedByJostle = 1; 
    elephantDetectedByFreeFall = freefall; 
    return 1; 
  }  
  } 
  elephantDetectedByJostle = 0; 
  elephantDetectedByFreeFall = 0; 
  return 0; 
 
} 
void enterSleep() { 
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // deepest sleep 
  sleep_enable(); 
  EIFR = (1 << INTF1) | (1 << INTF0);  // Clear INT0 & INT1 flags 
  sei(); 
  sleep_cpu();                          // Sleep here 
  sleep_disable();                      // Code resumes here after wake 
  sei(); 
} 
 
byte encodeState(Status s) { 
    byte b = 0; 
    b |= (s.standby << 0); 
    b |= (s.on      << 1); 
    b |= (s.armed   << 2); 
    b |= (s.combat  << 3); 
    return b; 
} 
byte encodeSensors(Status s) { 
    byte b = 0; 
    b |= (s.pir    << 0); 
    b |= (s.jostle  << 1); 
    b |= (s.freefall  << 2); 
    b |= (s.sound  << 3); 
    return b; 
} 
 
void changeSound(int function){ //called when we want to communicate with dfplayer for something 
  //if function is 1, we want to start looping folder 
  //if function is 2, we want to stop playing sound 
  //if function is 3, we want to increase volume by 5 
  //if function is 4, we want to change to other sound file 
  //if function is 5, we want to decrease sound by 5 
 
  while (xbee.available()) {    //get latest commands  
      byte cmd = xbee.read(); 
      lastActivity = millis(); 
      bool armToggle = cmd & 0x01; 
      if (armToggle) { 
          if (state == 2) {  unArmDevice = 1; } // force disarm if commanded (removed state = 0;) 
      } 
  } 
 
  dfPlayer.listen();      //starts communicating to dfplayer instead of xbee 
  delay(10); 
 
  if(function == 1){ 
    player.volume(25); // starting volume  
    player.loopFolder(1);  //(can do loop folder 1 but volume will need to be louder) 
  } 
 
  if(function == 2){ 
    player.stop(); 
  } 
 
  if(function == 3){    //increments volume up in units of 5 until max reached  
    volume = volume +5; 
    if(volume >= 30){ 
      volume = 30; 
    } 
    player.volume(volume); 
     
  } 
 
  if(function == 4){      //play other sound folder  
    player.volume(15); 
    player.loopFolder(2); 
  } 
 
  if(function == 5){   //decreases volume in units of 5  
    volume = volume -5; 
    player.volume(volume); 
     
  } 
 
  delay(10); 
  xbee.listen(); 
  //clearXbeeBuffer(); 
} 
 
int main(){ 
  init(); 
 
// Inputs from master 
bool armDevice; 
  bool deviceInStandby = 0; 
  bool deviceOn = 1; 
  bool deviceArmed = 0; 
  bool deviceInCombat = 0; 
  bool currentTrigger = 0; 
  bool lastTrigger = 0; 
 
  byte lastSentByte = 0; 
  byte lastSensorByte = 0; 
 
  int retriggerCount = 0; 
 
  char setSleepOnNextCycle = 0; 
  //int state = 0; 
 
unsigned long lastRetriggerTime = 0; 
 
  startSerial(globalBaud); 
 
 
  DDRD  &= ~(0x04); //set pin2 to input (0 in register) (PIR) 
  PORTD &= ~(0x04); 
  DDRB  |= CombatIndicator;  //set pin10 to output for LED indicator 
  DDRB  |= ArmedIndicator;  //set pin11 to output for LED indicator 
  DDRB  |= StandbyIndicator;  //set pin12 to output for LED indicator 
  DDRB  |= OnIndicator; 
  PORTB &= ~StandbyIndicator; 
  PORTB &= ~ArmedIndicator; 
  PORTB &= ~CombatIndicator; 
  DDRD &= ~(0x08);      // PD3 (digital pin 3) as input connected to rssi pin on xbee 
  PORTD &= ~(0x08);    // disable pull-up 
  DDRD &= ~(0x40);      // pin 6 as input (dfplayer busy) 
  PORTD |= (0x40);    // Enable pull-up 
 
    cli(); 
  EICRA |= (1<<ISC00 | 0<< ISC01);  //Enables the interrupt whenever any change happens 
  EIMSK |= (1 << INT0);             //Enables the interrupt on Pin 2 
  sei(); //enable interrupts 
 
  player.begin(dfPlayer, false, true);  // false = no ACK (non-blocking), true = reset module 
  player.volume(20);  //set volume 0-30, adjust as needed 
 
 
  while(1){ 
 
    xbee.listen(); 
    delay(10); 
    if (xbee.available()) { 
        byte cmd = xbee.read(); 
        lastActivity = millis();  // reset timeout 
        // Decode master toggles (single toggle per command) 
         
         masterReboot = cmd & 0x02; 
       
        if(masterReboot){ 
          state = 0;                     // Force UNARMED / ON state 
          deviceOn = 1; 
          deviceArmed = 0; 
          deviceInCombat = 0; 
          changeSound(1); 
          PORTB |= OnIndicator; 
          PORTB &= ~CombatIndicator; 
          PORTB &= ~ArmedIndicator; 
          Serial.println(F("MASTER ON → FORCED TO ON STATE")); 
          continue;                      // skip rest of processing 
        } 
         
        bool armToggle = cmd & 0x01; // bit 0 = arm/unarm toggle 
 
        // ---- Apply toggles immediately ---- 
          if (armToggle) { 
            if (state == 0) {        // If unarmed 
              //  deviceArmed = 1; 
                unArmDevice = 0; 
                armDevice = 1;          
               // detection(1);        // Enable detection 
            } else {                 // Already armed 
               // deviceArmed = 0; 
                armDevice = 0; 
                unArmDevice = 1; 
               // detection(0);        // Disable detection 
            } 
          } 
 
        Serial.print("Received master byte: "); 
        Serial.println(cmd, BIN); 
    } 
 
 
 
  switch(state){ 
 
      case 0:       //ON not armed  
        PORTB &= ~StandbyIndicator; 
        PORTB &= ~CombatIndicator; 
        PORTB &= ~ArmedIndicator; 
        PORTB |= OnIndicator; 
        deviceArmed = 0; 
        deviceOn = 1; 
 
        if (millis() - lastActivity >= INACTIVITY_MS) { //handles timeout - puts MCU to sleep 
after 1 minute of inactivity 
          PORTB &= ~OnIndicator;      
          //PORTB |= StandbyIndicator;    //just used for testing  
          deviceOn = 0;  
          deviceInStandby = 1; 
          setSleepOnNextCycle = 1;  //puts device in standby after sending out status to master  
        } 
 
        if(armDevice){   //armed signal from transmitter 
          deviceArmed = 1; 
          armDevice = 0; //may get rid of  
          accelSetup();  //sets up accelerometer to read initial values 
          detection(1);  //tells detection to look for elephant 
          lastAccelActivityTime = 0; 
          lastPIRMotionTime = 0; 
          state = 1;    //move to armed state 
           
         } 
 
         
      break;   
 
      case 1:         //armed state 
        deviceArmed = 1; 
        PORTB &= ~StandbyIndicator; 
        PORTB &= ~CombatIndicator; 
        PORTB |= ArmedIndicator; 
        PORTB |= OnIndicator; 
       
        elephantDetectedByAccel =  accelerometer(1); 
 
        lastTrigger = (millis() - lastAccelActivityTime < ACCEL_TIMEOUT_MS) || (millis() - 
lastPIRMotionTime <  PIR_TIMEOUT_MS); 
         
         
        if(lastTrigger){ 
          lastCombatActivityTime = millis(); // reset combat timer 
          state = 2; // move to combat state 
          deviceInCombat = 1;  
           
        } 
 
        if(elephantDetectedByAccel == 1){ 
          Serial.println("vibration detected"); 
        } 
         
        if(unArmDevice || masterReboot){                      //unArmed signal from system 
          deviceArmed = 0; 
          deviceOn = 1;  
          unArmDevice = 0; 
          masterReboot = 0; 
          state = 0;                      //move to ON /unarmed state 
          detection(0); 
          accelerometer(0); 
         } 
      break;  
 
      case 2:                              //combat state (minimum of 5 seconds) 
       deviceInCombat = 1; 
       PORTB &= ~StandbyIndicator; 
       PORTB |= ArmedIndicator; 
       PORTB |= CombatIndicator; 
       PORTB |= OnIndicator; 
 
        accelerometer(1); 
        currentTrigger = (millis() - lastAccelActivityTime < ACCEL_TIMEOUT_MS) || (millis() - 
lastPIRMotionTime < PIR_TIMEOUT_MS); 
 
        if(currentTrigger && !lastTrigger){  //logic change 
          retriggerCount++; 
          if(retriggerCount > 4){retriggerCount = 4;} 
          lastCombatActivityTime = millis(); 
           
 
          if(retriggerCount == 1){changeSound(4);} //change from normal bees to angry bees 
          if(retriggerCount == 2){changeSound(3);} //increases volume by 5 
          if(retriggerCount == 3){changeSound(3);} //increases volume by 5 again  
          if(retriggerCount == 4){changeSound(3);} //increases volume by 5 (max volume now) 
          lastRetriggerTime = millis(); 
         // Serial.print(retriggerCount); 
        } 
          lastTrigger = currentTrigger; 
 
        //decay retriggerCount every 10 seconds  
        if(retriggerCount > 0 && (millis()-lastRetriggerTime >= 10000)){ 
          retriggerCount--; 
 
          if(retriggerCount == 0){changeSound(4);} //change from normal bees to angry bees 
          if(retriggerCount == 1){changeSound(5);} //decrease volume by 5 
          if(retriggerCount == 2){changeSound(5);} //decrease volume by 5 again  
          if(retriggerCount == 3){changeSound(5);} //decrease volume by 5 (max volume now) 
          lastRetriggerTime = millis(); 
        } 
 
 
 
 
 
        //  Serial.print("volume: "); 
        //  Serial.print(volume); 
         
 
          bool currentlyPlaying = !(PIND & 0x40); 
 
        //  Serial.print(" retrigger count: "); 
        //  Serial.println(retriggerCount); 
 
          if(!isSoundPlaying && !currentlyPlaying){ //start sound code 
            while (xbee.available()) { 
                byte cmd = xbee.read(); 
                lastActivity = millis(); 
                bool armToggle = cmd & 0x01; 
                if (armToggle) { 
                    if (state == 2) {  unArmDevice = 1; } // force disarm if commanded (removed 
state = 0;) 
                } 
            } 
            changeSound(1); 
 
           
          } 
 
          isSoundPlaying = currentlyPlaying;  //update tracker variable 
     
 
             // Stay in combat until timeout 
            if(millis() - lastCombatActivityTime >= COMBAT_TIMEOUT_MS){ 
              // Stop audio before exiting 
              elephantDetectedByPIR = 0; 
              while (xbee.available()) { 
                byte cmd = xbee.read(); 
                lastActivity = millis(); 
                bool armToggle = cmd & 0x01; 
                if (armToggle) { 
                    if (state == 2) {  unArmDevice = 1; } // force disarm if commanded 
                } 
            } 
 
           
              changeSound(2); 
              isSoundPlaying = false; 
              state = 1; // go back to armed 
              deviceInCombat = 0; 
              deviceArmed = 1; 
              retriggerCount = 0; 
            } 
 
            
 
        if(unArmDevice || masterReboot){   //unArmed signal from transmitter 
          changeSound(2); 
          delay(10); 
          deviceInCombat = 0; 
          deviceArmed = 0; 
          deviceOn = 0; 
          unArmDevice = 0; 
          masterReboot = 0; 
          detection(0); 
          accelerometer(0); 
          elephantDetectedByPIR = 0; 
          elephantDetectedByAccel = 0; 
          state = 0;    //move to ON /unarmed state 
          lastAccelActivityTime = 0; 
          lastPIRMotionTime = 0; 
          lastCombatActivityTime = 0; 
          isSoundPlaying = false; 
          retriggerCount = 0; 
         } 
       break;  
 
      default:    //default state  
        state = 0;  
        unArmDevice = 0;  //may get rid of 
        armDevice = 0;   //may get rid of 
       break;  
 
    } //end case statment 
 
     
 
     
    Status s; 
 
    s.standby = deviceInStandby; 
    s.on      = deviceOn; 
    s.armed   = deviceArmed; 
    s.combat  = deviceInCombat; 
 
    s.pir     = elephantDetectedByPIR; 
    s.freefall = elephantDetectedByFreeFall; 
    s.jostle   = elephantDetectedByJostle; 
    s.sound   = !(PIND & DF_BUSY_PIN);      
 
    byte stateByte   = encodeState(s); 
    byte sensorByte  = encodeSensors(s); 
 
    if (stateByte != lastSentByte) { 
        transmit(stateByte); 
        transmit(sensorByte); 
 
        Serial.print("Sent: "); 
        Serial.print(stateByte, BIN); 
        Serial.print(" "); 
        Serial.println(sensorByte, BIN); 
 
        lastSentByte = stateByte; 
 
        delay(500); 
    } 
 
    if(setSleepOnNextCycle){ 
     // PORTB |= StandbyIndicator; 
      PORTB &= ~OnIndicator; 
      setSleepOnNextCycle = 0;  //resets for next wakeup 
      enterSleep();             // put MCU to sleep 
      lastActivity = millis();  // reset after waking up 
      deviceInStandby = 0; 
      deviceOn = 1; 
      PORTB &= ~StandbyIndicator; 
      PORTB |= OnIndicator; 
    } 
       
       
  } 
  return 0; 
} 
 
 
 
ISR(INT0_vect) { 
    static unsigned long last_trigger = 0; 
    unsigned long now = millis(); 
 
    last_trigger = now; 
    lastPIRMotionTime = now; 
    lastActivity = now; 
    elephantDetectedByPIR = 1; 
 
} 
 
 
