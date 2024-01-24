#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID    "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

#define UUID_WRITE_A    "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define UUID_READ_A     "2158b4bd-ecf7-424c-bc81-816aefe3f80b"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic_A_get;
BLECharacteristic *pCharacteristic_A_give;

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t server_value = 0;

#define up 2
#define down 3
#define hall_sensor 10

int chain_turns = 0;
float one_turn = 0.2;
float chain_length = 0;

void send_length();



class MyServerCallbacks: public BLEServerCallbacks {        //Server Callback 
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class A_get_Callbacks: public BLECharacteristicCallbacks
{      //A get value
  void onWrite(BLECharacteristic *pCharacteristic_A_get){
    std::string value = pCharacteristic_A_get->getValue();
    if (value.length() > 0){
      int state = std::stoi(value);
      if(state == 1){
        digitalWrite(up, HIGH);
        if(analogRead(hall_sensor)){
          chain_turns --;
        }

      }
      else if (state == 2){
        digitalWrite(down, HIGH);
                if(analogRead(hall_sensor)){
          chain_turns ++;
        }
        
      }
      else{
        digitalWrite(down, LOW);
        digitalWrite(up, LOW);
      }
    }
  }
};

class A_give_Callbacks: public BLECharacteristicCallbacks {    //A give value
  void onRead(BLECharacteristic *pCharacteristic_A_give) {
 // what to send    
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("LightStrike");
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
  pCharacteristic_A_give->setCallbacks(new A_give_Callbacks()); }

void loop() {
  // put your main code here, to run repeatedly:
}

void send_length(){
  chain_length = chain_turns * one_turn;
  pCharacteristic_A_get->writeValue(chain_length.c_str(), chain_length.length());

}