#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <string>



#define SERVICE_UUID    "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

#define UUID_WRITE_A    "50fff68d-f609-444c-ae38-512e152b6450"
#define UUID_READ_A     "29388e57-e4ed-4594-9ec3-5be3a4b1c015"



BLEServer *pServer;
BLEService *pService;

BLECharacteristic *pCharacteristic_A_get;
BLECharacteristic *pCharacteristic_A_give;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t server_value = 0;

#define up 9
#define down 10
#define hall_sensor 8

int revolution = 0;

int state_previous;

// Interrupt Service Routine (ISR) for hall sensor
void hallSensorISR() {
  if(state_previous == 1){
    revolution--;}
  else {
    revolution++;}
  Serial.println(revolution);
  Serial.println("Interrupt");
  std::string revolution_s = std::to_string(revolution);
  pCharacteristic_A_give->setValue(revolution_s);

}

    


class MyServerCallbacks: public BLEServerCallbacks {        //Server Callback 
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      digitalWrite(down, HIGH);
      digitalWrite(up, HIGH);
      Serial.println("N.A. Disconnect");
    }
};

class A_get_Callbacks: public BLECharacteristicCallbacks {      //A get value
  void onWrite(BLECharacteristic *pCharacteristic_A_get){
    std::string value = pCharacteristic_A_get->getValue();
    if (value.length() > 0){
      int state = std::stoi(value);

      if(state == 1){
        digitalWrite(up, LOW);
        state_previous = 1;
        Serial.println("UP");
      }
      else if (state == 2){
        digitalWrite(down, LOW);
        state_previous = 2;
        Serial.println("DOWN");
      }
      else{
        digitalWrite(up, HIGH);
        digitalWrite(down, HIGH);
        Serial.println("N.A. Callback");
      }
    }
  }
};

class A_give_Callbacks: public BLECharacteristicCallbacks {    //A give value
  void onRead(BLECharacteristic *pCharacteristic_A_give) {
    // Handle read for the second characteristic
    // You can provide data to be read when this characteristic is read
  }
};

void setup() {

  pinMode(down, OUTPUT);
  pinMode(up, OUTPUT);

  digitalWrite(up, HIGH);
  digitalWrite(down, HIGH);

  pinMode(hall_sensor, INPUT);

  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("AnchorWinch");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  pService = pServer->createService(SERVICE_UUID);

  pCharacteristic_A_get = pService->createCharacteristic(
      UUID_WRITE_A,
      BLECharacteristic::PROPERTY_WRITE
  );
  pCharacteristic_A_get->setCallbacks(new A_get_Callbacks());

  pCharacteristic_A_give = pService -> createCharacteristic(
      UUID_READ_A,
      BLECharacteristic::PROPERTY_READ
  );
  pCharacteristic_A_give->setCallbacks(new A_give_Callbacks());

  pService->start();

  //-------------------------------------------------------------------------------
  
  Serial.begin(115200);



  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it on your phone!");

  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif

  attachInterrupt(digitalPinToInterrupt(hall_sensor), hallSensorISR, FALLING);


}

void loop() {
  // if (!deviceConnected){
  //   digitalWrite(down, HIGH);
  //   digitalWrite(up, HIGH);
  //   Serial.println("N.A. loop");
  // }

}
