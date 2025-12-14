final project base code

//Final Code for Base/Control Station 

#include <SoftwareSerial.h>  //for xbee 
#include <LiquidCrystal_I2C.h>  //for ui 
#include <avr/io.h>   
#include <util/delay.h> 
#include <time.h>    
#include <SD.h>             //SD logging 
#include <SPI.h>            //SD logging 
 
const int ssRX = 8;           // XBee DOUT → Arduino pin 8 
const int ssTX = 9;           // XBee DIN  → Arduino pin 9 
const int globalBaud = 9600; 
SoftwareSerial xbee(ssRX, ssTX); 
LiquidCrystal_I2C lcd(0x27,20,4); 
 
const uint8_t SD_CS_PIN = 10;           //SD logging 
File logFile;                           //SD logging  
 
volatile bool sendToggle = false;         
volatile uint32_t lastPressTime = 0; 
 
volatile char updateClock = 0; 
 
struct Status { 
    bool standby; 
    bool on; 
    bool armed; 
    bool combat; 
    bool pir; 
    bool freefall; 
    bool jostle; 
    bool sound; 
}; 
 
const unsigned long LCD_REFRESH_INTERVAL = 60000;  //60 seconds 
unsigned long lastLCDRefresh = 0; 
uint8_t lastStateByte = 0xFF; //forces refresh  
uint8_t lastSensorByte = 0xff; 
 
 
void LCDUpdate(uint8_t stateByte, uint8_t sensorByte) { 
    static uint8_t lastState = 0xFF; 
    static uint8_t lastSensor = 0xFF; 
 
    if (stateByte == lastState && sensorByte == lastSensor && !updateClock)  // Only update LCD 
if something actually changed 
        return; 
 
    lastState  = stateByte; 
    lastSensor = sensorByte; 
    updateClock = 0; 
    lcd.clear();   
 
    // Line 0: Main system status  
    lcd.setCursor(0, 0); 
    if (stateByte & 0x08)      lcd.print("SYSTEM IN COMBAT  "); 
    else if (stateByte & 0x04) lcd.print("SYSTEM ARMED    "); 
    else if (stateByte & 0x02) lcd.print("SYSTEM ON       "); 
    else if (stateByte & 0x01) lcd.print("STANDBY MODE ACTIVE"); 
    else                       lcd.print("SYSTEM OFFLINE  "); 
 
    // Line 1: SENSOR ALERTS  
        lcd.setCursor(0, 1); 
    if (stateByte & 0x08) { 
        if (sensorByte & 0x01)      lcd.print("MOTION DETECTED "); 
        else if (sensorByte & 0x04) lcd.print("JOSTLE / VIBRATION "); 
        else if (sensorByte & 0x02) lcd.print("VIBRATION DETECTED  "); 
        else if (sensorByte & 0x08) lcd.print("BEES ACTIVE        "); 
        else                        lcd.print("THREAT DETECTED    "); 
    } else if(stateByte & 0x01) lcd.print("Press to Wake Device"); 
      else  lcd.print("                     ");  // blank when not in combat 
     
 
    // Line 2: Detailed bits (optional debug)  
    lcd.setCursor(0, 2); 
    lcd.print("State: "); 
    lcd.print(stateByte, BIN); 
    lcd.print(" Sens: "); 
    lcd.print(sensorByte, BIN); 
 
    // Line 3: Timestamp or custom message  
    lcd.setCursor(0, 3); 
    lcd.print("UpTime: "); 
    lcd.print((millis() / 60000)); 
    lcd.print("min   "); 
} 
 
