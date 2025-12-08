#include <avr/io.h>   
#include <avr/interrupt.h> 
 
//define inputs and outputs  
#define B1 0x04       //select direction   (pin1) 
#define B2 0x08       //on/off movement  (pin2) 
#define B3 0x10       //emergency stop       (pin 7) 
#define LED1 0x01     //direction indicator (on = counter clockwise, off = clockwise) 
#define LED2 0x02     //moving indicator (on=moving) 
#define LED3 0x04     //emergency stop (active if on) 
 
#define AIN1 0x10     //AIN1 for opto to motor (pin 12) 
#define AIN2 0x20     //AIN2 for opto to motor  (pin 13) 
#define PWM 0x40      //PWM signal for opto to motor (pin 6) 
 
//interupt variables  
volatile int Button1 = 0; 
volatile int Button2 = 0; 
volatile int Button3 = 0; 
 
volatile unsigned char analogVal = 1; 
 
int main(){ 
 
init(); 
 
cli(); //disable interupts  
 
unsigned char state = 0; 
 
unsigned char outPattern[5] = { 0x00,  
                                0x01, //LED1 
                                0x02, //LED 2 
                                0x03, //LED 1 AND LED 2 
                                0x04, //LED 3 
};  
DDRD &= ~(B1|B2|B3);             
DDRB |= (LED1)|(LED2)|(LED3);    
PORTD |= (B1|B2|B3);   
//configure inputs on PORTD 
//configure outputs on PORTB 
//Enable pull-up resistors on inputs  
PORTB &= ~((LED1)|(LED2)|(LED3));   
PORTB |= outPattern[state];   
//declarations for motor outputs 
DDRB |= (AIN1)|(AIN2); //configures motor outputs on PORTB 
DDRD |= (PWM);   
//pwm signals  
//configures pin 6 for pwm output signal 
TCCR0A |= (1<<COM0A1);   
TCCR0A &= ~(1<<COM0A0); 
TCCR0A |= (1<<WGM01)|(1<<WGM00);  //set WGM for Fast PWM (mode3) 
TCCR0B &= ~(1<<WGM02); 
TCCR0B |= (1<<CS00);              
//set prescaler (1) 
TCCR0B &= ~( (1<<CS02)|(1<<CS01) ); 
OCR0A = analogVal; 
//adc configuration  
ADMUX |= (1<<REFS0); //set ref to avcc 
ADMUX &= ~(1<<REFS1); 
ADMUX &= 0xF0; //clear mux[3:0] to select A0 
ADMUX |= (1<<ADLAR); 
ADCSRA |= ( (1<<ADEN)|(1<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)); 
//intterupt declarations  
EIMSK |= (1<<INT0);  //Sets interrupt on PD2 (pin2) 
EIMSK |= (1<<INT1);  //Sets interrupt on PD3 (pin3) 
EICRA |= (1 << ISC01);               // INT0: falling edge 
EICRA &= ~(1 << ISC00); 
EICRA |= (1 << ISC11);               // INT1: falling edge 
EICRA &= ~(1 << ISC10); 
 
PCICR |= (1<<PCIE2);  //allows pins on portd to act as external interrupts  
PCMSK2 |= (1<<PCINT20); //enables interrupt on PD4 (pin4) 
 
sei(); //enables interrupts 
 
ADCSRA |= (1<<ADSC); //starts ADC conversation 
 
 
while(1){ 
   
  switch(state){ 
 
  case 0:         //no movement (set to clockwise) 
 PORTB |= AIN1; 
  PORTB |= AIN2; 
    if(Button1){  
    state = 1;  
  Button1 = 0;  } 
  else if(Button2){   
    state = 2; 
   Button2 = 0;  
  } 
    else if(Button3){   
      state = 4;  
    Button3 = 0;  
    } 
      else{ 
      state = 0; 
      } 
   
 
  break;   
 
  case 1:       //no movement (set to counter clockwise) 
  PORTB |= AIN1; 
  PORTB |= AIN2; 
    if(Button1){   
    state = 0; 
  Button1 = 0; }   
  else if (Button2) { 
    state = 3;  
  Button2 = 0; } 
    else if (Button3)  { 
      state = 4;  
    Button3 = 0; } 
      else{ 
      state = 1; 
      } 
  break;   
 
  case 2:       //moving clockwise  
  PORTB &= ~AIN1; 
  PORTB |= AIN2; 
    if(Button1) {  
    state = 3;   
  Button1 = 0; } 
  else if (Button2){   
    state = 0;  
  Button2 = 0; } 
    else if (Button3) {  
      state = 4;  
    Button3 = 0; } 
      else{ 
      state = 2; 
      } 
  break;   
 
  case 3:        //moving counter clockwise 
  PORTB |= AIN1; 
  PORTB &= ~AIN2; 
    if(Button1) {  
    state = 2; 
  Button1 = 0;  }  
  else if (Button2){   
    state = 1;  
  Button2 = 0; } 
    else if (Button3){   
      state = 4;  
    Button3 = 0; } 
      else{ 
      state = 3; 
      } 
  break;   
 
  case 4:        //emergency stop 
  PORTB &= ~AIN1; 
  PORTB &= ~AIN2; 
    if(Button1){   
    state = 0; 
  Button1 = 0;  } 
  else if (Button2)  { 
    state = 0;  
  Button2 = 0; } 
    else  { 
      state = 4;  
    } 
  break;   
  default: 
    state =0; 
  break; 
  } 
 
PORTB &= ~((LED1)|(LED2)|(LED3));   
PORTB |= outPattern[state];  
} 
return 0; 
} 
//end main  
//interupts  
ISR(INT0_vect){   //interrupt for button 1 
if (!(PIND & B1)) { // Check if button is pressed (low, due to pull-up) 
Button1 = 1; 
} 
} 
ISR(INT1_vect){  //interrupt for button 2 
if (!(PIND & B2)) { // Check if button is pressed (low, due to pull-up) 
Button2 = 1; 
} 
} 
ISR(PCINT2_vect){   //interrupt for button 3 
if (!(PIND & B3)) { // Check if button is pressed (low, due to pull-up) 
Button3 = 1; 
} 
} 
ISR(ADC_vect){    
analogVal = ADCH;    
//interrupt for potentiometer  
//read the 8bit adc values 
OCR0A = analogVal;    
ADCSRA |= (1<<ADSC); 
} 
