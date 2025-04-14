#include <Audio.h> 
#include <NoiseMaker.h>

AudioControlSGTL5000 sgtl5000;        // SGTL5000 controller for Teensy Audio 4 board
NoiseMaker noisemaker(true);          // NoiseMaker synth with reverb enabled
AudioOutputI2S i2s1;                  // Audio output sink
AudioConnection patchCord1(noisemaker, 0, i2s1, 0);
AudioConnection patchCord2(noisemaker, 1, i2s1, 1);

void setup(void) {
    Serial.begin(115200);
    noisemaker.setProgram(64);        // select preset "DR Perc Bongo Room" from sound library
    AudioMemory(10);
    sgtl5000.enable();                // start processing audio
    sgtl5000.volume(0.8);   
}

/* Play a two octave chromatic scale */

uint8_t step = 0;
uint8_t note = 0;

void loop(void) {
    AudioNoInterrupts();              // disable audio interrupts to control the noisemaker engine
    SynthEngine * engine = noisemaker.engine;
    if (note)                         // turn of previous note, if any
        engine -> setNoteOff(note);
    note = 64 + step;
    engine -> setNoteOn(note, 1);     // turn on next note. Velocity is in range 0..1
    AudioInterrupts();
    Serial.printf("Note %d\n", note);
    step = (step+1) % 24;             // advance to next note in cycle
    delay(500);
}
