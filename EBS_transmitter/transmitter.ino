#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <avr/io.h>
#include <util/delay.h>

RF24 radio(7, 8);  // CE, CSN
const byte address[6] = "49055"; //last digits of tofuu in ascii lol (probably safer than before which was 00001)

#define BUTTON_PRESS_WAIT 800 //200ms / 0.25ms
#define BLINK_MS 50
#define TIMER_MATCH_INTERVAL 400 //should_sends every 400 = 100ms/0.25ms (100ms)

volatile unsigned long button_press_millis = 0;
volatile unsigned long timer0_millis = 0;
volatile unsigned long timer0_matches = 0;

volatile bool go = 0x00;
bool should_send = 0x00;


void SetupButton() {
  DDRD &= ~(1 << DDD2); //input at pin 2.
  PORTD &= ~(1 << PD2); 
  EIMSK |= (1 << INT0);
  EICRA |= (1 << ISC00) | (1 << ISC01); //rising edge
}

void SetupTimer() {
  TIMSK0 |= (1 << OCIE0A); //Compares with OCR0A inerrupt
  TCCR0A |= (1 << WGM01); //uses CTC mode
  TCCR0B |= (1 << CS01); //8 prescaler
  OCR0A = 499; //when TOP is 499 (+1), allows for interrupt to be called every 0.25ms, a nice neat number allowing for 1 second overall intervals.
}

void SetupRadio() {
  while (!radio.begin()) {
    PulseNTimes(2, 1000);
  }
  radio.stopListening();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW); //maybe comment this out to increase range if it still doesnt work
}

void PulseNTimes(uint8_t n, int over_interval_ms) {
  DDRB |= (1 << DDB5);
  //blinks n times over the interval time 'over_interval_ms'.
  int period = over_interval_ms / n;
  int off_time = period - BLINK_MS;

  while (n > 0) {
    PORTB |= (1 << PB5);
    _delay_ms(BLINK_MS);
    PORTB &= ~(1 << PB5);
    _delay_ms(off_time);
    n--;
  }
}


int main() {
  cli();
  SetupButton();
  SetupTimer();
  SetupRadio();
  sei();

  while (1) {
    if (should_send) {
      should_send = 0x00;
      bool go_snap;
      cli(); go_snap = go; sei();
      radio.write(&go_snap, sizeof(go_snap));
    }
  }
}

ISR(INT0_vect) {//for switch debouncing
  if (timer0_millis - button_press_millis >= BUTTON_PRESS_WAIT) {
    go ^= 0x01;
    button_press_millis = timer0_millis;
  }
}

ISR(TIMER0_COMPA_vect) {
  timer0_millis++; //incremenets every 0.25ms
  timer0_matches++;
  if (timer0_matches >= TIMER_MATCH_INTERVAL) {
    should_send = 0x01;
    timer0_matches = 0;
  }
}