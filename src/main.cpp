/*
;-----------------------------------------------------------------------------
; Title: Upload ESP Sensor Data
;-----------------------------------------------------------------------------
; Program Detail:
------------------------------------------------------------------------------
; Purpose: ESP8266 acts as an MQTT client, subscribing to a topic to toggle
;     an LED and publishing both the LED state and Potentiometer value
; Inputs: Push Button, MQTT messages
; Outputs: Potentiometer Voltage, LED status
; Date: 2026-11-19
; Compiler: GCC
; Author: Lucas Loughner
; Versions:
;     1.0 - Initial Version
;-----------------------------------------------------------------------------
; File Dependencies:
; wirelessFuncs.h
; controlSensors.h
; dataSensors.h
;-----------------------------------------------------------------------------
*/

#include <Arduino.h>
#include <wirelessFuncs.h>

#define READ_DELAY 250 // Delay between switch checks in milliseconds


void setup() {
  Serial.begin(9600);
  setupControlSensors();
  setupDataSensors();
  wifiConnect();
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(callback);
  delay(READ_DELAY); // Sleep for 0.25 second
}

void loop() {
  // Make sure MQTT client is connected before checking switch state
  if(!mqttClient.connected()) {
    reconnect(); // check for the latest value in inTopic 
  }
  mqttClient.loop();

  // Serial.println("Checking Switch");
  checkSwitch();
  checkPot();
  delay(READ_DELAY); // Sleep for 0.25 second
}
