/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 4.3.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "Main.h"
#include "MainComponent.h"
//[/Headers]

#include "SingleNoteAssign.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SingleNoteAssign::SingleNoteAssign ()
{
    //[Constructor_pre] You can add your own custom stuff here..
	colourSubwindow = new ColourEditComponent();
	addAndMakeVisible(colourSubwindow);
	//[/Constructor_pre]

    addAndMakeVisible (noteAndChannelAssGroup = new GroupComponent ("noteAndChannelAssGroup",
                                                                    TRANS("Key Note and Channel Assignments")));

    addAndMakeVisible (editInstructionText = new Label ("editInstructionText",
                                                        TRANS("Assign these values to a key by clicking on the desired key-face.")));
    editInstructionText->setFont (Font (15.00f, Font::plain));
    editInstructionText->setJustificationType (Justification::centredLeft);
    editInstructionText->setEditable (false, false, false);
    editInstructionText->setColour (TextEditor::textColourId, Colours::black);
    editInstructionText->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (noteBox = new ComboBox ("noteBox"));
    noteBox->setEditableText (false);
    noteBox->setJustificationType (Justification::centredLeft);
    noteBox->setTextWhenNothingSelected (String());
    noteBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    noteBox->addListener (this);

    addAndMakeVisible (noteAutoIncrButton = new ToggleButton ("noteAutoIncrButton"));
    noteAutoIncrButton->setButtonText (TRANS("Auto Increment"));
    noteAutoIncrButton->addListener (this);

    addAndMakeVisible (channelBox = new ComboBox ("channelBox"));
    channelBox->setEditableText (false);
    channelBox->setJustificationType (Justification::centredLeft);
    channelBox->setTextWhenNothingSelected (String());
    channelBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    channelBox->addListener (this);

    addAndMakeVisible (channelAutoIncrButton = new ToggleButton ("channelAutoIncrButton"));
    channelAutoIncrButton->setButtonText (TRANS("Increment after Note #"));
    channelAutoIncrButton->addListener (this);

    addAndMakeVisible (channelAutoIncrNoteBox = new ComboBox ("channelAutoIncrNoteBox"));
    channelAutoIncrNoteBox->setTooltip (TRANS("After reaching this note, the channel is incremented and the note is reset to 0."));
    channelAutoIncrNoteBox->setEditableText (false);
    channelAutoIncrNoteBox->setJustificationType (Justification::centredLeft);
    channelAutoIncrNoteBox->setTextWhenNothingSelected (String());
    channelAutoIncrNoteBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    channelAutoIncrNoteBox->addListener (this);

    addAndMakeVisible (setNoteToggleButton = new ToggleButton ("setNoteToggleButton"));
    setNoteToggleButton->setButtonText (TRANS("Note (0-127):"));
    setNoteToggleButton->addListener (this);

    addAndMakeVisible (setChannelToggleButton = new ToggleButton ("setChannelToggleButton"));
    setChannelToggleButton->setButtonText (TRANS("Channel (1-16):"));
    setChannelToggleButton->addListener (this);

    addAndMakeVisible (setColourToggleButton = new ToggleButton ("setColourToggleButton"));
    setColourToggleButton->setButtonText (TRANS("Colour:"));
    setColourToggleButton->addListener (this);

    addAndMakeVisible (keyTypeToggleButton = new ToggleButton ("keyTypeToggleButton"));
    keyTypeToggleButton->setButtonText (TRANS("Key type:"));
    keyTypeToggleButton->addListener (this);

    addAndMakeVisible (keyTypeCombo = new ComboBox ("keyTypeCombo"));
    keyTypeCombo->setEditableText (false);
    keyTypeCombo->setJustificationType (Justification::centredLeft);
    keyTypeCombo->setTextWhenNothingSelected (String());
    keyTypeCombo->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    keyTypeCombo->addItem (TRANS("Note on/Note off"), 1);
    keyTypeCombo->addItem (TRANS("Continuous controller"), 2);
    keyTypeCombo->addListener (this);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (428, 325);


    //[Constructor] You can add your own custom stuff here..

	// Colour scheme
	editInstructionText->setColour(TextEditor::textColourId, TerpstraSysExApplication::getApp().getColourScheme()["MAINWINDOWTEXTCOLOUR"]);
	editInstructionText->setColour(TextEditor::backgroundColourId, TerpstraSysExApplication::getApp().getColourScheme()["LABELBGCOLOUR"]);


	for (int i = 0; i <= 127; i++)
	{
		noteBox->addItem(String(i), i + 1);
		channelAutoIncrNoteBox->addItem(String(i), i + 1);
	}

	for (int i = 1; i <= 16; i++)
		channelBox->addItem(String(i), i);

	setNoteToggleButton->setToggleState(true, juce::NotificationType::sendNotification);
	setChannelToggleButton->setToggleState(true, juce::NotificationType::sendNotification);
	setColourToggleButton->setToggleState(true, juce::NotificationType::sendNotification);
	keyTypeToggleButton->setToggleState(true, juce::NotificationType::sendNotification);
	keyTypeCombo->setSelectedId(TerpstraKey::noteOnNoteOff);
    //[/Constructor]
}

