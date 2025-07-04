#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <avr/io.h>
#include <util/delay.h>

RF24 radio(7, 8);  // CE, CSN
const byte address[6] = "49055"; //last digits of tofuu in ascii lol (probably safer than before which was 00001)

#define BLINK_MS 50

bool go = 0x00;



void SetupRadio() {
  while (!radio.begin()) {
    asm(nop);
  }
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW); //maybe comment this out to increase range if it still doesnt work
  radio.startListening();
}



int main() {
  cli();
  SetupRadio();
  sei();

  while (1) {
    if (radio.available()) {
      radio.read(&go, sizeof(go));

    if (go) {  
      digitalWrite(LED_GO, HIGH);
      digitalWrite(LED_STOP, LOW);
    }
    else  {
      digitalWrite(LED_GO, LOW);
      digitalWrite(LED_STOP, HIGH);

    }
  }
  }
}