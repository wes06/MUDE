#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE();

void setup() 
{
    MIDI.begin(MIDI_CHANNEL_OMNI);
    pinMode(13, OUTPUT);


}

void loop() 
{
  digitalWrite(13, HIGH);
  for (int i = 0; i < 24; i++) 
  {
    MIDI.sendNoteOn(i, i*2, 1); 
    delay(500); 
  }

  // Delay == Bad! Set up an interrupt based timer instead 
  // (or something like Blink Without Delay)
  delay(1000); 
digitalWrite(13, LOW);
delay(1000); 
}
