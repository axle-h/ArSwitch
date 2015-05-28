// Connected to power toggle switch with pull-up resistor.
// HIGH = Power off, LOW = Power on.
const int powerSwitchPin = 2;

// Connected to MOSFET with pull-down resistor.
// LOW = Pi off, HIGH = Pi on.
const int powerSignalPin = 9;

// Connected to Pi Gpio set as input
// Set to HIGH to signal PiSwitch to halt the Pi.
const int rPiGpioOutPin = 10;

// Connected to Pi Gpio set as output
// Set HIGH by pi when it's powered and PiSwitch is running.
const int rPiGpioInPin = 11;

// LED will flash while we're doing 'stuff'
const int ledPin = 13;
const long ledBlinkInterval = 500;           

int ledState;
unsigned long lastLedBlinkMillis;

void setup() {
    pinMode(powerSwitchPin, INPUT_PULLUP);
    pinMode(powerSignalPin, OUTPUT);
    pinMode(rPiGpioOutPin, OUTPUT);
    pinMode(rPiGpioInPin, INPUT);
    pinMode(ledPin, OUTPUT);
  
    // Set values.
    digitalWrite(powerSignalPin, LOW);
    digitalWrite(rPiGpioOutPin, LOW);
    
    resetLed();
}

void loop() {    
    // Check if power switch has been toggled.
    int currentPowerSwitchState = digitalRead(powerSwitchPin);
    int currentPiPowerState = digitalRead(rPiGpioInPin);
  
    // This equality looks weird because we're comparing a pulled up value with one from pi.
    if(currentPiPowerState == currentPowerSwitchState) {
        if(currentPowerSwitchState == LOW) {
            switchOn();
        } else {
            switchOff();
        }
        resetLed();
    }
}

void resetLed() {
    digitalWrite(ledPin, LOW);
    ledState = LOW;
    lastLedBlinkMillis = 0;
}

void blinkLed() {
    unsigned long currentMillis = millis();
    if(currentMillis - lastLedBlinkMillis > ledBlinkInterval) {
        // save the last time you blinked the LED 
        lastLedBlinkMillis = currentMillis;   

        // if the LED is off turn it on and vice-versa:
        if (ledState == LOW) {
            ledState = HIGH;
        } else {
            ledState = LOW;
        }
        
        // set the LED with the ledState of the variable:
        digitalWrite(ledPin, ledState);
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
        blinkLed();
        
        if(currentGpioInState == HIGH) {
            return;
        }
    }
}

void switchOff() {
    // Trigger Pi halt.
    // PiSwitch should be running and waiting for rPiGpioOutPin to be HIGH.
    digitalWrite(rPiGpioOutPin, HIGH);
  
    // Wait for signal to drop from Pi.
    while(1) {
        int currentGpioInState = digitalRead(rPiGpioInPin);
        blinkLed();
        
        if(currentGpioInState == LOW) {
            // Set led on for this.
            digitalWrite(ledPin, HIGH);
        
            // Give it 5 more seconds.
            delay(5000);
      
            // Turn off the MOSFET and Pi halt signal.
            digitalWrite(powerSignalPin, LOW);
            digitalWrite(rPiGpioOutPin, LOW);
            return;
        }
    }
}



