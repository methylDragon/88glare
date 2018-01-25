const int MINPWM = 0;
const int MAXPWM = 130;
int PWMvar;

void setup(){
  Serial.begin(9600);
  pinMode(11, OUTPUT);
}

void loop(){
  PWMvar = MAXPWM - MINPWM;

  for (int i = 0; i <20; i++){
    analogWrite(11,(PWMvar*i)/20 + MINPWM);
    Serial.println(PWMvar*i/20 + MINPWM);
    delay(3000);
  }

  delay(3000);
  Serial.println("LOOPING");

}
