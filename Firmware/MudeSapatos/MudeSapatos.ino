#include <SoftwareSerial.h>

#define ANALOGPORTS 6

int analogPorts[]       = { A0, A1, A2, A3, A4, A5 };
float lowPassAnalog[]   = { 0, 0, 0, 0, 0, 0 };
float sensitivityAnalog = 0.2f;
float thresholdAnalog[] = { 1.3f, 1.3f, 1.15f, 1.3f, 1.3f, 1.3f };
int   debounceTimeMax   = 100;
int   debounceMillis[]  = { 0, 0, 0, 0, 0, 0 };
int64_t lastMillis      = 0;

void setup()
{
  Serial.begin(9600);

  noteOn(0x90, 0x1E, 0x45);
  delay(100);
  //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
  noteOn(0x90, 0x1E, 0x00);
  delay(100);
  
  for(int i=0; i<ANALOGPORTS; i++)
  {
    pinMode(analogPorts[i], INPUT);
    lowPassAnalog[i] = analogRead(analogPorts[i]);
  }
  lastMillis = millis();
}

void loop()
{ 
  int64_t currentMillis = millis();
  int deltaMillis = abs(currentMillis - lastMillis);
  lastMillis = currentMillis;
  
  for(int i=0; i<ANALOGPORTS; i++)
  {
    int currentRead = analogRead(analogPorts[i]);
    lowPassAnalog[i] = lowPassAnalog[i] * (1 - sensitivityAnalog) + sensitivityAnalog * currentRead;
    debounceMillis[i] = max(0, debounceMillis[i] - deltaMillis);
    if(currentRead > (thresholdAnalog[i] * lowPassAnalog[i]) && debounceMillis[i] == 0)
    {
      noteOn(0x90, 0x1E + i*2, 0x45);
      delay(100);
      //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
      noteOn(0x90, 0x1E + i*2, 0x00);
      delay(100);
      debounceMillis[i] = debounceTimeMax;
      //debounceMillis[(i%2==0?i+1:i-1)] = debounceTimeMax;
//      Serial.print("Movement detected:");
//      Serial.println(i);
    }
  }
}

void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

