String serInput;
long on_period_s = 2;
long off_period_s = 180; 
unsigned long timer; 

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH); // start with nebel on
  Serial.begin(9600); 
  Serial.println("you can type two numbers (separated by space): <on period (s)> <off period (s)>");
  Serial.print("On Period: ");
  Serial.println(on_period_s);
  Serial.print("Off Period: ");
  Serial.println(off_period_s);
  timer = millis();
}

void loop() {
  // put your main code here, to run repeatedly:

    if(Serial.available()){
        serInput = Serial.readStringUntil('\n');
    }
    int num1, num2;

    if (sscanf(serInput.c_str(), "%d %d", &num1, &num2) == 2) {
        on_period_s = num1;
        off_period_s = num2;
        Serial.print("On Period: ");
        Serial.println(on_period_s);
        Serial.print("Off Period: ");
        Serial.println(off_period_s);
    }

    if(millis() > (timer + on_period_s*1e3)) {
      digitalWrite(13, LOW);
      delay(10);
    }

    Serial.print("actual millis");
    Serial.println(millis());
    Serial.print("current timer: ");
    Serial.println(timer);
    if(millis() > (timer + on_period_s*1e3 + off_period_s*1e3)) {
      digitalWrite(13, HIGH);
      Serial.println("Starting new period");
      
      timer = millis();
      delay(10);
    }
    delay(1000);
}
