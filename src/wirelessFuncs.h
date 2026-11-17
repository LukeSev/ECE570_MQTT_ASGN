#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <cmath>
#include <ArduinoJson.h>
#include <base64.h>

#include <private_credentials.h>
#include <controlSensors.h>
#include <dataSensors.h>

/*
;-----------------------------------------------------------------------------
; Main Program:
;-----------------------------------------------------------------------------
*/

// Function Declarations
void wifiConnect();
void callback(char* topic, byte* payload, int length);
void reconnect();
void mqttPublish(String payload, const char *publishTopic);
void checkSwitch();
void checkPot();

// WiFi information
String SSID = HOME_SSID;
String PW = HOME_PW;

WiFiClient client;
PubSubClient mqttClient(client);

const char* mqttServer = "broker.hivemq.com";
const char* publishTopicLED = "testtopic/temp/outTopic/LSL/LED";
const char* publishTopicPOT = "testtopic/temp/outTopic/LSL/POT";
const char* subscribeTopic = "testtopic/temp/inTopic/LSL";
const uint16_t mqttPort = 1883;
#define publishInterval 15000 // 15 seconds between publishes to server
#define cooldownInterval 2000 // Wait 2 seconds after potentiometer value changes for value to stabilize
#define RETAINED true


String successfulResponse = "DATA UPLOAD SUCCESSFUL";
String timeReceivedResponse = "TIME RECEIVED ALREADY EXISTS.";

String testDate = "2025-12-08%2012:00:32"; // Placeholder date

void wifiConnect() {
  WiFi.begin(SSID, PW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("Signal Strength (RSSI): %d dBm\n", WiFi.RSSI());
}

// Next two functions are from GitHub page supplied by instructor
void callback(char* topic, byte* payload, int length) {
  Serial.print("Message arrived ["); // Received the incoming message
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  // the received value is in (char)payload[i]
  }
  Serial.println();
  // Switch on the LED if a 1 was received as first character, off if a 0 was received as first character
  // add your code here
  if((char)payload[0] == '1') {
    Serial.println("LED ON");
    turnOnLED();
    mqttPublish("1", publishTopicLED);
  } else if((char)payload[0] == '0') {
    Serial.println("LED OFF");
    turnOffLED();
    mqttPublish("0", publishTopicLED);
  } else {
    Serial.println("Non-actionable MQTT message received");
  }
}
  //------------------------------------------

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    // clientId += "LSL";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if(mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      mqttClient.subscribe(subscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqttPublish(String payload, const char *publishTopic) {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.publish(publishTopic, payload.c_str(), RETAINED);
}

void checkSwitch() {
  int buttonState = readButtonState();

  // Only respond when button being pressed
  if(buttonState == LOW && prevButtonState == HIGH) {
    Serial.println("Button Pressed");
    float potVal = readPotentiometer();
    Serial.println("Publishing \"1\" to MQTT topic: " + String(publishTopicLED));
    mqttPublish("1", publishTopicLED);
    delay(5*1000);
    Serial.println("Publishing \"0\" to MQTT topic: " + String(publishTopicLED));
    mqttPublish("0", publishTopicLED);
  } 
  
  prevButtonState = buttonState;
}

void checkPot() {
  // If potentiometer value has changed, wait for it to stabilize then upload to DB
  float currPotVal = readPotentiometer();
  if(abs(currPotVal-previousPotVal) > threshold) {
    delay(cooldownInterval);
    currPotVal = readPotentiometer();
    Serial.println("Publishing potentiometer value (" + String(currPotVal) + ") to MQTT topic: " + String(publishTopicPOT));
    mqttPublish(String(currPotVal), publishTopicPOT);
  }
  previousPotVal = currPotVal;
}
