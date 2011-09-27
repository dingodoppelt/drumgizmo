/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/***************************************************************************
 *            drumgizmo_vst.cc
 *
 *  Tue Sep 20 08:22:48 CEST 2011
 *  Copyright 2011 Bent Bisballe Nyeng
 *  deva@aasimon.org
 ****************************************************************************/

/*
 *  This file is part of DrumGizmo.
 *
 *  DrumGizmo is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  DrumGizmo is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with DrumGizmo; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#include "drumgizmo_vst.h"

#include "constants.h"

#define NUM_PROGRAMS 0
#define NUM_PARAMS 0

AudioEffect* createEffectInstance(audioMasterCallback audioMaster)
{
	return new DrumGizmoVst(audioMaster);
}

DrumGizmoVst::DrumGizmoVst(audioMasterCallback audioMaster)
  : AudioEffectX(audioMaster, NUM_PROGRAMS, NUM_PARAMS)
{
  pos = 0;
  buffer = NULL;
  buffer_size = 0;

  output = new OutputVST();
  input = new InputVST();
  drumgizmo = new DrumGizmo(output, input);
  
	// initialize programs
	//programs = new DrumGizmoVstProgram[kNumPrograms];
	//for(VstInt32 i = 0; i < 16; i++) channelPrograms[i] = i;

  //if(programs) setProgram(0);
	
	if(audioMaster)	{
		setNumInputs(0); // no audio inputs
		setNumOutputs(NUM_OUTPUTS);
		canProcessReplacing();
		isSynth();
    char id[] = "BLOP"; // Four bytes typecasted into an unsigned integer
		setUniqueID(*(unsigned int*)id);
	}

	initProcess();
	suspend();
}

DrumGizmoVst::~DrumGizmoVst()
{
  delete drumgizmo;
  delete input;
  delete output;
}

void DrumGizmoVst::setProgram(VstInt32 program) {}
void DrumGizmoVst::setProgramName(char* name) {}
void DrumGizmoVst::getProgramName(char* name) { name[0] = '\0'; }

void DrumGizmoVst::getParameterLabel(VstInt32 index, char* label)
{
	label[0] = '\0';
	/*
	switch(index)
	{
		case kWaveform1:
		case kWaveform2:
			vst_strncpy(label, "Shape", kVstMaxParamStrLen);
			break;

		case kFreq1:
		case kFreq2:
			vst_strncpy(label, "Hz", kVstMaxParamStrLen);
			break;

		case kVolume1:
		case kVolume2:
		case kVolume:
			vst_strncpy(label, "dB", kVstMaxParamStrLen);
			break;
	}
	*/
}

void DrumGizmoVst::getParameterDisplay(VstInt32 index, char* text)
{
	text[0] = 0;
	/*
	switch(index)
	{
		case kWaveform1:
			if(fWaveform1 < .5)
				vst_strncpy(text, "Sawtooth", kVstMaxParamStrLen);
			else
				vst_strncpy(text, "Pulse", kVstMaxParamStrLen);
			break;

		case kFreq1:		float2string(fFreq1, text, kVstMaxParamStrLen);	break;
		case kVolume1:		dB2string(fVolume1, text, kVstMaxParamStrLen);	break;
		
		case kWaveform2:
			if(fWaveform2 < .5)
				vst_strncpy(text, "Sawtooth", kVstMaxParamStrLen);
			else
				vst_strncpy(text, "Pulse", kVstMaxParamStrLen);
			break;

		case kFreq2:		float2string(fFreq2, text, kVstMaxParamStrLen);	break;
		case kVolume2:		dB2string(fVolume2, text, kVstMaxParamStrLen);	break;
		case kVolume:		dB2string(fVolume, text, kVstMaxParamStrLen);	break;
	}
	*/
}

void DrumGizmoVst::getParameterName(VstInt32 index, char* label)
{
	/*
	switch(index)
	{
		case kWaveform1:	vst_strncpy(label, "Wave 1", kVstMaxParamStrLen);	break;
		case kFreq1:		vst_strncpy(label, "Freq 1", kVstMaxParamStrLen);	break;
		case kVolume1:		vst_strncpy(label, "Levl 1", kVstMaxParamStrLen);	break;
		case kWaveform2:	vst_strncpy(label, "Wave 2", kVstMaxParamStrLen);	break;
		case kFreq2:		vst_strncpy(label, "Freq 2", kVstMaxParamStrLen);	break;
		case kVolume2:		vst_strncpy(label, "Levl 2", kVstMaxParamStrLen);	break;
		case kVolume:		vst_strncpy(label, "Volume", kVstMaxParamStrLen);	break;
	}
	*/
}

void DrumGizmoVst::setParameter(VstInt32 index, float value)
{
	/*
	DrumGizmoVstProgram *ap = &programs[curProgram];
	switch(index)
	{
		case kWaveform1:	fWaveform1	= ap->fWaveform1	= value;	break;
		case kFreq1:		fFreq1 		= ap->fFreq1		= value;	break;
		case kVolume1:		fVolume1	= ap->fVolume1		= value;	break;
		case kWaveform2:	fWaveform2	= ap->fWaveform2	= value;	break;
		case kFreq2:		fFreq2		= ap->fFreq2		= value;	break;
		case kVolume2:		fVolume2	= ap->fVolume2		= value;	break;
		case kVolume:		fVolume		= ap->fVolume		= value;	break;
	}
	*/
}

