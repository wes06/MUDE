#include <Adafruit_NeoPixel.h>
#include <MIDI.h>

#define ANALOGPORTS 6
#define CHANNEL     0x90 //channel 1
#define MUTE        0x00
#define LOUD        0x7F

MIDI_CREATE_DEFAULT_INSTANCE();

#define PIN            53
#define NUMPIXELS      4
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400);



uint32_t cOn[6][4] = {
  {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
  },
  {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF
  },
  {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF
  },
  {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF
  },
  {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF
  },
  {
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF,
    0xFFFFFFFF
  },
};

uint32_t cOff[] = {
  0x05050505,
  0x05050505,
  0x05050505,
  0x05050505,
  0x05050505,
  0x05050505
};

int analogPorts[]       = { A0, A2, A4, A8, A10, A12 };

float lowPassAnalog[]   = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
float sensitivityAnalog = 0.5f;

float yTh = 0.15f;
float zTh = 45;
float thresholdAnalog[] = { zTh, zTh, zTh, zTh, zTh, zTh };

int   debounceTimeMax   = 100;
int   debounceMillis[]  = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int   debounceLEDTimeMax   = 150;
int   debounceLedMillis[]  = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int   midiTargetNotes[] = { 0x24, 0x25, 0x26, 0x27, 0x28, 0x29 };

int   midiTargetVeloc[] = { MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE };
int   midiCurrenVeloc[] = { MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE, MUTE };
int64_t lastMillis      = 0;

void setup()
{

  MIDI.begin(MIDI_CHANNEL_OMNI);
  //Serial.begin(9600);
  //Serial.println("Start");
  pixels.begin();
  pixels.setBrightness(255);

  for (int i = 0; i < ANALOGPORTS; i++)
  {
    pinMode(analogPorts[i], INPUT);
    lowPassAnalog[i] = analogRead(analogPorts[i]);
  }

  lastMillis = millis();
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
    debounceLedMillis[i] = max(0, debounceLedMillis[i] - deltaMillis);

    if (abs(lowPassAnalog[i] - currentRead) > thresholdAnalog[i] && debounceMillis[i] == 0)
    {
      midiTargetVeloc[i] = LOUD;
      debounceMillis[i] = debounceTimeMax;
      debounceLedMillis[i] = debounceLEDTimeMax;

      for (int j = 0; j < NUMPIXELS; j++) {
        pixels.setPixelColor(j, cOn[i][j]);
      }
      digitalWrite(13, HIGH);
    }
    else if (debounceMillis[i] == 0)
    {
      midiTargetVeloc[i] = MUTE;
    }

    if (debounceLedMillis[i] == 0) {
      for (int j = 0; j < NUMPIXELS; j++) {
        pixels.setPixelColor(j, cOff[i]);
      }
      digitalWrite(13, LOW);
    }

    if (midiCurrenVeloc[i] != midiTargetVeloc[i])
    {
      noteOn(CHANNEL, midiTargetNotes[i], midiTargetVeloc[i]);
      midiCurrenVeloc[i] = midiTargetVeloc[i];
    }
  }

  pixels.show();
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
