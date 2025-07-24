#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <WiFi.h>

#define DHTPIN 21
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
HTTPClient http;

const char* ssid = "TP-Link_60_502";
const char* password = "isgm1234";
const char* serverUrl =
    "https://thingsboard.cloud/api/v1/integrations/http/503e1217-f54a-be56-42e5-7e56040f91c0";

void setup()
{
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
}

void loop()
{
  delay(60000);

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Connection", "keep-alive");
    http.addHeader("security", "12345");

    DynamicJsonDocument doc(300);
    doc["deviceName"] = ARDUINO_BOARD;
    JsonObject values = doc.createNestedObject("values");
    values["temperature"] = String(temperature, 2);
    values["humidity"] = String(humidity, 2);

    String jsonData;
    serializeJson(doc, jsonData);

    Serial.print("Sending JSON: ");
    Serial.println(jsonData);

    unsigned long startTime = millis();  // Start time
    int httpResponseCode = http.POST(jsonData);
    unsigned long endTime = millis();  // End time

    if (httpResponseCode > 0)
    {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else
    {
      Serial.print("Error in sending POST: ");
      Serial.println(httpResponseCode);
    }

    Serial.print("HTTP POST took: ");
    Serial.print(endTime - startTime);
    Serial.println(" ms");

    http.end();
  }
  else
  {
    Serial.println("WiFi not connected");
  }
}
