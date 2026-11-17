// Function Declarations
void setupControlSensors();
int readButtonState();

// Define Pins for Button
const int buttonPin = D5; // Pin connected to the button

// Look for HIGH-->LOW transition
int prevButtonState = HIGH;

void setupControlSensors() {
  pinMode(buttonPin, INPUT_PULLUP);     // Set button pin as input with internal pull-up resistor

  prevButtonState = digitalRead(buttonPin); // Get initial button state
}

// Read button state, returns HIGH if not pressed, LOW if pressed
int readButtonState() {
  return digitalRead(buttonPin); 
}