//-----------------------------------------------------------

#ifndef noisemaker_h_
#define noisemaker_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"
#include <Audio.h>

#include "./Engine/SynthEngine.h"
#include "./Engine/Lfo.cpp"

struct Program {
	const char * name;
	float data[NUMPARAM];
};

#include "programs.h"

class NoiseMaker : public AudioStream
{
public:
	SynthEngine * engine;
	int currentVoices = 0;
	int bpm = 100;
	/* scale engine floats -> audio sample int16_t */
	int scale = 16383;

	NoiseMaker(bool withReverb=true) : AudioStream(0, NULL) {
		engine = new SynthEngine(AUDIO_SAMPLE_RATE, withReverb);
	}

	~NoiseMaker() {
		delete engine;
	}

	virtual void update(void) {
		audio_block_t * blockL = allocate();
		if (blockL == NULL) return;
		audio_block_t * blockR = allocate();
		if (blockR == NULL) { release(blockL); return; }

		for (int i=0; i<AUDIO_BLOCK_SAMPLES; i++) {
			float sampleL, sampleR;
			engine->process(&sampleL, &sampleR);
			blockL->data[i] = sampleL * scale;
			blockR->data[i] = sampleR * scale;
		}

		transmit(blockL, 0);
		release(blockL);
		transmit(blockR, 1);
		release(blockR);
  	}
	
	float preset[NUMPARAM];
	
	float getParameter(int index) {
		if (index < NUMPARAM)
			return preset[index];
		else
			return 0;
	}
	
