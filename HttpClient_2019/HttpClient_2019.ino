#include <ArduinoJson.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


ESP8266WiFiMulti WiFiMulti;

// global variables
String services[] = { "skypeforbusiness", "teams", "exchange", "sharepoint", "onedrive" };
int services_length = 5;
String api_uri = "http://audiapifunction.azurewebsites.net/api/audiapi?code=cvp43XxTZwWkLSEZLUazqmboF8BsAPoVwIBYmlAtEf/lffYYHbQ1bQ==";

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println("HOLD UP; WAIT A MINUTE...\n");
  for (uint8_t t = 4; t > 0; t--) {
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("HACKWIFI", "redmond19");
}

void loop() {

  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) { 
    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, api_uri.c_str())) {  // HTTP

      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          
          String payload = http.getString();

          // print out the value of the http payload for debugging
          //Serial.println(payload);

          // convert the payload to a json array
          char json[1024];
          strcpy(json, payload.c_str()); 

          // buffer the character array
          StaticJsonBuffer<1000> jsonBuffer;
          JsonObject& root = jsonBuffer.parseObject(json);

          String service_to_measure;
          bool failure_bool;
          for (int n = 0; n < services_length; n++){
              service_to_measure = services[n];

              if(!root.success()) {
                Serial.println("parseObject() failed");
              } else {
                failure_bool = root[service_to_measure.c_str()];                
                
                // test for failure or success
                if (failure_bool) {
                  // there is a failure
                  Serial.printf("FAILURE FOR %s\n", service_to_measure.c_str());
                } else {
                  // all good
                  Serial.printf("%s is all GOOD \n", service_to_measure.c_str());
                }
              }
          }

        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
 
      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
  Serial.println("\n");
  delay(50000);
}
