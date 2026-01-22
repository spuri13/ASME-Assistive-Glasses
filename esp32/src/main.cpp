#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_AudioTest");
  Serial.println("Bluetooth Classic started");
}

void loop() {
  SerialBT.println("MOCK_AUDIO: hello from esp32");
  delay(1000);
}
