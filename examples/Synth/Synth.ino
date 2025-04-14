extern unsigned long _heap_start;
extern unsigned long _heap_end;
extern char *__brkval;
int freeram() {
  return (char *)&_heap_end - __brkval;
}

#include <Audio.h> 
#include "NoiseMaker.h"
#include <Wire.h>
#include <SPI.h>

AudioControlSGTL5000 sgtl5000_1;
AudioOutputI2S i2s1;
NoiseMaker noisemaker(true);
AudioConnection patchCord1(noisemaker, 0, i2s1, 0);
AudioConnection patchCord2(noisemaker, 1, i2s1, 1);

#include <USBHost_t36.h> // access to USB MIDI devices (plugged into 2nd USB port)
USBHost myusb;
USBHub hub1(myusb);
MIDIDevice midi1(myusb);

const unsigned int nPatches = sizeof(programs)/sizeof(programs[0]);
bool debug=true;
unsigned int patch=0;

float pitchPitch = 0;
float pitchCutoff = 0;

const uint8_t CC_CUTOFF = 1;
const uint8_t CC_VOLUME = 7;
const uint8_t PROGRAM_NEXT = 0x63;
const uint8_t PROGRAM_PREV = 0x62;

const uint8_t NOTE_ON = 0x90;
const uint8_t NOTE_OFF = 0x80;
const uint8_t CONTROL_CHANGE = 0xb0;
const uint8_t PROGRAM_CHANGE = 0xc0;
const uint8_t PITCH_BEND = 0xe0;
const uint8_t ACTIVE_SENSE = 0xfe;

void setProgram(unsigned int newPatch) {
	patch = newPatch % nPatches;
	AudioNoInterrupts();
	noisemaker.setProgram(patch);
	noisemaker.engine->setPitchwheelPitch(pitchPitch);
	noisemaker.engine->setPitchwheelCutoff(pitchCutoff);
	AudioInterrupts();
	Serial.printf("Set Program %d: %s\n", patch, programs[patch].name);
}

void synthMessage(uint8_t type, uint8_t data1, uint8_t data2) {
	if (type == ACTIVE_SENSE)
		return;

	SynthEngine * engine = noisemaker.engine;

	switch (type & 0xF0) {

	case NOTE_ON:
		if (debug) Serial.printf("NOTE ON: %02x\n", data1);
		if (data1 == PROGRAM_NEXT) {
			setProgram(patch+1);
		} else if (data1 == PROGRAM_PREV) {
			setProgram(patch-1);
		} else {
			AudioNoInterrupts();
			engine->setNoteOn(data1, data2/127.0);
			AudioInterrupts();
		}
		break;

	case NOTE_OFF:
		if (debug) Serial.printf("NOTE OFF: %02x\n", data1);
		AudioNoInterrupts();
		engine->setNoteOff(data1);
		AudioInterrupts();
		break;

	case CONTROL_CHANGE:
		if (debug) Serial.printf("CONTROL CHANGE: %02x %02x\n", data1, data2);
		AudioNoInterrupts();
		if (data1 == CC_CUTOFF) {
			pitchCutoff = data2/127.0;
			engine->setPitchwheelCutoff(pitchCutoff);	/* 0 .. 1 */
		} else if (data1 == CC_VOLUME)
			engine->setVolume(data2/127.0); 			/* 0 .. 1 */
		AudioInterrupts();
		break;

	case PROGRAM_CHANGE:
		if (debug) Serial.printf("PROGRAM CHANGE: %02x\n", data1);
		setProgram(data1);
		break;

	case PITCH_BEND:
		if (debug) Serial.printf("PITCH BEND: %02x %02x\n", data1, data2);
		int bend = data1 | (data2 << 7);  // 14 bits -> 0 .. 16383
		AudioNoInterrupts();
		pitchPitch = 2*(bend/16383.0-0.5);
		engine->setPitchwheelPitch(pitchPitch);
		AudioInterrupts();
		break;

	default:
		if (debug) Serial.printf("Unhandled message type %02x: data=%02x,%02x\n", type, data1, data2);
	};
	if (debug) Serial.printf("CPU USAGE %.2f%%\n", AudioProcessorUsage());
}

void readMIDI() {
	/* read MIDI messages from Host port */
	if (midi1.read()) {
		uint8_t type = midi1.getType();
		uint8_t data1 = midi1.getData1();
		uint8_t data2 = midi1.getData2();
		synthMessage(type, data1, data2);
	}
	/* read MIDI messages from Client port */
	if (midi.read()) {
		uint8_t type = midi.getType();
		uint8_t data1 = midi.getData1();
		uint8_t data2 = midi.getData2();
		synthMessage(type, data1, data2);
	}
}

void setup(void) {
    Serial.begin(115200);
	while (!Serial && millis() < 5000) delay(100);
	Serial.println("Setup begin");
	Serial.printf("Initial ram: %d\n", freeram());

	AudioMemory(10);
	sgtl5000_1.enable();
	sgtl5000_1.volume(0.8);

	SynthEngine * engine = noisemaker.engine;
	AudioNoInterrupts();
	noisemaker.setProgram(patch);
	engine -> setNumberOfVoices(6);
	engine -> setPitchwheelAmount(1);
	AudioInterrupts();

	Serial.println("Setup USB...");
	myusb.begin();

	Serial.println("Setup done");
	Serial.printf("Final ram: %d\n", freeram());
}

void loop(void) {
	readMIDI();
}
