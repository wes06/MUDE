//#include <Adafruit_NeoPixel.h>

#include <MIDI.h>

#define ANALOGPORTS 6
//#define ANALOGPORTS 12
#define CHANNEL     0x90 //channel 1
#define MUTE        0x00
#define LOUD        0x7F




MIDI_CREATE_DEFAULT_INSTANCE();


#define PIN            6
#define NUMPIXELS      6
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int analogPorts[]       = { A0, A2, A4, A8, A10, A12 };
// Y     Z (checar)
//                             A0,   A1,
//                             A2,   A3,
//                             A4,   A5,
//                             A8,   A9,
//                             A10,  A11,
//                             A12,  A13,
//                            };


float lowPassAnalog[]   = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float sensitivityAnalog = 0.5f;

float yTh = 0.15f;
float zTh = 45;
float thresholdAnalog[] = { zTh, zTh, zTh, zTh, zTh, zTh };
//{ yTh, zTh, yTh, zTh, yTh, zTh, yTh, zTh, yTh, zTh, yTh, zTh,};

int   debounceTimeMax   = 100;
int   debounceMillis[]  = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int   midiTargetNotes[] = { 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 };
//                            0x8A, 0x18,
//                            0x7A, 0x19,
//                            0x6A, 0x1A,
//                            0x5A, 0x1B,
//                            0x4A, 0x1C,
//                            0x3A, 0x1D
//                          };
int   midiTargetVeloc[] = { MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE };
int   midiCurrenVeloc[] = { MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE };
int64_t lastMillis      = 0;

void setup()
{

  MIDI.begin(MIDI_CHANNEL_OMNI);
  //Serial.begin(9600);
  //Serial.println("Start");

  for (int i = 0; i < ANALOGPORTS; i++)
  {
    pinMode(analogPorts[i], INPUT);
    lowPassAnalog[i] = analogRead(analogPorts[i]);
  }
  lastMillis = millis();
  //Serial.println("\tBOOT");
  pinMode(13, OUTPUT);
}

void loop()
{
  int64_t currentMillis = millis();
  int deltaMillis = abs(currentMillis - lastMillis);
  lastMillis = currentMillis;

  for (int i = 0; i < ANALOGPORTS; i++)
  {
    int currentRead = analogRead(analogPorts[i]);
    lowPassAnalog[i] = lowPassAnalog[i] * (1 - sensitivityAnalog) + sensitivityAnalog * currentRead;
    debounceMillis[i] = max(0, debounceMillis[i] - deltaMillis);

    if (abs(lowPassAnalog[i] - currentRead) > thresholdAnalog[i] && debounceMillis[i] == 0)
    {
      midiTargetVeloc[i] = LOUD;
      debounceMillis[i] = debounceTimeMax;
      //pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.

      //pixels.setPixelColor(i, corAceso[i]);
      digitalWrite(13, HIGH);
    }
    else if (debounceMillis[i] == 0)
    {
      //pixels.setPixelColor(i, corApagado[i]);
      digitalWrite(13, LOW);
      midiTargetVeloc[i] = MUTE;
    }

    if (midiCurrenVeloc[i] != midiTargetVeloc[i])
    {
      noteOn(CHANNEL, midiTargetNotes[i], midiTargetVeloc[i]);
      midiCurrenVeloc[i] = midiTargetVeloc[i];
    }
  }

  // pixels.show();
}

void noteOn(int cmd, int pitch, int velocity) {
  //  Serial.print("\t");
  //  Serial.print(pitch);
  //  Serial.print(":\t");
  //  Serial.print(velocity);
  //  Serial.print("\r\n");

  //Serial.write(cmd);
  //Serial.write(pitch);
  //Serial.write(velocity);

  MIDI.sendNoteOn(pitch, velocity, 1);
}

