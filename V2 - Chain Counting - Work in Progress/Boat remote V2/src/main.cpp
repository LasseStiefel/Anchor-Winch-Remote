#include <Arduino.h>
#include <BLEDevice.h>

#include <Adafruit_NeoPixel.h>

/* Specify the Service UUID of Server */
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
/* Specify the Characteristic UUID of the Write Server */
static BLEUUID charUUID_Write("50fff68d-f609-444c-ae38-512e152b6450");
/* Specify the Characteristic UUID of the Read Server*/
static BLEUUID  charUUID_Read("29388e57-e4ed-4594-9ec3-5be3a4b1c015");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLERemoteCharacteristic *pRemoteCharacteristic_Read;
static BLEAdvertisedDevice* myDevice;

String state = "0";

int chain_int;

float revolution_length = 0.2;

#define up 2
#define down 3
#define charging 10

#define LED_PIN 5
#define NUMPIXELS 8

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

int chain_function (void);


static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                            uint8_t* pData, size_t length, bool isNotify)
{
  Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient* pclient)
  {
    
  }

  void onDisconnect(BLEClient* pclient)
  {
    connected = false;
    Serial.println("onDisconnect");
  }
};

/* Start connection to the BLE Server */
bool connectToServer()
{
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());
    
  BLEClient*  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");

  pClient->setClientCallbacks(new MyClientCallback());

    /* Connect to the remote BLE Server */
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");

    /* Obtain a reference to the service we are after in the remote BLE server */
  BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr)
  {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our service");


  /* Obtain a reference to the characteristic in the service of the remote BLE server */
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID_Write);
  if (pRemoteCharacteristic == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID: ");
    Serial.println(charUUID_Write.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our characteristic");

  /* Read the value of the characteristic */
  /* Initial value is 'Hello, World!' */
  if(pRemoteCharacteristic->canRead())
  {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("The characteristic value was: ");
    Serial.println(value.c_str());
  }

  if(pRemoteCharacteristic->canNotify())
  {
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  }

/* Obtain a reference to the read characteristic in the service of the remote BLE server */
  pRemoteCharacteristic_Read = pRemoteService->getCharacteristic(charUUID_Read);
  if (pRemoteCharacteristic_Read == nullptr)
  {
    Serial.print("Failed to find our read characteristic UUID: ");
    Serial.println(charUUID_Read.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Found our read characteristic");

  /* Read the value of the read characteristic */
  if (pRemoteCharacteristic_Read->canRead())
  {
    std::string value = pRemoteCharacteristic_Read->readValue();
    Serial.print("The read characteristic value was: ");
    Serial.println(value.c_str());
  }

  if (pRemoteCharacteristic_Read->canNotify())
  {
    pRemoteCharacteristic_Read->registerForNotify(notifyCallback);
  }
    connected = true;
    return true;
}
/* Scan for BLE servers and find the first one that advertises the service we are looking for. */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
 /* Called for each advertising BLE server. */
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    /* We have found a device, let us now see if it contains the service we are looking for. */
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID))
    {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    }
  }
};



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("ESP32-BLE-Client");

  /* Retrieve a Scanner and set the callback we want to use to be informed when we
     have detected a new device.  Specify that we want active scanning and start the
     scan to run for 5 seconds. */
     
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  pixels.begin();

  pixels.setPixelColor(4, pixels.Color(150, 0, 0)); // Red
  pixels.show();
}

void loop() {

if(digitalRead(charging) == LOW){


  static String previousState = "";
  
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
      pixels.setPixelColor(4, pixels.Color(0, 200, 0)); // Green
      pixels.show();
    } else {
      Serial.println("We have failed to connect to the server; there is nothing more we will do.");
    }
    doConnect = false;
  }

  bool upPressed = digitalRead(up) == HIGH;
  bool downPressed = digitalRead(down) == HIGH;

  if (upPressed && !downPressed) {
    state = "1";
    chain_function();
  } else if (downPressed && !upPressed) {
    state = "2";
    chain_function();
  } else {
    state = "0";
  }

  if (state != previousState) {
    pRemoteCharacteristic->writeValue(state.c_str(), state.length());
    previousState = state;

    // if (state == "1") {
    //   pixels.clear();
    //   pixels.setPixelColor(0, pixels.Color(150, 0, 0)); // Red
    //   pixels.show();
    // } else if (state == "2") {
    //   pixels.clear();
    //   pixels.setPixelColor(3, pixels.Color(150, 0, 0)); // Red
    //   pixels.show();
    // } else {
    //   pixels.clear();
    //   for (int i = 0; i < NUMPIXELS; i++) {
    //     pixels.setPixelColor(4, pixels.Color(0, 200, 0)); // Green
    //   }
    //   pixels.show();
    // }
  
  }
  delay(100); // Small delay to prevent bouncing issues}
}

