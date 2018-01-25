//REMEMBER TO SET ENDLINE AS NEWLINE CHARACTER IN SERIAL MONITOR TO HAVE THIS WORK

const int PWMpin = 3;
 
String inputString = ""; // a string to hold incoming data
boolean stringComplete = false; // whether the string is complete

void setup() {
  pinMode(PWMpin, OUTPUT);
  Serial.begin(9600);
  inputString.reserve(200);
}

void loop() {
  if (stringComplete) {
    analogWrite(PWMpin, inputString.toInt());
    Serial.print("Writing PWM: ");
    Serial.print(inputString);
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
    while (Serial.available()) {
      char inChar = (char)Serial.read(); 
      inputString += inChar;
      if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}
