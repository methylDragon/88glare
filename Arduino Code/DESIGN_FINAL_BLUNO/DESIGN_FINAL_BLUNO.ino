/*
                            .     .
                         .  |\-^-/|  .    
                        /| } O.=.O { |\  
                       /´ \ \_ ~ _/ / `\
                     /´ |  \-/ ~ \-/  | `\
                     |   |  /\\\ //\  |  | 
                      \|\|\/-""-""-\/|/|/
                              ______/ /
                              '------'
                _   _        _  ___                         
      _ __  ___| |_| |_ _  _| ||   \ _ _ __ _ __ _ ___ _ _  
     | '  \/ -_)  _| ' \ || | || |) | '_/ _` / _` / _ \ ' \ 
     |_|_|_\___|\__|_||_\_, |_||___/|_| \__,_\__, \___/_||_|
                        |__/                |___/
     -------------------------------------------------------
                        methylDragon.com

Author: methylDragon
MODE-SWITCHING ACTIVE SUNGLASS FIRMWARE

*/

//Scenario A: No user headlight, max 50
//Scenario B: Low user headlight, max 120
//Scenario C: High user headlight, max 255

//DEBUG MODE and PARAMs
const int DEBUGMODE = 0; //0 is OFF, 1 is ON
const double MULTIPLIER = 5.2; //To bring forward saturation point
const int AMAX = 70; //MAX PWM for A (+20 for tanh compensation)
const int BMAX = 170; //MAX PWM for B (+40 for tanh compensation)
const int CMAX = 315; //MAX PWM for C (+60 for tanh compensation)

//Calculation and easeMap variables
const int THRESHOLDA = 340;
const int THRESHOLDB = 340;
const int THRESHOLDC = 340;
const double NATURALE = 2.718281828459;
double x;

//Variables dealing with mode switches between A, B, and C
const int BUTTONPIN = INT0; //Pin 2
volatile int buttonState = 1; //Volatile used for interrupt (Default mode B)
const int LEDA = 10; //CHANGE THIS. LED TO DEMARCATE MODE A IS ACTIVE
const int LEDB = 11; //CHANGE THIS. LED TO DEMARCATE MODE B IS ACTIVE
const int LEDC = 12; //CHANGE THIS. LED TO DEMARCATE MODE C IS ACTIVE

//LDR variables
const int LLDRPIN = A0; //A0 is left
const int MLDRPIN = A2; //A2 is middle
const int RLDRPIN = A1; //A1 is right

//LCD variables
const int LCDPIN = 6; //CHANGE THIS

int lLDRval = 0;
int mLDRval = 0;
int rLDRval = 0;
int LDRavr = 0;
double LDRmoving = 0.0;
int LDRreadings[5] = {0, 0, 0, 0, 0};

int max3(int a, int b, int c) {
  int output;

  output = max(a,b);
  output = max(output,c);

  return(output);  
}

//map() but for doubles
double doubleMap(double x, double in_min, double in_max, double out_min, double out_max) {
  double temp = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
  temp = int(4*temp + 0.5);
  return double(temp/4);
}

//Self-explanatory, calculates moving average. With delay 200, takes an average of 5 elements in a window of 1 second
void updateMovingAverage(){
  for (int i = 0; i < 4; i++){
    LDRreadings[i] = LDRreadings[i+1]; //Discard first value, shift all values left to free up the last element for replacement
  }
  LDRreadings[4] = LDRavr;

  int arraySum = 0;

  for (int i = 0; i < 5; i++){
    arraySum += LDRreadings[i]; //Sum the array elements
  }
  LDRmoving = arraySum/5.0; //Find the average  
}

