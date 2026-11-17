#define LED_BLINKTIME 5   // Seconds to keep LED on for before turning off

// Function Declarations
void setupDataSensors();
float readPotentiometer();
void blinkLED();

// GPIO pins
const int ledPin = D1;
const int potentiometerPin = A0;

float previousPotVal = 0;
const float threshold = 0.1;

void setupDataSensors() {
  pinMode(ledPin, OUTPUT);
  previousPotVal = readPotentiometer();
//   pinMode(potentiometerPin, INPUT);
}

float readPotentiometer() {
    // Convert raw ADC reading to usable voltage
    return analogRead(potentiometerPin) * (3.3/1023.0);
}

void turnOnLED() {
    Serial.println("Turning on LED");
    digitalWrite(ledPin, HIGH);
}

void turnOffLED() {
    Serial.println("Turning off LED");
    digitalWrite(ledPin, LOW);
}