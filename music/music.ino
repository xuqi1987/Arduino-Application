#include "pitches.h"
// notes in the melody:
int melody1[] = {
NOTE_C4, NOTE_G3,NOTE_G3, NOTE_GS3, NOTE_G3,0, NOTE_B3, NOTE_C4};
// note durations: 4 = quarter note, 8 = eighth note, etc.:

int melody[] = {
NOTE_C3, NOTE_C3,NOTE_C4, NOTE_C5, NOTE_C5,NOTE_C4, NOTE_C3, NOTE_C2};

int noteDurations[] = {
   4, 4, 4, 4,4,4,4,4 
};

void setup() {
   // iterate over the notes of the melody:
   for (int thisNote = 0; thisNote < 8; thisNote++) {
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000/noteDurations[thisNote];
      tone(8, melody[thisNote],noteDuration);
      //pause for the note's duration plus 30 ms:
      delay(noteDuration +30);
   }
}
void loop() {
   // no need to repeat the melody.
}
