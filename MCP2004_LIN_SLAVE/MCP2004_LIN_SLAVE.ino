#include <SoftwareSerial.h>

volatile const int Rx_D = 2; // Rx_D looped from Rx pin (Uno/Nano pin 0)
volatile boolean clearToSend_flg = false; // can be used to flag out that you should not try to send anything as the LIN bus is busy at the moment
const int CS = 4; // HIGH enables MCP2004/A chip

char inByte[3];

const int ledPin1 = 13;
const int led2Pin = 8;
const int led3Pin = 9;

void setup()                     
{ 
  Serial.begin(9600);
  pinMode (CS, OUTPUT); // initialize pin.
  pinMode(Rx_D, INPUT_PULLUP); // pin looped from Rx pin 0
  digitalWrite (CS, HIGH); // write pin high.
  pinMode(ledPin1, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(led3Pin, OUTPUT);
  digitalWrite(led2Pin, HIGH);
  digitalWrite(led3Pin, HIGH);
  delay(500);
  digitalWrite(ledPin1, LOW);
  busClear();
}

void loop(){
  // add your main loop code in here
  if(Serial.available() == 3){
    for (int i = 0; i < 3; i++){
    inByte[i] = Serial.read();
    }
    digitalWrite(led2Pin, HIGH);
    
  }
  else{
    digitalWrite(led2Pin, LOW);
    digitalWrite(led3Pin, LOW);
  }
}

void busClear() {

#define START_TIMER1 TCCR1B |= (1 << CS10)|(1 << CS12) // //Set CS10 and CS12 bits for 1024 prescaler:
#define STOP_TIMER1  TCCR1B &= 0B11111000

  // initialize Timer1
  TCCR1A = 0;     // set entire TCCR1A register to 0
  TCCR1B = 0;     // same for TCCR1B

  // set compare match register to desired timer count:
  OCR1A = 157; // Set timer to fire CTC interrupt after approx 10ms

  // turn on CTC mode:
  TCCR1B |= (1 << WGM12);

  // enable timer compare interrupt:
  TIMSK1 |= (1 << OCIE1A);

  attachInterrupt(0, rxdChange, CHANGE); // run rxdChange every time serial Rx changes state

}


//========================

void rxdChange() {
  if (PIND & (1<<PIND1) && (PIND & (1<<PIND0))){ // check Tx high and Rx high
    START_TIMER1;
  }
  else if (PIND & (1<<PIND1) &&  (!(PIND & (1<<PIND0)))){ // check Tx high and Rx low
    clearToSend_flg = false;
    digitalWrite(ledPin1, HIGH);
    STOP_TIMER1;
  }
}

//========================

ISR(TIMER1_COMPA_vect) // runs if timer runs for 10ms
{
  clearToSend_flg = true;
  digitalWrite(ledPin1, LOW);

  STOP_TIMER1;
}