float DrumGizmoVst::getParameter(VstInt32 index)
{
	float value = 0;
	/*
	switch(index)
	{
		case kWaveform1:	value = fWaveform1;	break;
		case kFreq1:		value = fFreq1;		break;
		case kVolume1:		value = fVolume1;	break;
		case kWaveform2:	value = fWaveform2;	break;
		case kFreq2:		value = fFreq2;		break;
		case kVolume2:		value = fVolume2;	break;
		case kVolume:		value = fVolume;	break;
	}
	*/
	return value;
}

bool DrumGizmoVst::getOutputProperties(VstInt32 index,
                                       VstPinProperties* properties)
{
	if(index < NUM_OUTPUTS)
	{
		vst_strncpy(properties->label, "Channel ", 63);
		char temp[11] = {0};
		int2string(index + 1, temp, 10);
		vst_strncat(properties->label, temp, 63);

		properties->flags = kVstPinIsActive;

		return true;
	}
	return false;
}

bool DrumGizmoVst::getProgramNameIndexed(VstInt32 category, VstInt32 index,
                                         char* text)
{
	return false;
}

bool DrumGizmoVst::getEffectName(char* name)
{
	vst_strncpy(name, "DrumGizmoSKU", kVstMaxEffectNameLen);
	return true;
}

bool DrumGizmoVst::getVendorString(char* text)
{
	vst_strncpy(text, "Aasimon.org-yeah", kVstMaxVendorStrLen);
	return true;
}

bool DrumGizmoVst::getProductString(char* text)
{
	vst_strncpy(text, "Vst Synthdims", kVstMaxProductStrLen);
	return true;
}

VstInt32 DrumGizmoVst::getVendorVersion()
{ 
	return 1000; 
}

VstInt32 DrumGizmoVst::canDo(char* text)
{
	if(!strcmp(text, "receiveVstEvents")) return 1;
	if(!strcmp(text, "receiveVstMidiEvent"))	return 1;
  //if(!strcmp(text, "midiProgramNames")) return 1;
	return -1;	// explicitly can't do; 0 => don't know
}

VstInt32 DrumGizmoVst::getNumMidiInputChannels()
{
	return 1; // we are monophonic
}

VstInt32 DrumGizmoVst::getNumMidiOutputChannels()
{
	return 0; // no MIDI output back to Host app
}

VstInt32 DrumGizmoVst::getMidiProgramName(VstInt32 channel,
                                          MidiProgramName* mpn)
{
	VstInt32 prg = mpn->thisProgramIndex;
	if(prg < 0 || prg >= 128)
		return 0;
	fillProgram(channel, prg, mpn);
	if(channel == 9)
		return 1;
	return 128L;
}

VstInt32 DrumGizmoVst::getCurrentMidiProgram(VstInt32 channel,
                                             MidiProgramName* mpn)
{
	if(channel < 0 || channel >= 16 || !mpn) return -1;
	VstInt32 prg = 0;
	mpn->thisProgramIndex = prg;
	fillProgram(channel, prg, mpn);
	return prg;
}

void DrumGizmoVst::fillProgram(VstInt32 channel, VstInt32 prg,
                               MidiProgramName* mpn)
{
	mpn->midiBankMsb = mpn->midiBankLsb = -1;
	mpn->reserved = 0;
	mpn->flags = 0;

	vst_strncpy(mpn->name, "Standard", 63);
	mpn->midiProgram = 0;
	mpn->parentCategoryIndex = 0;
}

VstInt32 DrumGizmoVst::getMidiProgramCategory(VstInt32 channel,
                                              MidiProgramCategory* cat)
{
	cat->parentCategoryIndex = -1;	// -1:no parent category
	cat->flags = 0;					// reserved, none defined yet, zero.
	VstInt32 category = cat->thisCategoryIndex;
	vst_strncpy(cat->name, "Drums", 63);
	return 1;
}

bool DrumGizmoVst::hasMidiProgramsChanged(VstInt32 channel)
{
	return false;	// updateDisplay()
}

bool DrumGizmoVst::getMidiKeyName(VstInt32 channel, MidiKeyName* key)
// struct will be filled with information for 'thisProgramIndex' and
//  'thisKeyNumber'
// if keyName is "" the standard name of the key will be displayed.
// if false is returned, no MidiKeyNames defined for 'thisProgramIndex'.
{
	// key->thisProgramIndex;		// >= 0. fill struct for this program index.
	// key->thisKeyNumber;			// 0 - 127. fill struct for this key number.
	key->keyName[0] = 0;
	key->reserved = 0;				// zero
	key->flags = 0;					// reserved, none defined yet, zero.
	return false;
}

void DrumGizmoVst::setSampleRate(float sampleRate)
{
	AudioEffectX::setSampleRate(sampleRate);
}

void DrumGizmoVst::setBlockSize(VstInt32 blockSize)
{
	AudioEffectX::setBlockSize(blockSize);
}

void DrumGizmoVst::initProcess()
{
  drumgizmo->loadkit(getenv("DRUMGIZMO_DRUMKIT"));
  drumgizmo->init(true);
}

void DrumGizmoVst::processReplacing(float** inputs, float** outputs,
                                    VstInt32 sampleFrames)
{
  output->setOutputs(outputs);

   if(buffer_size != sampleFrames) {
    if(buffer) free(buffer);
    buffer_size = sampleFrames;
    buffer = (sample_t*)malloc(sizeof(sample_t) * buffer_size);
  }

  drumgizmo->run(pos, buffer, buffer_size);

  pos += sampleFrames;
}

VstInt32 DrumGizmoVst::processEvents(VstEvents* ev)
{
  input->processEvents(ev);
	return 1;
}
