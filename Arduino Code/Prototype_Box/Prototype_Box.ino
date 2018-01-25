#include <SoftwareSerial.h>
SoftwareSerial DebugSerial(2, 3); // RX, TX

#define BLYNK_PRINT DebugSerial

#include <BlynkSimpleSerialBLE.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "70f3f34d793f48cdba4ca645d114484f";

void setup()
{
  // Debug console
  DebugSerial.begin(9600);
  DebugSerial.println("Waiting for connections...");

  // Blynk will work through Serial
  // Do not read or write this serial manually in your sketch
  Serial.begin(115200);
  Blynk.begin(Serial, auth);
}

void loop()
{
  Blynk.run();
}