SingleNoteAssign::~SingleNoteAssign()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
	colourSubwindow = nullptr;
    //[/Destructor_pre]

    noteAndChannelAssGroup = nullptr;
    editInstructionText = nullptr;
    noteBox = nullptr;
    noteAutoIncrButton = nullptr;
    channelBox = nullptr;
    channelAutoIncrButton = nullptr;
    channelAutoIncrNoteBox = nullptr;
    setNoteToggleButton = nullptr;
    setChannelToggleButton = nullptr;
    setColourToggleButton = nullptr;
    keyTypeToggleButton = nullptr;
    keyTypeCombo = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SingleNoteAssign::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffbad0de));

    //[UserPaint] Add your own custom painting code here..
	g.fillAll(TerpstraSysExApplication::getApp().getColourScheme()["MAINWINDOWBGCOLOUR"]);
	//[/UserPaint]
}

void SingleNoteAssign::resized()
{
    //[UserPreResize] Add your own custom resize code here..
	colourSubwindow->setBounds(120, 136, 196, 32);
    //[/UserPreResize]

    noteAndChannelAssGroup->setBounds (0, 8, 424, 208);
    editInstructionText->setBounds (8, 32, 352, 24);
    noteBox->setBounds (120, 64, 56, 24);
    noteAutoIncrButton->setBounds (192, 64, 160, 24);
    channelBox->setBounds (120, 96, 56, 24);
    channelAutoIncrButton->setBounds (192, 96, 160, 24);
    channelAutoIncrNoteBox->setBounds (360, 96, 56, 24);
    setNoteToggleButton->setBounds (8, 64, 112, 24);
    setChannelToggleButton->setBounds (8, 96, 112, 24);
    setColourToggleButton->setBounds (8, 136, 112, 24);
    keyTypeToggleButton->setBounds (8, 176, 112, 24);
    keyTypeCombo->setBounds (120, 176, 192, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SingleNoteAssign::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == noteBox)
    {
        //[UserComboBoxCode_noteBox] -- add your combo box handling code here..
        //[/UserComboBoxCode_noteBox]
    }
    else if (comboBoxThatHasChanged == channelBox)
    {
        //[UserComboBoxCode_channelBox] -- add your combo box handling code here..
        //[/UserComboBoxCode_channelBox]
    }
    else if (comboBoxThatHasChanged == channelAutoIncrNoteBox)
    {
        //[UserComboBoxCode_channelAutoIncrNoteBox] -- add your combo box handling code here..
        //[/UserComboBoxCode_channelAutoIncrNoteBox]
    }
    else if (comboBoxThatHasChanged == keyTypeCombo)
    {
        //[UserComboBoxCode_keyTypeCombo] -- add your combo box handling code here..
        //[/UserComboBoxCode_keyTypeCombo]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void SingleNoteAssign::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == noteAutoIncrButton)
    {
        //[UserButtonCode_noteAutoIncrButton] -- add your button handler code here..
        //[/UserButtonCode_noteAutoIncrButton]
    }
    else if (buttonThatWasClicked == channelAutoIncrButton)
    {
        //[UserButtonCode_channelAutoIncrButton] -- add your button handler code here..
        //[/UserButtonCode_channelAutoIncrButton]
    }
    else if (buttonThatWasClicked == setNoteToggleButton)
    {
        //[UserButtonCode_setNoteToggleButton] -- add your button handler code here..
		bool fieldActive = setNoteToggleButton->getToggleState();
		noteBox->setEnabled(fieldActive);
		noteAutoIncrButton->setEnabled(fieldActive);
        //[/UserButtonCode_setNoteToggleButton]
    }
    else if (buttonThatWasClicked == setChannelToggleButton)
    {
        //[UserButtonCode_setChannelToggleButton] -- add your button handler code here..
		bool fieldActive = setChannelToggleButton->getToggleState();
		channelBox->setEnabled(fieldActive);
		channelAutoIncrButton->setEnabled(fieldActive);
		channelAutoIncrNoteBox->setEnabled(fieldActive);
        //[/UserButtonCode_setChannelToggleButton]
    }
    else if (buttonThatWasClicked == setColourToggleButton)
    {
        //[UserButtonCode_setColourToggleButton] -- add your button handler code here..
		bool fieldActive = setColourToggleButton->getToggleState();
		colourSubwindow->setEnabled(fieldActive);
        //[/UserButtonCode_setColourToggleButton]
    }
    else if (buttonThatWasClicked == keyTypeToggleButton)
    {
        //[UserButtonCode_keyTypeToggleButton] -- add your button handler code here..
		bool fieldActive = keyTypeToggleButton->getToggleState();
		keyTypeCombo->setEnabled(fieldActive);
        //[/UserButtonCode_keyTypeToggleButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

// Called from MainComponent when one of the keys is clicked
void SingleNoteAssign::PerformMouseClickEdit(int setSelection, int keySelection)
{
	jassert(setSelection >= 0 && setSelection < NUMBEROFBOARDS && keySelection >= 0 && keySelection < TERPSTRABOARDSIZE);

	TerpstraKey& keyData = ((MainContentComponent*)(getParentComponent()->getParentComponent()))->getMappingInEdit().sets[setSelection].theKeys[keySelection];

	// Set note if specified
	if (setNoteToggleButton->getToggleState())
	{
		keyData.noteNumber = noteBox->getSelectedItemIndex(); //-1 for no selection or 0-127
		if (keyData.noteNumber < 0) keyData.noteNumber = 0;
	}

	// Set channel if specified
	if (setChannelToggleButton->getToggleState())
	{
		keyData.channelNumber = channelBox->getSelectedId();	// 0 for no selection or 1-16
	}

	// Set colour if specified
	if (setColourToggleButton->getToggleState())
	{
		keyData.colour = colourSubwindow->getColourAsNumber();
	}

	// Set key type if specified
	if (keyTypeToggleButton->getToggleState())
	{
		keyData.keyType = (TerpstraKey::KEYTYPE)keyTypeCombo->getSelectedId();	// XXX if no selection?
	}

	// Send to device
	TerpstraSysExApplication::getApp().getMidiDriver().sendAndMaybeSaveKeyParam(setSelection + 1, keySelection, keyData);

	// Auto increment note
	if (noteAutoIncrButton->getToggleState())
	{
		int newNote = keyData.noteNumber + 1;

		// Auto increment channel
		if (channelAutoIncrButton->getToggleState() && channelAutoIncrNoteBox->getSelectedItemIndex() > 0 &&
			newNote > channelAutoIncrNoteBox->getSelectedItemIndex())
		{
			newNote = 0;
			int newChannel = keyData.channelNumber + 1;
			if (newChannel > 16)
				newChannel = 1;
			channelBox->setSelectedId(newChannel);
		}

		if (newNote > 127)
			newNote = 0;

		noteBox->setSelectedItemIndex(newNote);
	}
}

void SingleNoteAssign::onSetData(TerpstraKeyMapping& newData)
{
	SortedSet<int> usedColours = newData.getUsedColours();
	for (int pos = 0; pos < usedColours.size(); pos++)
		colourSubwindow->addColourToBox(usedColours[pos]);
}

void SingleNoteAssign::restoreStateFromPropertiesFile(PropertiesFile* propertiesFile)
{
	setNoteToggleButton->setToggleState(
		propertiesFile->getBoolValue("SingleNoteNoteSetActive", true),
		juce::NotificationType::sendNotification);
	setChannelToggleButton->setToggleState(
		propertiesFile->getBoolValue("SingleNoteChannelSetActive", true),
		juce::NotificationType::sendNotification);
	setColourToggleButton->setToggleState(
		propertiesFile->getBoolValue("SingleNoteColourSetActive", true),
		juce::NotificationType::sendNotification);
	keyTypeToggleButton->setToggleState(
		propertiesFile->getBoolValue("SingleNoteKeyTypeSetActive", true),
		juce::NotificationType::sendNotification);
}

void SingleNoteAssign::saveStateToPropertiesFile(PropertiesFile* propertiesFile)
{
	propertiesFile->setValue("SingleNoteNoteSetActive", setNoteToggleButton->getToggleState());
	propertiesFile->setValue("SingleNoteChannelSetActive", setChannelToggleButton->getToggleState());
	propertiesFile->setValue("SingleNoteColourSetActive", setColourToggleButton->getToggleState());
	propertiesFile->setValue("SingleNoteKeyTypeSetActive", keyTypeToggleButton->getToggleState());

}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SingleNoteAssign" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="428" initialHeight="325">
  <BACKGROUND backgroundColour="ffbad0de"/>
  <GROUPCOMPONENT name="noteAndChannelAssGroup" id="cd12ee7381d485a1" memberName="noteAndChannelAssGroup"
                  virtualName="" explicitFocusOrder="0" pos="0 8 424 208" title="Key Note and Channel Assignments"/>
  <LABEL name="editInstructionText" id="c03ef432c2b4599" memberName="editInstructionText"
         virtualName="" explicitFocusOrder="0" pos="8 32 352 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Assign these values to a key by clicking on the desired key-face."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15" bold="0" italic="0" justification="33"/>
  <COMBOBOX name="noteBox" id="123cacc6155f964" memberName="noteBox" virtualName=""
            explicitFocusOrder="0" pos="120 64 56 24" editable="0" layout="33"
            items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="noteAutoIncrButton" id="49829699593b11f7" memberName="noteAutoIncrButton"
                virtualName="" explicitFocusOrder="0" pos="192 64 160 24" buttonText="Auto Increment"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <COMBOBOX name="channelBox" id="208bbc8901c22319" memberName="channelBox"
            virtualName="" explicitFocusOrder="0" pos="120 96 56 24" editable="0"
            layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="channelAutoIncrButton" id="1749290d10236ec3" memberName="channelAutoIncrButton"
                virtualName="" explicitFocusOrder="0" pos="192 96 160 24" buttonText="Increment after Note #"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <COMBOBOX name="channelAutoIncrNoteBox" id="4560285c5e467e2f" memberName="channelAutoIncrNoteBox"
            virtualName="" explicitFocusOrder="0" pos="360 96 56 24" tooltip="After reaching this note, the channel is incremented and the note is reset to 0."
            editable="0" layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <TOGGLEBUTTON name="setNoteToggleButton" id="79f2522d584925d1" memberName="setNoteToggleButton"
                virtualName="" explicitFocusOrder="0" pos="8 64 112 24" buttonText="Note (0-127):"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="setChannelToggleButton" id="f79f82eef6095c3c" memberName="setChannelToggleButton"
                virtualName="" explicitFocusOrder="0" pos="8 96 112 24" buttonText="Channel (1-16):"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="setColourToggleButton" id="fb41f2b9539dfb3f" memberName="setColourToggleButton"
                virtualName="" explicitFocusOrder="0" pos="8 136 112 24" buttonText="Colour:"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="keyTypeToggleButton" id="3f2eba6027c4f2f5" memberName="keyTypeToggleButton"
                virtualName="" explicitFocusOrder="0" pos="8 176 112 24" buttonText="Key type:"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <COMBOBOX name="keyTypeCombo" id="6a64d9cabf0d810f" memberName="keyTypeCombo"
            virtualName="" explicitFocusOrder="0" pos="120 176 192 24" editable="0"
            layout="33" items="Note on/Note off&#10;Continuous controller"
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