if(digitalRead(charging) == HIGH){
  for(int i=1; i<8; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); //off
    pixels.show();
  }
  pixels.setPixelColor(4, pixels.Color(0, 100, 0)); //green
  pixels.show();
  delay(500);
  pixels.setPixelColor(4, pixels.Color(0, 0, 0)); //off
  pixels.show();
  delay(500);
  Serial.println("Charging");
}      


}

int chain_function(void){
    std::string chain_string = pRemoteCharacteristic_Read->readValue();
    int revolution = std::stoi(chain_string);
    Serial.println("Chain Length:");
    Serial.print(revolution);

    chain_int = revolution * revolution_length;

    if (chain_int >= 5 && chain_int <10){
      pixels.setPixelColor(0, pixels.Color(100, 100, 0)); //yellow  
      pixels.show();
    }
    else if (chain_int <5){
      pixels.setPixelColor(0, pixels.Color(0, 0, 0)); //off
      pixels.show();
    }
    if (chain_int >= 10){
      pixels.setPixelColor(0, pixels.Color(0, 0, 150)); //Blue
      pixels.show();
    }

    if (chain_int >= 15 && chain_int <20){
      pixels.setPixelColor(1, pixels.Color(100, 100, 0)); //yellow
      pixels.show();
    }
    else if (chain_int <15){
      pixels.setPixelColor(1, pixels.Color(0, 0, 0)); //off
      pixels.show();
    }
    if (chain_int >= 20){
      pixels.setPixelColor(1, pixels.Color(0, 0, 150)); //Blue
      pixels.show();
    }

    if (chain_int >= 25 && chain_int <30){
      pixels.setPixelColor(2, pixels.Color(100, 100, 0)); //yellow
      pixels.show();
    }
    else if (chain_int <25){
      pixels.setPixelColor(2, pixels.Color(0, 0, 0)); //off
      pixels.show();
    }
    if (chain_int >= 30){
      pixels.setPixelColor(2, pixels.Color(0, 0, 150)); //Blue
      pixels.show();
    }

    if (chain_int >= 35 && chain_int <40){
      pixels.setPixelColor(3, pixels.Color(100, 100, 0)); //yellow
      pixels.show();
    }
    else if (chain_int <35){
      pixels.setPixelColor(3, pixels.Color(0, 0, 0)); //off
      pixels.show();
    }
    if (chain_int >= 40){
      pixels.setPixelColor(3, pixels.Color(0, 0, 150)); //Blue
      pixels.show();
    }

    if (chain_int >= 45 && chain_int <50){
      pixels.setPixelColor(7, pixels.Color(100, 100, 0)); //yellow
      pixels.show();
    }
    else if (chain_int <45){
      pixels.setPixelColor(7, pixels.Color(0, 0, 0)); //off
      pixels.show();
    }
    if (chain_int >= 50){
      pixels.setPixelColor(7, pixels.Color(0, 0, 150)); //Blue
      pixels.show();
    }

    if (chain_int >= 55 && chain_int <60){
      pixels.setPixelColor(6, pixels.Color(100, 100, 0)); //yellow
      pixels.show();
    }
    else if (chain_int <55){
      pixels.setPixelColor(6, pixels.Color(0, 0, 0)); //off
      pixels.show();
    }
    if (chain_int >= 60){
      pixels.setPixelColor(6, pixels.Color(0, 0, 150)); //Blue
      pixels.show();
    }

    return chain_int;
}
