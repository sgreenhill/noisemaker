import fire

import xml.dom.minidom

synthParameters2 = [ "VOLUME", "FILTERTYPE", "CUTOFF", "RESONANCE", "KEYFOLLOW", "FILTERCONTOUR", "FILTERATTACK", "FILTERDECAY", "FILTERSUSTAIN", "FILTERRELEASE", "AMPATTACK", "AMPDECAY", "AMPSUSTAIN", "AMPRELEASE", "OSC1VOLUME", "OSC2VOLUME", "OSC3VOLUME", "OSCMASTERTUNE", "OSC1TUNE", "OSC2TUNE", "OSC1FINETUNE", "OSC2FINETUNE", "OSC1WAVEFORM", "OSC2WAVEFORM", "OSCSYNC", "LFO1WAVEFORM", "LFO2WAVEFORM", "LFO1RATE", "LFO2RATE", "LFO1AMOUNT", "LFO2AMOUNT", "LFO1DESTINATION", "LFO2DESTINATION", "LFO1PHASE", "LFO2PHASE", "OSC2FM", "OSC2PHASE", "OSC1PW", "OSC1PHASE", "TRANSPOSE", "FREEADATTACK", "FREEADDECAY", "FREEADAMOUNT", "FREEADDESTINATION", "LFO1SYNC", "LFO1KEYTRIGGER", "LFO2SYNC", "LFO2KEYTRIGGER", "PORTAMENTO", "PORTAMENTOMODE", "VOICES", "VELOCITYVOLUME", "VELOCITYCONTOUR", "VELOCITYCUTOFF", "PITCHWHEELCUTOFF", "PITCHWHEELPITCH", "RINGMODULATION", "CHORUS1ENABLE", "CHORUS2ENABLE", "REVERBWET", "REVERBDECAY", "REVERBPREDELAY", "REVERBHIGHCUT", "REVERBLOWCUT", "OSCBITCRUSHER", "HIGHPASS", "DETUNE", "VINTAGENOISE", "PANIC", "UNUSED2", "ENVELOPEEDITORDEST1", "ENVELOPEEDITORSPEED", "ENVELOPEEDITORAMOUNT", "ENVELOPEONESHOT", "ENVELOPEFIXTEMPO", "ENVELOPERESET", "TAB1OPEN", "TAB2OPEN", "TAB3OPEN", "TAB4OPEN", "FILTERDRIVE", "DELAYWET", "DELAYTIME", "DELAYSYNC", "DELAYFACTORL", "DELAYFACTORR", "DELAYHIGHSHELF", "DELAYLOWSHELF", "DELAYFEEDBACK" ]

synthParameters1 = [ "VOLUME", "FILTERTYPE", "CUTOFF", "RESONANCE", "KEYFOLLOW", "FILTERCONTOUR", "FILTERATTACK", "FILTERDECAY", "FILTERSUSTAIN", "FILTERRELEASE", "AMPATTACK", "AMPDECAY", "AMPSUSTAIN", "AMPRELEASE", "OSC1VOLUME", "OSC2VOLUME", "OSC3VOLUME", "OSCMASTERTUNE", "OSC1TUNE", "OSC2TUNE", "OSC1FINETUNE", "OSC2FINETUNE", "OSC1WAVEFORM", "OSC2WAVEFORM", "OSCSYNC", "LFO1WAVEFORM", "LFO2WAVEFORM", "LFO1RATE", "LFO2RATE", "LFO1AMOUNT", "LFO2AMOUNT", "LFO1DESTINATION", "LFO2DESTINATION", "LFO1PHASE", "LFO2PHASE", "OSC2FM", "OSC2PHASE", "OSC1PW", "OSC1PHASE", "TRANSPOSE", "FREEADATTACK", "FREEADDECAY", "FREEADAMOUNT", "FREEADDESTINATION", "LFO1SYNC", "LFO1KEYTRIGGER", "LFO2SYNC", "LFO2KEYTRIGGER", "PORTAMENTO", "PORTAMENTOMODE", "VOICES", "VELOCITYVOLUME", "VELOCITYCONTOUR", "VELOCITYCUTOFF", "PITCHWHEELCUTOFF", "PITCHWHEELPITCH", "RINGMODULATION", "CHORUS1ENABLE", "CHORUS2ENABLE", "REVERBWET", "REVERBDECAY", "REVERBPREDELAY", "REVERBHIGHCUT", "REVERBLOWCUT", "OSCBITCRUSHER", "HIGHPASS", "DETUNE" ]

def checkParameters(synthParameters, attributes):
	a = [ i for i in attributes]
	missing = []

	for i in synthParameters:
		if i in attributes:
			a.remove(i)
		else:
			missing.append(i)

	print("Attributes missing:", missing)
	print("Extra attributes:", a)

splineAttributes = [ "isStartPoint", "isEndPoint", "centerPointX", "centerPointY", "controlPointLeftX", "controlPointLeftY", "controlPointRightX", "controlPointRightY" ]

def makeHeader(programs, attributes, synthParameters, version, file=None):
	maxPoints = 0
	if version > 1:
		# output envelope spline points
		for i, p in enumerate(programs):
			splinePoints = p.getElementsByTagName("splinePoint")
	
			print(f"float splines_{i} [] = {{ ", file=file, end="");
			maxPoints = max(maxPoints, len(splinePoints))
			for child in splinePoints:
				for a in splineAttributes:
					value = child.attributes[a].value
					print(f"{value}, ", file=file, end="")
				
			print(" -1 };", file=file)
	
	print("const struct Program programs[] = {", file=file)
	for i, p in enumerate(programs):
		splinePoints = p.getElementsByTagName("splinePoint")
		print("\t { ", file=file, end="")

		name = p.attributes['programname'].value
		print(f' "{name}"', file=file, end="")

		if version > 1:
			print(f', splines_{i}, {len(splinePoints)}', file=file, end="")

		print(f', {{ ', file=file, end="")
		for j, a in enumerate(synthParameters):
			value = p.attributes[a].value if a in attributes else 0
			print(f" {value}, ", file=file, end="")
		print(" } },", file=file)
	print("}; ", file=file)

	if version > 1:
		print(f"int maxSplinePoints = {maxPoints};", file=file)

def makeCSV(programs, attributes, synthParameters, file=None):
	print(', '.join(['name'] + synthParameters), file=file)
	for i, p in enumerate(programs):
		name = p.attributes['programname'].value
		print(f'"{name}"', file=file, end="")
		for j, a in enumerate(synthParameters):
			value = p.attributes[a].value if a in attributes else 0
			print(f", {value}", file=file, end="")
		print(file=file)

def main(name, version=1, outHeader='program.h', outCSV=None):
	synthParameters = synthParameters1 if version == 1 else synthParameters2
	synthParameters = [i.lower() for i in synthParameters]

	tree = xml.dom.minidom.parse(name)
	programs = tree.getElementsByTagName("program")

	p = programs[-1]
	attributes = list(p.attributes.keys())
	# checkParameters(synthParameters, attributes)

	if outHeader is not None:
		with open(outHeader, 'w') as file:
			makeHeader(programs, attributes, synthParameters, version, file=file);

	if outCSV is not None:
		with open(outCSV, "w") as file:
			makeCSV(programs, attributes, synthParameters, file=file);

fire.Fire(main)