	void setParameter(int index, float newValue) {
		if (index < NUMPARAM) {
			preset[index] = newValue;
			switch(index) {
			case VOLUME:
				engine->setVolume(newValue);
				break;
			case CUTOFF:
				engine->setCutoff(newValue);
				break;
			case RESONANCE:
				engine->setResonance(newValue);
				break;
			case FILTERCONTOUR:
				engine->setFilterContour(newValue);
				break;
			case KEYFOLLOW:
				engine->setKeyfollow(newValue);
				break;
			case FILTERATTACK:
				engine->setFilterAttack(newValue);
				break;
			case FILTERDECAY:
				engine->setFilterDecay(newValue);
				break;
			case FILTERSUSTAIN:
				engine->setFilterSustain(newValue);
				break;
			case FILTERRELEASE:
				engine->setFilterRelease(newValue);
				break;
			case AMPATTACK:
				engine->setAmpAttack(newValue);
				break;
			case AMPDECAY:
				engine->setAmpDecay(newValue);
				break;
			case AMPSUSTAIN:
				engine->setAmpSustain(newValue);
				break;
			case AMPRELEASE:
				engine->setAmpRelease(newValue);
				break;
			case OSC1VOLUME:
				engine->setOsc1Volume(newValue);
				break;
			case OSC2VOLUME:
				engine->setOsc2Volume(newValue);
				break;
			case OSC3VOLUME:
				engine->setOsc3Volume(newValue);
				break;
			case OSC1WAVEFORM:
				engine->setOsc1Waveform(newValue);
				break;
			case OSC2WAVEFORM:
				engine->setOsc2Waveform(newValue);
				break;
			case OSC1TUNE:
				engine->setOsc1Tune(newValue);
				break;
			case OSC2TUNE:
				engine->setOsc2Tune(newValue);
				break;
			case OSC1FINETUNE:
				engine->setOsc1FineTune(newValue);
				break;
			case OSC2FINETUNE:
				engine->setOsc2FineTune(newValue);
				break;
			case OSCSYNC:
				engine->setOscSync(newValue > 0.0f);
				break;
			case PANIC:
				engine->setPanic();
				break;
			case PORTAMENTO:
				engine->setPortamento(newValue);
				break;
			case PORTAMENTOMODE:
				engine->setPortamentoMode(newValue);
				break;
			case LFO1RATE:
				engine->setLfo1Rate(newValue, this->bpm);
				break;
			case LFO2RATE:
				engine->setLfo2Rate(newValue, this->bpm);
				break;
			case LFO1AMOUNT:
				engine->setLfo1Amount(newValue);
				break;
			case LFO2AMOUNT:
				engine->setLfo2Amount(newValue);
				break;
			case LFO1WAVEFORM:
				engine->setLfo1Waveform(newValue);
				break;
			case LFO2WAVEFORM:
				engine->setLfo2Waveform(newValue);
				break;
			case LFO1DESTINATION:
				engine->setLfo1Destination(newValue);
				break;
			case LFO2DESTINATION:
				engine->setLfo2Destination(newValue);
				break;
			case OSC1PW:
				engine->setOsc1Pw(newValue);
				break;
			case OSC1PHASE:
				engine->setOsc1Phase(newValue);
				break;
			case OSC2FM:
				engine->setOsc1Fm(newValue);
				break;
			case OSC2PHASE:
				engine->setOsc2Phase(newValue);
				break;
			case FREEADATTACK:
				engine->setFreeAdAttack(newValue);
				break;
			case FREEADDECAY:
				engine->setFreeAdDecay(newValue);
				break;
			case FREEADAMOUNT:
				engine->setFreeAdAmount(newValue);
				break;
			case FREEADDESTINATION:
				engine->setFreeAdDestination(newValue);
				break;
			case LFO1SYNC:
				engine->setLfo1Sync(newValue, getParameter(LFO1RATE), this->bpm);
				break;
			case LFO1KEYTRIGGER:
				engine->setLfo1KeyTrigger(newValue);
				break;
			case LFO1PHASE:
				engine->setLfo1Phase(newValue);
				break;
			case LFO2SYNC:
				engine->setLfo2Sync(newValue, getParameter(LFO2RATE), this->bpm);
				break;
			case LFO2KEYTRIGGER:
				engine->setLfo2KeyTrigger(newValue);
				break;
			case LFO2PHASE:
				engine->setLfo2Phase(newValue);
				break;
			case VELOCITYVOLUME:
				engine->setVelocityVolume(newValue);
				break;
			case VELOCITYCONTOUR:
				engine->setVelocityContour(newValue);
				break;
			case VELOCITYCUTOFF:
				engine->setVelocityCutoff(newValue);
				break;
			case PITCHWHEELCUTOFF:
				engine->setPitchwheelCutoff(newValue);
				break;
			case PITCHWHEELPITCH:
				engine->setPitchwheelPitch(newValue);
				break;
			case HIGHPASS:
				engine->setHighPass(newValue);
				break;
			case DETUNE:
				engine->setDetune(newValue);
				break;
			case OSCMASTERTUNE:
				engine->setMastertune(newValue);
				break;
			case TRANSPOSE:
				engine->setTranspose(newValue);
				break;
			case RINGMODULATION:
				engine->setRingmodulation(newValue);
				break;
			case CHORUS1ENABLE:
				engine->setChorus(newValue > 0.0f, getParameter(CHORUS2ENABLE) > 0.0f);
				break;
			case CHORUS2ENABLE:
				engine->setChorus(getParameter(CHORUS1ENABLE) > 0.0f, newValue > 0.0f);
				break;
			case REVERBWET:
				engine->setReverbWet(newValue);
				break;
			case REVERBDECAY:
				engine->setReverbDecay(newValue);
				break;
			case REVERBPREDELAY:
				engine->setReverbPreDelay(newValue);
				break;
			case REVERBHIGHCUT:
				engine->setReverbHighCut(newValue);
				break;
			case REVERBLOWCUT:
				engine->setReverbLowCut(newValue);
				break;
			case OSCBITCRUSHER:
				engine->setOscBitcrusher(newValue);
				break;
			case FILTERTYPE:
				engine->setFiltertype(newValue);
				break;
			}
		}
	}

	void resetProgram(void) {
		for (int i=VOLUME; i<=DETUNE; i++)
			setParameter(i, getParameter(i));
		engine->reset();
	}

	boolean syncNumberOfVoices(int nVoices) {
		if (nVoices != currentVoices) {
			currentVoices = nVoices;
			engine->setNumberOfVoices(nVoices);
			return true;
		}
		return false;
	}

	void setNumberOfVoices(int nVoices) {
		if (syncNumberOfVoices(nVoices))
			resetProgram();
	}

	void setProgram(const struct Program & p, int nVoices=0) {
		syncNumberOfVoices(nVoices ? nVoices : p.data[VOICES-VOLUME]);
		for (int i=VOLUME; i<=DETUNE; i++)
			setParameter(i, p.data[i-VOLUME]);
		engine->reset();
	}

	void setProgram(unsigned int program, int nVoices=0) {
		if (program >= sizeof(programs)/sizeof(programs[0]))
			return;
		setProgram(programs[program], nVoices);
	}  
};

#endif
