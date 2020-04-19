/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.5

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../../TerpstraSysEx.2014/JuceLibraryCode/JuceHeader.h"

#include "HajuLib/HajuErrorVisualizer.h"
#include "MappingLogic.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class KBMForOneChannel  : public Component,
                          public TextEditor::Listener,
                          public ComboBox::Listener,
                          public Button::Listener
{
public:
    //==============================================================================
    KBMForOneChannel (int		subDlgIndex, KBMFilesMappingLogic&	mappingLogic);
    ~KBMForOneChannel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
	void restoreStateFromPropertiesFile(PropertiesFile* propertiesFile);
	void saveStateToPropertiesFile(PropertiesFile* propertiesFile);
    void textEditorFocusLost(TextEditor& textEdit) override;

	void updateFieldsAndMappingLogic();

public:
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
	int		subDlgIndex;
	File	currentFile;
	KBMMappingDataStructure kbmMappingStructure;
    KBMFilesMappingLogic*	pMappingLogic;
    HajuErrorVisualizer     errorVisualizer;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<ComboBox> channelBox;
    std::unique_ptr<TextEditor> textMappingFile;
    std::unique_ptr<TextButton> btnFileSelectMacro;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KBMForOneChannel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

