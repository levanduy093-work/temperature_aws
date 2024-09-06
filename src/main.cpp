#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <AWS_IOT.h>
#include <WiFi.h>
#include <time.h>

Adafruit_BMP280 bmp; // I2C

AWS_IOT hornbill;

char WIFI_SSID[] = "VpnT Van tinh";
char WIFI_PASSWORD[] = "01677349185";
char HOST_ADDRESS[] = "amzcvj179e7ya-ats.iot.us-east-1.amazonaws.com";
char CLIENT_ID[] = "esp32-temperature";
char TOPIC_NAME[] = "temperature/topic";

char payload[128];

void setup() {
  Serial.begin(115200);
  delay(2000);

  // Khởi tạo BMP280
  if (!bmp.begin(0x76)) { // Địa chỉ I2C thường là 0x76 hoặc 0x77
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  // Kết nối Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(5000); // Đợi 5 giây trước khi thử lại
  }
  Serial.println("Connected to Wi-Fi");

  // Kết nối AWS IoT
  if (hornbill.connect(HOST_ADDRESS, CLIENT_ID) == 0) {
    Serial.println("Connected to AWS");
  } else {
    Serial.println("AWS connection failed");
    while (1);
  }

  // Cấu hình NTP
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("NTP configured");
}

String getCurrentTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "";
  }
  char timeStringBuff[50];
  strftime(timeStringBuff, sizeof(timeStringBuff), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
  return String(timeStringBuff);
}

void loop() {
  // Đọc nhiệt độ từ BMP280
  float temperature = bmp.readTemperature();
  if (isnan(temperature)) {
    Serial.println("Failed to read temperature from BMP280 sensor");
    return;
  }

  String status = "";

  if (temperature > 30) {
    status = "hot";
  } else if (temperature < 20) {
    status = "cold";
  } else {
    status = "normal";
  }

  String timestamp = getCurrentTime();

  snprintf(payload, sizeof(payload), "{\"temperature\": %.2f, \"timestamp\": \"%s\", \"status\": \"%s\"}", temperature, timestamp.c_str(), status.c_str());

  if (hornbill.publish(TOPIC_NAME, payload) == 0) {
    Serial.print("Published: ");
    Serial.println(payload);
  } else {
    Serial.println("Publish failed");
  }

  delay(10000);
}