// Connected to power toggle switch with pull-up resistor.
// HIGH = Power off, LOW = Power on.
const int powerSwitchPin = 2;

// Connected to reset push button with upll-up resistor.
// HIGH = Do nothing, LOW = Reset.
const int resetSwitchPin = 3;

// Connected to MOSFET with pull-down resistor.
// LOW = Pi off, HIGH = Pi on.
const int powerSignalPin = 9;

// Connected to Pi Gpio set as input
// Set to HIGH to signal PiSwitch to halt the Pi.
const int rPiGpioOutPin = 10;

// Connected to Pi Gpio set as output
// Set HIGH by pi when it's powered and PiSwitch is running.
const int rPiGpioInPin = 11;

void setup() {
  pinMode(powerSwitchPin, INPUT_PULLUP);
  pinMode(resetSwitchPin, INPUT_PULLUP);
  pinMode(powerSignalPin, OUTPUT);
  pinMode(rPiGpioOutPin, OUTPUT);
  pinMode(rPiGpioInPin, INPUT);
  
  // Set values.
  digitalWrite(powerSignalPin, LOW);
  digitalWrite(rPiGpioOutPin, LOW);
}

void loop() {
  // Check if power switch has been toggled.
  int currentPowerSwitchState = digitalRead(powerSwitchPin);
  int currentResetState = digitalRead(resetSwitchPin);
  int currentPiPowerState = digitalRead(rPiGpioInPin);
  
  // This equality looks weird because we're comparing a pulled up value with one from pi.
  if(currentPiPowerState == currentPowerSwitchState) {
    if(currentPowerSwitchState == LOW) {
      switchOn();
    } else {
      switchOff();
    }
    
    return;
  }
  
  // Check if Pi is off. If so then nothing to do.
  if(currentPiPowerState == LOW) {
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
  }
}



