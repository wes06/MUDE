#include <Adafruit_NeoPixel.h>

#define ANALOGPORTS 6
#define CHANNEL     0x90 //channel 1
#define MUTE        0x00
#define LOUD        0x45


#define PIN            6
#define NUMPIXELS      16
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int analogPorts[]       = { A0, A1, A2, A3, A4, A5 };
float lowPassAnalog[]   = { 0, 0, 0, 0, 0, 0 };
float sensitivityAnalog = 0.5f;
float thresholdAnalog[] = { 0.15f, 0.3f, 0.15f, 0.3f, 0.15f, 0.3f };
int   debounceTimeMax   = 100;
int   debounceMillis[]  = { 0, 0, 0, 0, 0, 0 };
int   midiTargetNotes[] = { 0x5A, 0x55, 0x4A, 0x45, 0x3A, 0x35 };
int   midiTargetVeloc[] = { MUTE, MUTE, MUTE, MUTE, MUTE, MUTE };
int   midiCurrenVeloc[] = { MUTE, MUTE, MUTE, MUTE, MUTE, MUTE };
int64_t lastMillis      = 0;

void setup()
{
  Serial.begin(9600);
  
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
    if(abs(lowPassAnalog[i] - currentRead) > (thresholdAnalog[i] * lowPassAnalog[i]) && debounceMillis[i] == 0)
    {
      midiTargetVeloc[i] = LOUD;
      debounceMillis[i] = debounceTimeMax;
      debounceMillis[(i%2==0?i+1:i-1)] = debounceTimeMax;
      pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
    }
    else if(debounceMillis[i] == 0)
    {
      midiTargetVeloc[i] = MUTE;
    }
    if(midiCurrenVeloc[i] != midiTargetVeloc[i])
    {
      noteOn(CHANNEL, midiTargetNotes[i], midiTargetVeloc[i]);
      midiCurrenVeloc[i] = midiTargetVeloc[i];
    }
  }
  pixels.show();
}

void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

