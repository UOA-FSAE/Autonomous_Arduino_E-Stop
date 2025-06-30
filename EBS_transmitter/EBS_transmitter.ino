
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

int BTN_GO = 4;
int BTN_STOP = 3;

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  int go = 0;
  int stop = 0;
  int test = 0;
};

Data_Package data; // Create a variable with the above structure

void setup() {
  pinMode(BTN_GO, INPUT);
  pinMode(BTN_STOP, INPUT);
  delay(100);
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println(F("radio hardware not responding!"));
    while (1);
  }
  radio.stopListening();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
}

void loop() {
  // Send the whole data from the structure to the receiver
  data.go = digitalRead(BTN_GO);
  data.stop = digitalRead(BTN_STOP);

  radio.write(&data, sizeof(Data_Package));
  data.test = 10;

  
  Serial.print("\nTransmitter ");
  Serial.print("\ngo: ");
  Serial.print(data.go);
  Serial.print("\nstop: ");
  Serial.print(data.stop);
  Serial.print("\ntest: ");
  Serial.print(data.test);
  delay(100);
}