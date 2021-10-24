// constants for the RGB shield
#define RGB_RED 2
#define RGB_GREEN 3
#define RGB_BLUE 4
#define RGB_BUTTON_A 5
#define RGB_BUTTON_B 6

// output on GPIO pin 1 - this controls the MOSFET via PWM
#define MOSFET_GATE 1

// track the output voltage
#define HEAT_OFF 0
#define HEAT_LOW 1
#define HEAT_MEDIUM 2
#define HEAT_HIGH 3
int heatLevel = HEAT_OFF;

// used to track control button state (standard debouncing approach
// Button A is used for on-board debugging (momentary)
// Button B works as the deployment toggler switch
int buttonAState;
int lastButtonAState = LOW;
int buttonBState;
int lastButtonBState = LOW;
unsigned long lastDebounceATime = 0;
unsigned long lastDebounceBTime = 0;
unsigned long debounceDelay = 50;

// setup the pins and make sure the blanket is off
void setup() {                
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);
  pinMode(RGB_BUTTON_A, INPUT);
  pinMode(RGB_BUTTON_B, INPUT);
  lastButtonBState = digitalRead(RGB_BUTTON_B);
  pinMode(MOSFET_GATE, OUTPUT);
  // setup serial comms for debugging
  //Serial.begin(9600);
  // intiailize it all as off
  updateFeedbackLed();
  updateOutputLevel();
}

// main functioning
void loop() {
  int changed = 0;
  changed = buttonWasPressed();
  if (changed == 1) {
    updateFeedbackLed();
    updateOutputLevel();
  }
}

int buttonWasPressed() {
  int changed = 0;
  int reading;
  
  // check button A first
  // A standard debounced button check
  // see: https://www.arduino.cc/en/Tutorial/BuiltInExamples/Debounce
  reading = digitalRead(RGB_BUTTON_A);
  if (reading != lastButtonAState) {
    lastDebounceATime = millis();
  }
  if ((millis() - lastDebounceATime) > debounceDelay) {
    if (reading != buttonAState) {
      buttonAState = reading;
      if (buttonAState == HIGH) {
        handleButtonPress();
        changed = 1;
        Serial.println("Button A Pressed");
      }
    }
  }
  lastButtonAState = reading;

  // now check for toggle on button B
  reading = digitalRead(RGB_BUTTON_B);
  if (reading != lastButtonBState) {
    lastDebounceBTime = millis();
  }
  if ((millis() - lastDebounceBTime) > debounceDelay) {
    if (reading != buttonBState) {
      buttonBState = reading;
      handleButtonPress();
      changed = 1;
      Serial.println("Button B Pressed");
    }
  }
  lastButtonBState = reading;
  
  return changed; // return 1 if the button was pressed, 0 if not
}

void updateOutputLevel() {
  int outputLevel = 0;
  switch(heatLevel) {
    case HEAT_OFF:
      outputLevel = 0;
      break;
    case HEAT_LOW:
      outputLevel = 70;
      break;
    case HEAT_MEDIUM:
      outputLevel = 150;
      break;
    case HEAT_HIGH:
      outputLevel = 255;
      break;
  }
  //Serial.print(" -> ouput level ");
  //Serial.println(outputLevel);
  analogWrite(MOSFET_GATE, outputLevel);
}

void updateFeedbackLed() {
  // I found that I needed to zero it before resetting in order to make the HEAT_LOW
  // value show up correctly. No idea why. WTF.
  analogWrite(RGB_RED, 0);
  analogWrite(RGB_GREEN, 0);
  analogWrite(RGB_BLUE, 0);
  delay(10);
  // now that we're reset the LEDs go ahead and set the right display level
  switch(heatLevel) {
    case HEAT_OFF:
      analogWrite(RGB_RED, 0);
      analogWrite(RGB_GREEN, 0);
      analogWrite(RGB_BLUE, 100);
      break;
    case HEAT_LOW:
      analogWrite(RGB_RED, 20);
      analogWrite(RGB_GREEN, 0);
      analogWrite(RGB_BLUE, 0);
      break;
    case HEAT_MEDIUM:
      analogWrite(RGB_RED, 120);
      analogWrite(RGB_GREEN, 0);
      analogWrite(RGB_BLUE, 0);
      break;
    case HEAT_HIGH:
      analogWrite(RGB_RED, 255);
      analogWrite(RGB_GREEN, 0);
      analogWrite(RGB_BLUE, 0);
      break;
  }
}

void handleButtonPress() {
  heatLevel = heatLevel + 1;
  if (heatLevel == 4) {
    heatLevel = 0;
  }
  //Serial.print("Set heatLevel to ");
  //Serial.println(heatLevel);
}
