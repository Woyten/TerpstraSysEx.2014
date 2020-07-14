/*
  ==============================================================================

    HajuMidiDriver.cpp
    Created: 23 Feb 2018 11:30:58pm
    Author:  hsstraub

  ==============================================================================
*/

#include "HajuMidiDriver.h"


HajuMidiDriver::HajuMidiDriver()
{
	midiInputs = MidiInput::getDevices();
	midiOutputs = MidiOutput::getDevices();

	deviceManager.initialise(midiInputs.size(), midiOutputs.size(), 0, true, String(), 0);
}

HajuMidiDriver::~HajuMidiDriver()
{
    if (lastInputIndex >= 0)
    {
        deviceManager.removeMidiInputDeviceCallback(midiInputs[lastInputIndex], lastInputCallback);
    }

	midiOutput = nullptr;
}

void HajuMidiDriver::setMidiInput(int deviceIndex, MidiInputCallback* callback)
{
    if (lastInputIndex >= 0)
    {
        deviceManager.removeMidiInputDeviceCallback(midiInputs[lastInputIndex], lastInputCallback);
    }

	auto newInput = midiInputs[deviceIndex];
	if (!deviceManager.isMidiInputEnabled(newInput))
		deviceManager.setMidiInputEnabled(newInput, true);

	deviceManager.addMidiInputDeviceCallback(newInput, callback);

	lastInputIndex = deviceIndex;
	lastInputCallback = callback;
}

void HajuMidiDriver::setMidiOutput(int deviceIndex)
{
	midiOutput = MidiOutput::openDevice(deviceIndex);
}

void HajuMidiDriver::sendMessageNow(const MidiMessage& message)
{
	// Send only if output device is there
	if (midiOutput != nullptr)
		midiOutput->sendMessageNow(message);
}

void HajuMidiDriver::sendNoteOnMessage(int noteNumber, int channelNumber, uint8 velocity)
{
	if (channelNumber > 0 && noteNumber >= 0)
		sendMessageNow(MidiMessage::noteOn(channelNumber, noteNumber, velocity));
}

void HajuMidiDriver::sendNoteOffMessage(int noteNumber, int channelNumber, uint8 velocity)
{
	if (channelNumber > 0 && noteNumber >= 0)
		sendMessageNow(MidiMessage::noteOff(channelNumber, noteNumber, velocity));
}
