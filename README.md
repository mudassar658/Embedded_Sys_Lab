# Local Interconnect Network (LIN)-based Arduino Network

## Scope
The system scope is to develop a LIN based network using two Arduino hardware components along with few sub components such as sensor and led display which should have the capability to transfer sensor data from one Arduino to another Arduino in the network and display the output to a connected display.

## Requirements
The requirement of the system is to design, implement and present an embedded system based on LIN communication between two Arduinos to implement Local Interconnect Network (LIN) functionality and transfer data from master device to slave device.

## What is Local Interconnect Network (LIN) Bus
LIN (Local Interconnect Network) is a network protocol based on Serial Communication Interface (SCI) used mainly to implement communication among automobile components like doors, roof top, steering. The Local Interconnect Network (LIN), standardized with ISO17897, is a low-cost, multipoint and easily-implemented communication bus in vehicles, working as a sub-bus along with Controller Area Network (CAN) in most of the applications.

With the outgrowth of internal communication among automobile components it was required to design a network protocol that should be less expensive and able to implement communication between components which don’t required high competency, and speed in operations such as vehicle doors, roof tops and headlights as compare to existing CAN. LIN is a serial interface network came into existence as a technology in the car manufacturing industry with the growth of the industry, on the other hand the CAN bus was very expensive to implement for low fault tolerance component in cars.

## Features of LIN bus
•	It used mostly for vehicle windows, steering, wipers and air conditioning control etc.
•	It best low-cost option for less fault tolerance & speed critical components in a car.
•	Provides time triggered scheduling with guaranteed latency time.
•	There is no chance of conflict in communication because master node controls the communication and slaves are not allowed to communicate directly.
•	Its operating voltage range from 9V and 18V.
•	It consists of a single wire connection plus a ground wire with 1-20 kbits at max.
•	The slave node message varies in data length from 2, 4, 8 bytes.
•	Wakeup and sleep mode is supported.

## How does LIN bus work?
At its core LIN communication is relatively simple:

A master node connects with bus and interact through each of the slave nodes, sending a header request for information - and each slave reacts with / without data when questioned. The data in bytes are transmitted in the form of signals over the LIN.

Simply, we can say that LIN bus message frames transmitted over the bus are of a header and a response.

Specifically, LIN master node transmits header bytes over the LIN bus and this kicks off a slave, which sends either 2, 6 or 8 bytes of data in response.

## Design Approach
Since LIN bus uses Master / Slave approach so, we will use the same master / slave approach to implement communication between two Arduino controllers to setup LIN Network. The hardware shall be connected with computer via USB and interact with a software to receive instructions. We will code Master and Slave in Arduino IDE. In our circuit design we will use push button as an input at Master node and read this input at Slave and blink LEDs.

## Notes:
Our final circuit might be little different from the given below circuit diagram to implement the best practices and procedures.
 
## Schematics
![Schematic Diagram](/images/schematic.jpg)

## Breadboard
![Breadboard Diagram](/images/breadboard.png)

## Hardware Required
-	Two Arduino UNO
-	One breadboard
-	One push button / switch
-	Two MCP2003 transceivers
-	Multiple connecting Wires
-	one resistor 1k Ohm
-	two resistors 150 Ohm
-	two LED red 633nm
-	Jumper Cables (Multiple)
-	One debugger
-	One power plug
-	1 USB cable to connect with computer

# Implementation

We implemented the LIN network using two Arduino, two LINBus Breakout board with MCP2004/A and other hardware was unchanged. See following image of implementation.

![Implementation Diagram](/images/implementation.jpg)

## Schematic Diagram of LINBus Breakout board with MCP2004/A

![Implementation Diagram](/images/LinBus_Breakout_board.jpg)

## LIN Master code 

```
volatile const int Rx_D = 2; // Rx looped from Rx pin (Uno/Nano pin 0)
const int ledPin = 13;
const int CS_0 = 4; // HIGH enables MCP2004/A chip
volatile boolean clearToSend_flg = true; // can be used to flag that you shouldn't try to send anything as the LIN bus is busy

const int var_buttonPin = 8;
int var_buttonState;
int last_buttonState_var = LOW;

long lastDebounceTime = 0;
long debounceDelay = 50;

void setup()                     
{ 
  Serial.begin(9600);
  pinMode (CS_0, OUTPUT); // initialize pin.
  pinMode(Rx_D, INPUT_PULLUP); // pin looped from Rx pin 0
  digitalWrite (CS_0, HIGH); // write pin high.
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(var_buttonPin, INPUT_PULLUP);
  busClear();
} 

void loop(){
  // add your main loop code in here
  int reading = digitalRead(var_buttonPin);
  if (reading != last_buttonState_var){
    lastDebounceTime = millis();
  }

  if ((millis()-lastDebounceTime) > debounceDelay){
    if (reading != var_buttonState){
      var_buttonState = reading;
      if (var_buttonState == LOW){
        byte msg[] = {
          0x01, 0x02, 0x03
        };
        for (int i = 0; i < sizeof msg; i++){
          Serial.write(msg[i]);
        }
      }
    }
  }
  last_buttonState_var = reading;
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

  attachInterrupt(0, rxChange_fn, CHANGE); // run rxChange_fn every time serial Rx changes state

}


//========================

void rxChange_fn() {
  if (PIND & (1<<PIND1) && (PIND & (1<<PIND0))){ // check Tx high and Rx high
    START_TIMER1;
  }
  else if (PIND & (1<<PIND1) &&  (!(PIND & (1<<PIND0)))){ // check Tx high and Rx low
    clearToSend_flg = false;
    digitalWrite(ledPin, HIGH);
    STOP_TIMER1;
  }
}

//========================

ISR(TIMER1_COMPA_vect) // runs if timer runs for 10ms
{
  clearToSend_flg = true;
  digitalWrite(ledPin, LOW);

  STOP_TIMER1;
}
```

## LIN Slave code 

```
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
```

Group Members
* _Faheemuddin Mohammed_
* _Mudassar Manzoor_
