// Connected to power toggle switch.
const int powerSwitchPin = 2;

// Connected to reset push button.
const int resetSwitchPin = 3;

// Connected to MOSFET with pull-down resistor. When LOW, Pi is off, when HIGH Pi is on.
const int powerSignalPin = 4;

// Set to HIGH to signal a halt on the Pi.
const int rPiGpioOutPin = 5;

// Set HIGH (pull-up inverted) by pi when it's powered.
const int rPiGpioInPin = 6;

void setup() {
  pinMode(powerSwitchPin, INPUT_PULLUP);
  pinMode(resetSwitchPin, INPUT_PULLUP);
  pinMode(powerSignalPin, OUTPUT);
  pinMode(rPiGpioOutPin, OUTPUT);
  pinMode(rPiGpioInPin, INPUT_PULLUP);
  
  // Set values.
  digitalWrite(powerSignalPin, LOW);
  digitalWrite(rPiGpioOutPin, LOW);
}

void loop() {
  // Check if power switch has been toggled.
  int currentPowerSwitchState = digitalRead(powerSwitchPin);
  int currentPiPowerState = digitalRead(rPiGpioInPin);
  int currentResetState = digitalRead(resetSwitchPin);
  
  if(currentPiPowerState != currentPowerSwitchState) {
    if(currentPowerSwitchState == LOW) {
      switchOn();
    } else {
      switchOff();
    }
    
    return;
  }
  
  // Check if Pi is off. If so then nothing to do.
  if(currentPiPowerState == HIGH) {
    return;
  }
  
  // Check reset switch.
  if(currentResetState == LOW) {
    switchOff();
    switchOn();
    return;
  }
}

void switchOn() {
  // Turn on the MOSFET.
  digitalWrite(powerSignalPin, HIGH);
  
  // Set the gpio out LOW as Pi will halt if it sees HIGH.
  digitalWrite(rPiGpioOutPin, LOW);
  
  // Wait for signal from Pi.
  while(1) {
    int currentGpioInState = digitalRead(rPiGpioInPin);
    
    if(currentGpioInState == LOW) {
      return;
    }
    
    delay(500);
  }
}

void switchOff() {
  // Trigger Pi halt.
  digitalWrite(rPiGpioOutPin, HIGH);
  
  // Wait for signal to drop from Pi.
  while(1) {
    int currentGpioInState = digitalRead(rPiGpioInPin);
    
    if(currentGpioInState == HIGH) {
      // Give it 5 more seconds.
      delay(5000);
      
      // Turn off the MOSFET and Pi halt signal.
      digitalWrite(powerSignalPin, LOW);
      digitalWrite(rPiGpioOutPin, LOW);
      return;
    }
    
    delay(500);
  }
}


