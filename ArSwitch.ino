// Connected to power toggle switch with pull-up resistor.
// HIGH = Power off, LOW = Power on.
const int powerSwitchPin = 2;

// Connected to MOSFET with pull-down resistor.
// LOW = Pi off, HIGH = Pi on.
const int powerSignalPin = 9;

// Connected to Pi Gpio set as input
// Set to HIGH to signal PiSwitch to halt the Pi.
const int rPiGpioOutPin = 11;

// Connected to Pi Gpio set as output
// Set HIGH by pi when it's powered and PiSwitch is running.
const int rPiGpioInPin = 10;

// LED will flash while we're doing 'stuff'
const int ledPin = 13;
const long switchOnLedBlinkInterval = 500;
const long switchOffLedBlinkInterval = 250;

int ledState;
unsigned long lastLedBlinkMillis;

int expectedPiState;
int lastSwitchState;

void setup() {
    Serial.begin(9600);
    Serial.println("reset");
    delay(500);
    
    pinMode(powerSwitchPin, INPUT_PULLUP);
    pinMode(powerSignalPin, OUTPUT);
    pinMode(rPiGpioOutPin, OUTPUT);
    pinMode(rPiGpioInPin, INPUT);
    pinMode(ledPin, OUTPUT);
  
    // Set values.
    digitalWrite(powerSignalPin, LOW);
    digitalWrite(rPiGpioOutPin, LOW);
    
    expectedPiState = LOW;
    lastSwitchState = digitalRead(powerSwitchPin);
    
    Serial.println(lastSwitchState == HIGH ? "Switch = Off" : "Switch = On");
    delay(100);
    
    resetLed();
}

void loop() {    
    // Check if power switch has been toggled.
    int currentPowerSwitchState = digitalRead(powerSwitchPin);
    
    if(currentPowerSwitchState != lastSwitchState) {
        Serial.println(currentPowerSwitchState == HIGH ? "Switch -> Off" : "Switch -> On");
        delay(100);
        
        lastSwitchState = currentPowerSwitchState;
    }
    
    // This equality looks weird. Caused by pull-up resistor on switch.
    if(expectedPiState == currentPowerSwitchState) {
        // Don't care what the Pi is doing at this point.
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

void blinkLed(unsigned long ledBlinkInterval) {
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
    Serial.println("Pi -> On");

    // Turn on the MOSFET.
    digitalWrite(powerSignalPin, HIGH);
  
    // Set the gpio out LOW as Pi will halt if it sees HIGH.
    digitalWrite(rPiGpioOutPin, LOW);
    
    // Wait for a bit so input settles
    delay(2000);
    
    // Wait for signal from Pi.
    while(digitalRead(rPiGpioInPin) == LOW) {
        blinkLed(switchOnLedBlinkInterval);
    }
    
    expectedPiState = HIGH;
    Serial.println("Pi = Switch = On");
}

void switchOff() {
    Serial.println("Pi -> Off");

    // Trigger Pi halt.
    // PiSwitch should be running and waiting for rPiGpioOutPin to be HIGH.
    digitalWrite(rPiGpioOutPin, HIGH);
    
    // Wait for signal to drop from Pi.
    while(digitalRead(rPiGpioInPin) == HIGH) {
        blinkLed(switchOffLedBlinkInterval);
    }
    
    // Set led on while we wait.
    digitalWrite(ledPin, HIGH);

    Serial.println("Mosfet off in 5s");
    delay(5000);

    // Turn off the MOSFET and Pi halt signal.
    digitalWrite(powerSignalPin, LOW);
    digitalWrite(rPiGpioOutPin, LOW);
    
    expectedPiState = LOW;
    Serial.println("Pi = Switch = Off");
}



