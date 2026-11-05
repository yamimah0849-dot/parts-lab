/*
  Bluetooth Name Tag — Parts Lab
  Board: ESP32 DevKit (just the board!)
  iPhone: use a free BLE app (nRF Connect or LightBlue). Find DEVICE_NAME, connect,
  and write ON or OFF (as text) to the characteristic to switch the blue LED.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

const char* DEVICE_NAME = "Lab Member 001";   // CHANGE ME
#define SERVICE_UUID "1b8e0001-7a3f-4c2b-9d1e-5a0b9c1d2e3f"
#define MESSAGE_UUID "1b8e0002-7a3f-4c2b-9d1e-5a0b9c1d2e3f"
const int LED = 2;   // blue LED on most ESP32 DevKit boards

class MessageCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* c) override {
    String msg = c->getValue();
    msg.trim();
    Serial.print("Phone says: ");
    Serial.println(msg);
    String upper = msg;
    upper.toUpperCase();
    if (upper == "ON") digitalWrite(LED, HIGH);
    if (upper == "OFF") digitalWrite(LED, LOW);
  }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* s) override { Serial.println("Phone connected"); }
  void onDisconnect(BLEServer* s) override {
    Serial.println("Phone disconnected: advertising again");
    BLEDevice::startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  BLEDevice::init(DEVICE_NAME);
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());
  BLEService* service = server->createService(SERVICE_UUID);
  BLECharacteristic* message = service->createCharacteristic(
    MESSAGE_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  message->setCallbacks(new MessageCallbacks());
  message->setValue("Hello from Parts Lab!");
  service->start();
  BLEAdvertising* adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(SERVICE_UUID);
  adv->setScanResponse(true);
  BLEDevice::startAdvertising();
  Serial.print("Advertising as: ");
  Serial.println(DEVICE_NAME);
}

void loop() {
  delay(1000);
}
