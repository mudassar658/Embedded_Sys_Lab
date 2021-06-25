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
