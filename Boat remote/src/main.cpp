#include <Arduino.h>
#include <BLEDevice.h>

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

#define up 2
#define down 3

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
}

void loop() {

  if (doConnect == true){
    if (connectToServer()){
      Serial.println("We are now connected to the BLE Server.");
    } 
    else{
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
      doConnect = false;
      }


  if (digitalRead(up) == HIGH && digitalRead(down) == LOW){
    state = "1";
    pRemoteCharacteristic->writeValue(state.c_str(), state.length());

  }
  if (digitalRead(down) == HIGH && digitalRead(up) == LOW)
  {
    state = "2";
    pRemoteCharacteristic->writeValue(state.c_str(), state.length());

  }
  else{
    state = "0";
    pRemoteCharacteristic->writeValue(state.c_str(), state.length());

  }


  std::string chain_string = pRemoteCharacteristic_Read->readValue();
  int chain_int = std::stoi(chain_string);

}
