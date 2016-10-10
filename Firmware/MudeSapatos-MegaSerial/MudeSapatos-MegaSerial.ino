#include <Adafruit_NeoPixel.h>

#define ANALOGPORTS 12
#define CHANNEL     0x90 //channel 1
#define MUTE        0x00
#define LOUD        0x45


#define PIN            6
#define NUMPIXELS      6
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int analogPorts[]       = {
                          // Y     Z (checar)
                             A0,   A1, 
                             A2,   A3,
                             A4,   A5,
                             A8,   A9,
                             A10,  A11,
                             A12,  A13,
                            };


float lowPassAnalog[]   = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float sensitivityAnalog = 0.5f;

float yTh = 0.15f;
float zTh = 0.3f;
float thresholdAnalog[] = { yTh, zTh, yTh, zTh, yTh, zTh, yTh, zTh, yTh, zTh, yTh, zTh,};

int   debounceTimeMax   = 100;
int   debounceMillis[]  = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int   midiTargetNotes[] = { 
                            0x8A, 0x85,
                            0x7A, 0x75,
                            0x6A, 0x65,
                            0x5A, 0x55, 
                            0x4A, 0x45, 
                            0x3A, 0x35 
                          };
int   midiTargetVeloc[] = { MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE };
int   midiCurrenVeloc[] = { MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE };
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
  Serial.println("\tBOOT");
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
  Serial.print("\t");
  Serial.print(pitch);
  Serial.print(":\t");
  Serial.print(velocity);
  Serial.print("\r\n");
  //Serial.write(cmd);
  //Serial.write(pitch);
  //Serial.write(velocity);
}