void logEvent(const char* event, uint8_t stateByte = 0, uint8_t sensorByte = 0) { 
    logFile = SD.open("events.log", FILE_WRITE); 
    if (!logFile) return; 
 
    // Timestamp in seconds since boot 
    unsigned long seconds = millis() / 1000; 
 
    logFile.print(seconds); 
    logFile.print("s | "); 
 
    // Decode and log human-readable event 
    if (strcmp(event, "MASTER_ON") == 0) { 
        logFile.println(F("MASTER POWERED ON - WAKE-UP SENT")); 
    } 
    else if (strcmp(event, "ARM_TOGGLE") == 0) { 
        logFile.println(F("ARM/DISARM TOGGLE RECEIVED")); 
    } 
    else if (stateByte || sensorByte) { 
        // Full status received from slave 
        logFile.print(F("SLAVE: ")); 
        logFile.print(stateByte, BIN); 
        logFile.print(F(" ")); 
        logFile.print(sensorByte, BIN); 
        logFile.print(F(" → ")); 
 
        if (stateByte & 0x08) logFile.print(F("COMBAT ")); 
        else if (stateByte & 0x04) logFile.print(F("ARMED ")); 
        else if (stateByte & 0x02) logFile.print(F("ON ")); 
        else if (stateByte & 0x01) logFile.print(F("STANDBY ")); 
        else logFile.print(F("OFFLINE ")); 
 
        if (sensorByte & 0x01) logFile.print(F("| PIR ")); 
        if (sensorByte & 0x04) logFile.print(F("| JOSTLE ")); 
        if (sensorByte & 0x02) logFile.print(F("| FREEFALL ")); 
        if (sensorByte & 0x08) logFile.print(F("| SOUND ")); 
         
        logFile.println(); 
    } 
    else { 
        logFile.println(event); 
    } 
 
    logFile.flush(); 
    logFile.close(); 
} 
 
 
int main() { 
    init();                            
     
    
 
    Serial.begin(globalBaud); 
    xbee.begin(globalBaud); 
 
    Wire.begin(); 
    lcd.init(); 
    lcd.backlight(); 
    lcd.clear(); 
 
    xbee.begin(globalBaud); 
    _delay_ms(100);                   
     
 
    xbee.write((uint8_t)0x02);       // force on command 
    xbee.flush(); 
     
    Serial.println(F("Master ON → Wake-up pulse sent")); 
    lcd.setCursor(0,0); 
    lcd.print("Wake-up sent"); 
    lcd.setCursor(0,1); 
    lcd.print("Elephants Beware"); 
    lcd.setCursor(0,2); 
    lcd.print("System Booting..."); 
    delay(1000); 
    LCDUpdate(0x02, 0x00); 
 
    // sd card initiilzation  
    if (!SD.begin(SD_CS_PIN)) { 
        lcd.setCursor(0,3); 
        lcd.print("SD CARD FAIL    "); 
        Serial.println(F("SD CARD FAILED!")); 
    } else { 
        lcd.setCursor(0,3); 
        lcd.print("SD LOG ACTIVE   "); 
        Serial.println(F("SD card ready - logging enabled")); 
         
        // create/open log file with timestamp header 
        logFile = SD.open("events.log", FILE_WRITE); 
        if (logFile) { 
            logFile.println(F("\r\n=== NEW SESSION ===")); 
            logFile.print(F("Started: ")); 
            logFile.println(millis() / 1000); 
            logFile.flush(); 
            logFile.close(); 
        } 
    } 
 
    logEvent("MASTER_ON"); 
 
   // Button on pin 2 
    DDRD  &= ~(1 << DDD2);    // Input 
    PORTD |=  (1 << PD2);     // Pull-up 
 
    DDRB |= 0x04; 
    PORTB |= 0x04; 
 
    
    cli(); 
    EICRA = (EICRA & ~(1 << ISC00)) | (1 << ISC01);  // ISC01=1, ISC00=0 → falling edge 
    EIMSK |= (1 << INT0);                            // Enable INT0 
    EIFR  |= (1 << INTF0);                           // Clear flag 
    sei(); 
 
    while (1) { 
 
  
        if (sendToggle) { 
            sendToggle = false; 
            xbee.write((uint8_t)0x01); 
            logEvent("ARM/DISARM/WAKEUP TOGGLE"); 
            xbee.flush(); 
 
            Serial.println(F("→ SENT TOGGLE COMMAND (0x01)")); 
        } 
         
 
        static uint8_t currentStateByte = 0; 
        static uint8_t currentSensorByte = 0; 
        bool dataReceived = false; 
 
 
        // refresh every 60 seconds even if no new data arrives 
            if (millis() - lastLCDRefresh >= LCD_REFRESH_INTERVAL) { 
                updateClock = 1; 
                LCDUpdate(currentStateByte, currentSensorByte); 
                Serial.println("updated"); 
                lastLCDRefresh = millis(); 
            } 
 
 
 
        // receive slave status  
        if (xbee.available() >= 2) { 
            uint8_t stateByte  = xbee.read(); 
            uint8_t sensorByte = xbee.read(); 
            dataReceived = true; 
             
            currentSensorByte = sensorByte; 
            currentStateByte = stateByte; 
            LCDUpdate(stateByte, sensorByte); 
            logEvent("",stateByte, sensorByte); 
        } 
        _delay_ms(50); 
    } 
    return 0; 
} 
 
ISR(INT0_vect) { 
    uint32_t now = millis(); 
 
    // Software debounce 
    if (now - lastPressTime < 50) { 
        EIFR |= (1 << INTF0);   // Clear flag and ignore 
        return; 
    } 
 
    // Only trigger on FALLING edge (button to GND + pull-up) 
    if (!(PIND & (1 << PD2))) {     // Pin is LOW → pressed 
        sendToggle = true; 
        lastPressTime = now; 
    } 
 
    EIFR |= (1 << INTF0);           // Clear interrupt flag 
} 
 
 
 
 
 
 
