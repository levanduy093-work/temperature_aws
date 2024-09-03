#include <Arduino.h>
#include <DHT.h>
#include <AWS_IOT.h>
#include <WiFi.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

AWS_IOT hornbill;

char WIFI_SSID[] = "VpnT Van tinh";
char WIFI_PASSWORD[] = "01677349185";
char HOST_ADDRESS[] = "amzcvj179e7ya-ats.iot.us-east-1.amazonaws.com";
char CLIENT_ID[] = "esp32-temperature";
char TOPIC_NAME[] = "temperature/topic";

int status = WL_IDLE_STATUS;
char payload[128];

void setup() {
  Serial.begin(115200);
  delay(2000);
  dht.begin();
  
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to WiFi network: ");
    Serial.println(WIFI_SSID);
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(5000);
  }
  Serial.println("Connected to WiFi");

  if (hornbill.connect(HOST_ADDRESS, CLIENT_ID) == 0) {
    Serial.println("Connected to AWS");
  } else {
    Serial.println("AWS connection failed");
    while (1);
  }
}

void loop() {
  
}