//Maps 0-1023 (LDRmoving) onto 0-255 (for PWM), then runs it through an easing function, scaling the piecewise functions and overall functions according to threshold
int easeMap(double light){
  switch(buttonState){
    case 0:
      if (light < THRESHOLDA) { //If light amount is LESS than the threshold, return 0, if not, return a value according to the easing function, scaled accordingly
        return 0;
      } else {
        x = doubleMap(light, THRESHOLDA, 1023, 0, 100); //Scale x from threshold to max to 0 to 100 (resolution loss from doubleMap is a lot worse than from tanh)
        return int(constrain(MULTIPLIER/3*AMAX*tanh(x/100), 0, 255)); //Scales the part after the rectified (straight 0) section along tanh
      }
      break;
    case 1:
      if (light < THRESHOLDB) {
        return 0;
      } else {
        x = doubleMap(light, THRESHOLDB, 1023, 0, 100); //Scale x from threshold to max to 0 to 100 (resolution loss from doubleMap is a lot worse than from tanh)
        return int(constrain(MULTIPLIER/3*BMAX*tanh(x/100), 0, 255));
      }
      break;
    case 2:
      if (light < THRESHOLDC) {
        return 0;
      } else {
        x = doubleMap(light, THRESHOLDC, 1023, 0, 100); //Scale x from threshold to max to 0 to 100 (resolution loss from doubleMap is a lot worse than from tanh)
        return int(constrain(MULTIPLIER/3*CMAX*tanh(x/100), 0, 255));
      }
      break;
  }
}

//Interrupt handler for button press
void buttonPress() {
  static unsigned long last_interrupt_time = 0; //Static allows the local variable to persist after the function resolves
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 150ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 150) 
  {
    if (buttonState < 2) { //Cycle button state between 0, 1, and 2
      buttonState++;
    } else {
      buttonState = 0;
    }
    switch(buttonState) {
      case 0:
        digitalWrite(LEDA, HIGH); //Cycle through indicator LEDs
        digitalWrite(LEDB, LOW);
        digitalWrite(LEDC, LOW);
        break;
      case 1:
        digitalWrite(LEDA, LOW);
        digitalWrite(LEDB, HIGH);
        digitalWrite(LEDC, LOW);
        break;
      case 2:
        digitalWrite(LEDA, LOW);
        digitalWrite(LEDB, LOW);
        digitalWrite(LEDC, HIGH);
        break;
    }
    if(DEBUGMODE == 1) {
      Serial.print("Button Press! Mode: ");
      Serial.println(buttonState);
    }
  }
  last_interrupt_time = interrupt_time;
}

void setup() {
  Serial.begin(9600);
  
  pinMode(BUTTONPIN, INPUT);
  pinMode(LEDA, OUTPUT);
  pinMode(LEDB, OUTPUT);
  pinMode(LEDC, OUTPUT);
  
  pinMode(LLDRPIN, INPUT);
  pinMode(MLDRPIN, INPUT);
  pinMode(RLDRPIN, INPUT);
  pinMode(LCDPIN, OUTPUT);
  
  attachInterrupt(BUTTONPIN, buttonPress, RISING); //Monitor BUTTONPIN as an interrupt pin

  switch(buttonState) {
      case 0:
        digitalWrite(LEDA, HIGH); //Cycle through indicator LEDs
        digitalWrite(LEDB, LOW);
        digitalWrite(LEDC, LOW);
        break;
      case 1:
        digitalWrite(LEDA, LOW);
        digitalWrite(LEDB, HIGH);
        digitalWrite(LEDC, LOW);
        break;
      case 2:
        digitalWrite(LEDA, LOW);
        digitalWrite(LEDB, LOW);
        digitalWrite(LEDC, HIGH);
        break;
    }
  
}

void loop() {
  lLDRval = analogRead(LLDRPIN);
  mLDRval = analogRead(MLDRPIN);
  rLDRval = analogRead(RLDRPIN);
  LDRavr = max3(lLDRval, mLDRval, rLDRval); //Pick the highest out of the three LDR readings //int((lLDRval+mLDRval+rLDRval)/3); //Read and average the LDR values

  updateMovingAverage();
  
  if(DEBUGMODE == 1) {
    Serial.print("L Val: ");
    Serial.print(lLDRval);
    Serial.print(" | M Val: ");
    Serial.print(mLDRval);
    Serial.print(" | R Val: ");
    Serial.print(rLDRval);
    Serial.print(" | Moving average: ");
    Serial.print(LDRmoving);
    Serial.print(" | LCD Output: ");
    Serial.println(easeMap(LDRmoving));
  }
  
  analogWrite(LCDPIN, easeMap(LDRmoving)); //Change LCD according to output of mapping function

  delay(1); //Set delay to 200 for the moving average to average across 1s of time (5 * 200 = 1000 ms)
}
