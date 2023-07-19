const int ledPin =  13;
const int analogPin = A0;
const int openWindowPin = 15;
const int closeWindowPin = 16;
const int shutdownPin = 17;
const int turnOffButton = 18;

int ledState = LOW;
long myTimer = 0;
long myTimeout = 0;
double temp = 0.0;
double minTime = 25000.0;
double maxTime = 20.0 * 60.0 * 1000.0;

bool windowOpened = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(openWindowPin, OUTPUT);
  pinMode(closeWindowPin, OUTPUT);
  pinMode(shutdownPin, OUTPUT);
  
  pinMode(turnOffButton, INPUT);
  pinMode(analogPin, INPUT);

  
  if(true) { // initialize timer each time at startup
    myTimer = millis();
  }
  Serial.begin(9600);           //  setup serial
  
  digitalWrite(shutdownPin, HIGH); // hold the relay to power the circuit
}

void loop() {

  // open window:
  if(!windowOpened){
    Serial.println("opening Window...");
    digitalWrite(openWindowPin, HIGH);
    delay(9000); // check how long it takes
    digitalWrite(openWindowPin, LOW);
    
    windowOpened = true; // toggle flag
  }

  //read timeout:
  myTimeout = minTime + (maxTime * (analogRead(analogPin) / 1024.0));
  

  // timer end:
  if ((millis() > myTimeout + myTimer) || digitalRead(turnOffButton)) {
    myTimer = millis();
    if (ledState == LOW) ledState = HIGH;
    else ledState = LOW;
    digitalWrite(ledPin, ledState);
    
    Serial.println("Closing Window");
    digitalWrite(closeWindowPin, HIGH);
    delay(12000); // check how long it takes
    digitalWrite(closeWindowPin, LOW);
    
    digitalWrite(shutdownPin, LOW); // release relay, power down circuit
    
  }
  else {
      Serial.print("Time remaining: ");
      Serial.println(myTimeout+myTimer - millis());
  }
  
}
