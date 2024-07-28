#include <Arduino.h>

#define pin_read 8

int digital_chain_value = 0;
int chain_length = 0;   //chain length after function
int turn_length = 1;    // circumference winch

void setup(){
  Serial.begin(9600);
  pinMode(pin_read, INPUT);
  Serial.println("Start up");
}
void loop(){
  if (digitalRead(pin_read) == LOW){
    digital_chain_value ++;
    chain_length = digital_chain_value * turn_length;
    Serial.println("read");
    Serial.println(chain_length);
    delay(500);
  }
}