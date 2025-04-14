extern unsigned long _heap_start;
extern unsigned long _heap_end;
extern char *__brkval;
int freeram() {
  return (char *)&_heap_end - __brkval;
}
extern float tempmonGetTemp(void);
extern "C" uint32_t set_arm_clock(uint32_t frequency);

#include <Audio.h> 
#include "NoiseMaker.h"
#include <Wire.h>
#include <SPI.h>

AudioControlSGTL5000  sgtl5000_1;
AudioOutputI2S        i2s1;
NoiseMaker            noisemaker(true);
AudioConnection       patchCord1(noisemaker, 0, i2s1, 0);
AudioConnection       patchCord2(noisemaker, 1, i2s1, 1);

#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0]))

/* number of patches to test, default=all */
const unsigned nPatches = ARRAY_LEN(programs);


/* CPU frequencies to test, >600MHz is overclocked */
unsigned testFrequencies[] = { 600, 720, 816, 912 };
unsigned testVoiceMax[] =    { 4,   5,   5,   6 };

/* period (ms) for evaluation of CPU usage */
unsigned period = 1000;

/* set nVoices > 0 to overide the default number of voices for programs */
const int nVoices=6;

/* limit the number of voices to avoid overloading the CPU */
const boolean avoidOverload=true;

/* get maximum length of program name, for column formatting */

unsigned maxProgramNameLength;

unsigned getMaxProgramNameLength(void) {
	unsigned len = 0;
	for (unsigned i=0; i<nPatches; i++)
		len = max(len, strlen(programs[i].name));
	return len;
}

void tab(int spaces) {
	while (spaces > 0) {
		Serial.printf(" ");
		--spaces;
	}
}

/* print header for the benchmark CSV data */

int voiceLimit=0;

void printCSVHeader(void) {
	Serial.printf("%3s, %s,", "#", "\"Patch Name\"");
	tab(maxProgramNameLength-8);
	Serial.printf("V, %5s,", "temp");
	unsigned v = nVoices ? nVoices : 6;
	if (avoidOverload)
		v = min(v, voiceLimit);
	for (unsigned i=0; i<v; i++) {
		if (i) Serial.printf(",");
		Serial.printf("   N=%d", i+1);
	}
	Serial.printf("\n");
}

/* set the next CPU frequency for testing */

void setNextFreq(void) {
	static int freqStep = 0;
	voiceLimit = testVoiceMax[freqStep];
	set_arm_clock(testFrequencies[freqStep]*1000000);
	Serial.printf("\nF_CPU_ACTUAL: %d\n", F_CPU_ACTUAL);
	delay(period);
	freqStep = (freqStep+1) % ARRAY_LEN(testFrequencies);
	printCSVHeader();
}

void setup(void) {
    Serial.begin(115200);
	while (!Serial && millis() < 5000) delay(100);

	Serial.println("--- Setup begin");
	Serial.printf(" Initial ram: %d\n", freeram());
	Serial.printf(" F_CPU_ACTUAL: %d\n", F_CPU_ACTUAL);

	noisemaker.setProgram(0);

	AudioMemory(10);
	sgtl5000_1.enable();
	sgtl5000_1.volume(0.7);

	Serial.printf(" Final ram: %d\n", freeram());
	maxProgramNameLength = getMaxProgramNameLength();
	Serial.println("--- Setup done");
	setNextFreq();
}

unsigned patch=0;

/* loop runs once to test a patch for variable number of voices */

void loop(void) {
	/* set new program, 0 <= patch < nPatches */
	int programVoices = (int) noisemaker.getParameter(VOICES);
	int voices = nVoices ? nVoices : programVoices;
	if (avoidOverload)
		voices = min(voices, voiceLimit);
	AudioNoInterrupts();
	noisemaker.setProgram(patch, voices);
	AudioInterrupts();

	/* print patch information (name, nVoicecs, temperature) */
	const char * name = programs[patch].name;
	Serial.printf("%3d, \"%s\", ", patch, name);
	tab(maxProgramNameLength - strlen(name));
	Serial.printf("%2d, %5.1f", programVoices, tempmonGetTemp());

	/* wait for synth to stabilise before beginning */
	delay(period);

	/* successively add notes up to current number of voices */
	for (int i=0; i<noisemaker.currentVoices; ++i) {
		AudioNoInterrupts();
		AudioProcessorUsageMaxReset();
		noisemaker.engine->setNoteOn(64+3*i, 0.5);
		AudioInterrupts();

		delay(period);
		Serial.printf(", %5.1f", AudioProcessorUsageMax());
	}
	Serial.printf("\n");

	/* now clear all notes */
	AudioNoInterrupts();
	for (int i=0; i<noisemaker.currentVoices; ++i)
		noisemaker.engine->setNoteOff(64+3*i);
	AudioInterrupts();
	delay(period);

	/* advance to next patch */	
	patch = (patch+1)%nPatches;
	if (patch == 0)
		setNextFreq();
}

