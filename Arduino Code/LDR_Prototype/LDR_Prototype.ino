const int threshold = 175; //LDR threshold (We found that with a fully exposed LDR and a bright source of light placed a reasonable distance away, we get 650 with a 10k resistor)

//State input and output pins
const int leftLDR = A1;
const int leftLED = 6;
const int rightLDR = A2;
const int rightLED = 7;

const int buzzerPin = 9; //For sound-making
const int switchPin = 8; //For setting persistence mode

//Initialise mode and value variables
const int Delay = 10;
int persistentMode = 0;
int leftValue;
int rightValue;
int lightValue;

//methylSwag
char dragon[] = "                           .     .\n\
                        .  |\\-^-/|  .    \n\
                       /| } O.=.O { |\\  \n\
                      /' \\ \\_ ~ _/ / '\\\n\
                    /' |  \\-/ ~ \\-/  | '\\\n\
                    |   |  /\\\\ //\\  |   | \n\
                     \\|\\|\\/-\"\"-\"\"-\\/|/|/\n\
                             ______/ /\n\
                             '------'";

char methylDragon[] = "                _   _        _  ___                         \n\
      _ __  ___| |_| |_ _  _| ||   \\ _ _ __ _ __ _ ___ _ _  \n\
     | '  \\/ -_)  _| ' \\ || | || |) | '_/ _` / _` / _ \\ ' \\ \n\
     |_|_|_\\___|\\__|_||_\\_, |_||___/|_| \\__,_\\__, \\___/_||_|\n\
                        |__/                |___/          ";

void setup() {
  Serial.begin(9600);

  Serial.println(dragon);
  Serial.println(methylDragon);
  Serial.println("     -------------------------------------------------------");
  Serial.println("                         methylDragon.com\n\n");
  pinMode(leftLDR, INPUT);
  pinMode(rightLDR, INPUT);
  pinMode(switchPin, INPUT);

  pinMode(buzzerPin, OUTPUT);
  pinMode(leftLED, OUTPUT);
  pinMode(rightLED, OUTPUT);

  persistentMode = digitalRead(switchPin);
  delay(500);
  Serial.print("INITIALISED WITH PERSISTENCE MODE: ");
  Serial.print(String(persistentMode));
  Serial.println(" (Changes dynamically)\n");
  delay(500);
}

void loop() {
  leftValue = analogRead(leftLDR);
  rightValue = analogRead(rightLDR);
  persistentMode = digitalRead(switchPin);

  Serial.print("Switch Mode: ");
  Serial.print(digitalRead(switchPin));
  Serial.print(" | ");
  
  switch (persistentMode) {
    case 1: //If persistence mode is ON, then sound plays non-stop

      if ((leftValue + rightValue)/2 > threshold) { //Check if threshold triggered
        tone(buzzerPin, 1800, Delay + 50); //Buzz at maximum frequency
        Serial.print("Tone: "); Serial.print("TRIGGERED"); Serial.print(" | ");
      } else {
        lightValue = map(int(constrain((leftValue + rightValue)/2,0,threshold)),0,threshold,120,1800); //Map average light value to tone #MAXIMUMOBFUSCATION
        tone(buzzerPin, lightValue, Delay + 50);
        Serial.print("Tone: "); Serial.print(lightValue); Serial.print(" | ");
      }
        
      if(leftValue > threshold) { //Check left value
        digitalWrite(leftLED, HIGH);
        Serial.print("Left: "); Serial.print("TRIGGERED"); Serial.print(" | ");
      } else {
        digitalWrite(leftLED, LOW);
        Serial.print("Left: "); Serial.print(leftValue); Serial.print(" | ");
      }

      if(rightValue > threshold) { //Check right value
        digitalWrite(rightLED, HIGH);
        Serial.print("Right: "); Serial.print("TRIGGERED");
      } else {
        digitalWrite(rightLED, LOW);
        Serial.print("Right: "); Serial.print(rightValue);
      }
      
      break;

    case 0: //If persistence mode is OFF, then sound only plays on threshold trigger
      if((leftValue > threshold) || (rightValue > threshold)) { //Check if any thresholds were triggered
        tone(buzzerPin, 1800, Delay + 50);
        Serial.print("Autistic Screeching: "); Serial.print("REEEEEEEEEEE | ");
      } else {
        Serial.print("Tone: "); Serial.print("0 | ");
        noTone(buzzerPin);
      }

      if(leftValue > threshold) { //Check left value
        digitalWrite(leftLED, HIGH);
        Serial.print("Left: "); Serial.print("TRIGGERED"); Serial.print(" | ");
      } else {
        digitalWrite(leftLED, LOW);
        Serial.print("Left: "); Serial.print(leftValue); Serial.print(" | ");
      }

      if(rightValue > threshold) { //Check right value
        digitalWrite(rightLED, HIGH);
        Serial.print("Right: "); Serial.print("TRIGGERED");
      } else {
        digitalWrite(rightLED, LOW);
        Serial.print("Right: "); Serial.print(rightValue);
      }
      
      break;    
  }
  delay(Delay);
  Serial.print("\n");
}
