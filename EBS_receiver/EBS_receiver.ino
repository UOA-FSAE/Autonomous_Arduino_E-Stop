#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// On Uno
// #define LED_GO 5 
// #define LED_STOP 6

// On Nano PCB
#define LED_GO 4 
#define LED_STOP 3

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  int go = 0;
  int stop = 0;
  int test = 0;
};

enum State {
  STOP = 0,
  GO = 1
};
State state = STOP;

Data_Package data; //Create a variable with the above structure

void setup() {
  pinMode(LED_GO, OUTPUT);
  pinMode(LED_STOP, OUTPUT);
  Serial.begin(9600);
  if (!radio.begin()) {
    Serial.println(F("radio hardware not responding!"));
    while (1);
  }
  Serial.println(F("radio hardware ready to receive"));
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
}

void loop() {
  // Check whether there is data to be received
  if (radio.available()) {
    Serial.print("\nRadio Available ");
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
    // Serial.println(F("Data: ${data.go}"));
    if (data.stop == HIGH) {
      state = STOP;
    }
    else if (data.go == HIGH) {
      state = GO;
    }

    if (state == GO) {  
      digitalWrite(LED_GO, HIGH);
      digitalWrite(LED_STOP, LOW);
    }
    else  {
      digitalWrite(LED_GO, LOW);
      digitalWrite(LED_STOP, HIGH);

    }
  }
  Serial.print("\nReceiver ");
  Serial.print("\ngo: ");
  Serial.print(data.go);
  Serial.print("\nstop: ");
  Serial.print(data.stop);
  Serial.print("\ntest: ");
  Serial.print(data.test);

  delay(100);
  
  
}