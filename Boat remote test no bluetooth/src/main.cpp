#include <Arduino.h>
#include <Adafruit_NeoPixel.h>


#define up 2
#define down 3
#define charging 10

#define LED_PIN 5
#define NUMPIXELS 8

String state;

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pixels.begin();
}

void loop() {

// if(digitalRead(charging) == LOW){
  if (digitalRead(up) == HIGH && digitalRead(down) == LOW){
    state = "1";
    for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 150)); //blue
      pixels.show();
      Serial.println("UP");
    }

  }
  if (digitalRead(down) == HIGH && digitalRead(up) == LOW)
  {
    state = "2";
    for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 150, 0)); //green
      pixels.show();
      Serial.println("DOWN");
    }
  }
  if (digitalRead(down) == LOW && digitalRead(up) == LOW){
    state = "0";
    for(int i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(150, 150, 150)); //red
      pixels.show();
      Serial.println("Nothing");
    }
    // pixels.setPixelColor(0, pixels.Color(200, 200, 200)); //green
    // pixels.show();
  // }
}
// if(digitalRead(charging) == HIGH){
//   for(int i=1; i<8; i++) {
//     pixels.setPixelColor(i, pixels.Color(0, 0, 0)); //off
//     pixels.show();
//   }
//   pixels.setPixelColor(0, pixels.Color(0, 100, 0)); //green
//   pixels.show();
//   delay(500);
//   pixels.setPixelColor(0, pixels.Color(0, 0, 0)); //off
//   pixels.show();
//   delay(500);
//   Serial.println("Charging");
// }

}

