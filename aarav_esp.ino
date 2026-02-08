#include <HardwareSerial.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BluetoothSerial.h"
#include <driver/i2s.h>
#include <driver/adc.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI 23
#define OLED_CLK  18
#define OLED_DC   16
#define OLED_CS   5
#define OLED_RESET 17

#define LED_PIN 2  // Built-in LED on most ESP32 boards

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RESET, OLED_CS);

BluetoothSerial SerialBT;

#define MIC_PIN 34
#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 16000
#define BUFFER_SIZE 512

bool TEST_OLED = false;
bool TEST_BT = false;
bool TEST_MIC = false;
bool TEST_PIPELINE = true;

void debug(String msg) {
  Serial.print("[DEBUG] ");
  Serial.println(msg);
}

void blinkSuccess() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

void initOLED() {
  debug("Init OLED...");
  
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    debug("OLED dead, might be SSD1309 issue");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("OLED OK");
  display.display();
}

void oledShowText(String text) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextWrap(true);
  display.println(text);
  display.display();
}

void initBluetooth() {
  debug("BT starting...");
  if (!SerialBT.begin("ESP32_GLASSES")) {
    debug("BT init fail");
    while (1);
  }
  debug("BT up: ESP32_GLASSES");
}

void btHandleIncoming() {
  if (SerialBT.available()) {
    String msg = SerialBT.readStringUntil('\n');
    debug("BT RX: " + msg);
    oledShowText(msg);
  }
}

void initMic() {
  debug("Init I2S ADC for analog mic...");
  
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = BUFFER_SIZE,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_adc_mode(ADC_UNIT_1, ADC1_CHANNEL_6);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
  i2s_adc_enable(I2S_PORT);
  
  debug("Mic ready on ADC1_CH6 (GPIO34)");
}

void micCapture() {
  static uint16_t buffer[BUFFER_SIZE];
  size_t bytes_read = 0;
  
  i2s_read(I2S_PORT, buffer, BUFFER_SIZE * 2, &bytes_read, 100);
  
  if (bytes_read > 0) {
    int samples = bytes_read / 2;
    long sum = 0;
    for (int i = 0; i < samples; i++) {
      sum += buffer[i] & 0x0FFF;
    }
    int avg = sum / samples;
    
    String audioData = "AUDIO:" + String(avg) + ":" + String(samples);
    SerialBT.println(audioData);
    debug("Sent: " + audioData);
  }
}

void runTest_OLED() {
  static int frame = 0;
  oledShowText("OLED Test\nFrame: " + String(frame++));
  delay(700);
}

void runTest_BT() {
  static int tick = 0;
  SerialBT.println("BT_TEST_" + String(tick++));
  debug("BT TX test msg");
  btHandleIncoming();
  delay(1000);
}

void runTest_MIC() {
  micCapture();
  delay(50);
}

void runTest_Pipeline() {
  micCapture();
  btHandleIncoming();
  delay(100);
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.begin(115200);
  delay(1000);
  debug("===== GLASSES BOOT =====");
  
  initOLED();
  initBluetooth();
  initMic();
  
  oledShowText("Ready");
  debug("System up");
  
  blinkSuccess();  // Blink to confirm upload worked
}

void loop() {
  if (TEST_OLED) runTest_OLED();
  if (TEST_BT) runTest_BT();
  if (TEST_MIC) runTest_MIC();
  if (TEST_PIPELINE) runTest_Pipeline();
}
