#define START_PIN 2
#define END_PIN 14
#define PANIC_PIN 15
#define OCT_MINUS_PIN 16
#define OCT_PLUS_PIN 17

#define OCT_LED1_PIN 13
#define OCT_LED2_PIN 19
#define OCT_LED3_PIN 20
#define OCT_LED4_PIN 21

#define VELOCITY 0x45
#define LOW_C 0x24

#define OCT_SHIFT_COOLOFF_TIME 250
#define HOLD_MODE_BUTTON_HOLD_TIME 1500
#define LED_BLINK_TIME_LOW 100
#define LED_BLINK_TIME_HIGH 400
#define HOLD_MODE_NOTE_TOGGLE_TIME 500

#define CMD_NOTE_ON 0x90
#define CMD_NOTE_OFF 0x80

#define ANALOG_LED_HIGH 180

int octave_shift;
bool noteStateLastCycle[19][10];
bool oct_plus_state_last_cycle;
bool oct_minus_state_last_cycle;
unsigned long timer1;
unsigned long timer2;
unsigned long timer_hold_mode;
unsigned long timer_led_blink;
unsigned long timer_hold_note_toggle;
bool led_blink_high;

bool hold_mode_active; 
//int oct_shift_btn_cooloff_time = 500;

void setup() {
  // Set MIDI baud rate:
  Serial1.begin(31250);
  Serial.begin(9600);
  for(int i = START_PIN ; i <= END_PIN ; i++)
  {
    if(i == 13)
      i = 18;
    pinMode(i,INPUT_PULLUP);
    for(int j = 0; j < 10 ; j++ ) {
      noteStateLastCycle[i-START_PIN][j] = 0;  
    }    
    if(i == 18)
      i = 13;
  }
  pinMode(PANIC_PIN, INPUT_PULLUP);
  pinMode(OCT_MINUS_PIN, INPUT_PULLUP);
  pinMode(OCT_PLUS_PIN, INPUT_PULLUP);

  pinMode(OCT_LED1_PIN, OUTPUT);
  pinMode(OCT_LED2_PIN, OUTPUT);
  pinMode(OCT_LED3_PIN, OUTPUT);
  pinMode(OCT_LED4_PIN, OUTPUT);

  octave_shift = 0;
  oct_plus_state_last_cycle = 0;
  oct_minus_state_last_cycle = 0;
  hold_mode_active = false;
  timer1 = 0;
  timer2 = 0;
  timer_hold_mode = 0;
  timer_led_blink = 0;
  timer_hold_note_toggle = 0;
  led_blink_high = false;
}

void loop() {

  // panic mode
  // send all note off cmd's if panic button ins pressed
  if(digitalRead(PANIC_PIN) == LOW || hold_mode_active && digitalRead(OCT_MINUS_PIN) == LOW && digitalRead(OCT_PLUS_PIN) == LOW) {
    allNotesOff();
  }

  // hold mode

  if(digitalRead(PANIC_PIN) == HIGH) {
    timer_hold_mode = millis();
  }
  
  if(!hold_mode_active && digitalRead(PANIC_PIN) == LOW && millis() > timer_hold_mode + HOLD_MODE_BUTTON_HOLD_TIME) {
    hold_mode_active = true;
    timer_hold_mode = millis();
  }
  else if(hold_mode_active && digitalRead(PANIC_PIN) == LOW && millis() > timer_hold_mode + HOLD_MODE_BUTTON_HOLD_TIME) {
    hold_mode_active = false;
    timer_hold_mode = millis();
  }
  

  //octave shift
  if(digitalRead(OCT_MINUS_PIN) == LOW && oct_minus_state_last_cycle == 0 && millis() > timer1 + OCT_SHIFT_COOLOFF_TIME) {
    if(octave_shift >= -2) {
      octave_shift -= 1; 
    }    
    oct_minus_state_last_cycle = 1;
    timer1 = millis();
  }
  else if(digitalRead(OCT_MINUS_PIN) == HIGH) {
    oct_minus_state_last_cycle = 0;
  }
    
  if(digitalRead(OCT_PLUS_PIN) == LOW && oct_plus_state_last_cycle == 0 && millis() > timer2 + OCT_SHIFT_COOLOFF_TIME) {
    if(octave_shift <= 5) {
      octave_shift += 1; 
    }   
    oct_plus_state_last_cycle = 1;
    timer2 = millis();
  }
  else if(digitalRead(OCT_PLUS_PIN) == HIGH) {
    oct_plus_state_last_cycle = 0;
  }

  displayOctLed(octave_shift, hold_mode_active);

  
  // notes on/off

  // prevent spam if no change, only send on change
  for(int i = START_PIN ; i <= END_PIN ; i++) {
    if(i == 13)
      i = 18;
    
    if(digitalRead(i) == LOW && noteStateLastCycle[i-START_PIN][octave_shift + 2] == 0) {

      if(hold_mode_active) {
        for(int ii = START_PIN ; ii <= END_PIN ; ii++)
        {
          if(ii == 13)
            ii = 18;
          for(int jj = 0; jj < 10 ; jj++ ) {
            if(noteStateLastCycle[ii-START_PIN][jj] == 1) {
              noteStateLastCycle[ii-START_PIN][jj] = 0;
              if(ii == 18)
                ii = 13;
              noteOff((LOW_C - START_PIN + ii) + 12*(jj-2));
              if(ii == 13)
                ii = 18;              
            }
          }    
          if(ii == 18)
            ii = 13;
        } 
      }
      noteStateLastCycle[i-START_PIN][octave_shift + 2] = 1;
      if(i == 18)
        i = 13;
      noteOn((LOW_C - START_PIN + i) + 12*octave_shift, VELOCITY);
      Serial.print("Pin High, sending note");
      Serial.println((LOW_C - START_PIN + i) + 12*octave_shift);
    }
    else if((digitalRead(i) == HIGH && noteStateLastCycle[i-START_PIN][octave_shift + 2] == 1 && !hold_mode_active) 
    || (false && hold_mode_active && digitalRead(i) == LOW && noteStateLastCycle[i-START_PIN][octave_shift + 2] == 1 && millis() > timer_hold_note_toggle + HOLD_MODE_NOTE_TOGGLE_TIME)) {
      noteStateLastCycle[i-START_PIN][octave_shift + 2] = 0;
      if(i == 18)
        i = 13;
      noteOff((LOW_C - START_PIN + i) + 12*octave_shift);
      timer_hold_note_toggle = millis();
      Serial.print("Pin Low, sending note");
      Serial.println((LOW_C - START_PIN + i) + 12*octave_shift);
    }
    
    if(i == 18)
      i = 13;
  }
  


/*
  noteOn(0x90, 0x1F, 0x45);
  digitalWrite(14, HIGH);
  delay(20);
  digitalWrite(14, LOW);
  delay(20);
  */
}

// plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that
// data values are less than 127:
void noteOn(int pitch, int velocity) { 
  Serial1.write(CMD_NOTE_ON);
  Serial1.write(pitch);
  Serial1.write(velocity);
}

void noteOff(int pitch) {
  Serial1.write(CMD_NOTE_OFF);
  Serial1.write(pitch);
  Serial1.write(0x00);
}

void displayOctLed(int octaveShift, bool blink_led) {
if(!blink_led || led_blink_high)// led's on

  if(octaveShift == 0) {
    analogWrite(OCT_LED1_PIN, ANALOG_LED_HIGH);
    analogWrite(OCT_LED2_PIN, 0);
    analogWrite(OCT_LED3_PIN, 0);    
    analogWrite(OCT_LED4_PIN, 0);   
  }
  else if(octaveShift == 1) {
    analogWrite(OCT_LED1_PIN, ANALOG_LED_HIGH);
    analogWrite(OCT_LED2_PIN, ANALOG_LED_HIGH);
    analogWrite(OCT_LED3_PIN, 0);    
    analogWrite(OCT_LED4_PIN, 0);   
  }
  else if(octaveShift == 2) {
    analogWrite(OCT_LED1_PIN, ANALOG_LED_HIGH);
    analogWrite(OCT_LED2_PIN, ANALOG_LED_HIGH);
    analogWrite(OCT_LED3_PIN, ANALOG_LED_HIGH);   
    analogWrite(OCT_LED4_PIN, 0);      
  }
  else if(octaveShift >= 3) {
    analogWrite(OCT_LED1_PIN, ANALOG_LED_HIGH);
    analogWrite(OCT_LED2_PIN, ANALOG_LED_HIGH);
    analogWrite(OCT_LED3_PIN, ANALOG_LED_HIGH);    
    analogWrite(OCT_LED4_PIN, ANALOG_LED_HIGH);    
  }
  else {
    analogWrite(OCT_LED1_PIN, 0);
    analogWrite(OCT_LED2_PIN, 0);
    analogWrite(OCT_LED3_PIN, 0);    
    analogWrite(OCT_LED4_PIN, 0);  
  }

else if(!led_blink_high){ // led's off
    analogWrite(OCT_LED1_PIN, 0);
    analogWrite(OCT_LED2_PIN, 0);
    analogWrite(OCT_LED3_PIN, 0);    
    analogWrite(OCT_LED4_PIN, 0);
}
if(led_blink_high && millis() > timer_led_blink + LED_BLINK_TIME_HIGH) {
  led_blink_high = false;
  timer_led_blink = millis();
}
if(!led_blink_high && millis() > timer_led_blink + LED_BLINK_TIME_LOW) {
  led_blink_high = true;
  timer_led_blink = millis();
}

}

void allNotesOff() {
  for(int i = 0 ; i <= 127 ; i ++) {
      noteOff(i);
  }
  for(int i = START_PIN ; i <= END_PIN ; i++)
  {
    if(i == 13)
      i = 18;
    for(int j = 0; j < 8 ; j++ ) {
      noteStateLastCycle[i-START_PIN][j] = 0;  
    }    
    if(i == 18)
      i = 13;
  }
}
