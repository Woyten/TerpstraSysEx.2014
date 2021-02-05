/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.5

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "ViewComponents.h"
#include "MainComponent.h"
#include "Main.h"
#include "LumatoneMenu.h"

//[/Headers]

#include "AllKeysOverview.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

// Geometry settings
static Random r;//DEBUG

//==============================================================================
KeyMiniDisplayInsideAllKeysOverview::KeyMiniDisplayInsideAllKeysOverview(int newBoardIndex, int newKeyIndex)
	: Component("AllKeysOverview_Key" + String(newKeyIndex) + "," + String(newBoardIndex))
{
	// In your constructor, you should add any child components, and
	// initialise any special settings that your component needs.
	boardIndex = newBoardIndex;
	keyIndex = newKeyIndex;

	TerpstraSysExApplication::getApp().getMidiDriver().addListener(this);
}

KeyMiniDisplayInsideAllKeysOverview::~KeyMiniDisplayInsideAllKeysOverview()
{
	TerpstraSysExApplication::getApp().getMidiDriver().removeListener(this);
}

//void KeyMiniDisplayInsideAllKeysOverview::paint(Graphics& g)
//{
//	//jassert(getParentComponent() != nullptr);
//	//bool boardIsSelected = boardIndex == dynamic_cast<AllKeysOverview*>(getParentComponent())->getCurrentSetSelection();
//
//	//Colour hexagonColour = findColour(TerpstraKeyEdit::backgroundColourId).overlaidWith(getKeyColour());
//		//.withAlpha(boardIsSelected ? TERPSTRASINGLEKEYCOLOURALPHA : TERPSTRASINGLEKEYCOLOURUNSELECTEDMINIALPHA));
//	// NEW STYLE TODO: Use above alpha, or do octave outline?
//	//// ToDo if highlighted: even different alpha?
//	//g.setColour(hexagonColour);
//	//g.fillPath(hexPath);
//}

void KeyMiniDisplayInsideAllKeysOverview::mouseDown(const MouseEvent& e)
{
	Component::mouseDown(e);

	// Select this octave board
	jassert(getParentComponent() != nullptr);
	jassert(getParentComponent()->getParentComponent() != nullptr);
	dynamic_cast<MainContentComponent*>(getParentComponent()->getParentComponent())->
		getOctaveBoardSelectorTab()->setCurrentTabIndex(boardIndex);

	isHighlighted = true;
	repaint();

	if (e.mods.isRightButtonDown())
	{
		// Right mosue click: popup menu
		PopupMenu menu;

		menu.addCommandItem(TerpstraSysExApplication::getApp().getCommandManager(), Lumatone::Menu::commandIDs::deleteOctaveBoard);
		menu.addCommandItem(TerpstraSysExApplication::getApp().getCommandManager(), Lumatone::Menu::commandIDs::copyOctaveBoard);
		menu.addCommandItem(TerpstraSysExApplication::getApp().getCommandManager(), Lumatone::Menu::commandIDs::pasteOctaveBoard);
		
		menu.show();
	}
	else
	{
		// NoteOn MIDI message
		auto keyData = getKeyData();
		if (keyData != nullptr && keyData->channelNumber > 0)
		{
			if (keyData->keyType == TerpstraKey::noteOnNoteOff)
			{
				// Send "note on" event
				TerpstraSysExApplication::getApp().getMidiDriver().sendNoteOnMessage(keyData->noteNumber, keyData->channelNumber, 60);
			}
			// ToDo if keyType is "continuous controller": send controller event?
		}
	}
}

void KeyMiniDisplayInsideAllKeysOverview::mouseUp(const MouseEvent& e)
{
	Component::mouseDown(e);

	isHighlighted = false;
	repaint();

	// NoteOff MIDI message
	auto keyData = getKeyData();
	if (keyData != nullptr && keyData->channelNumber > 0)
	{
		if (keyData->keyType == TerpstraKey::noteOnNoteOff)
		{
			// Send "note off" event
			TerpstraSysExApplication::getApp().getMidiDriver().sendNoteOffMessage(keyData->noteNumber, keyData->channelNumber, 60);
		}
	}
}

void KeyMiniDisplayInsideAllKeysOverview::midiMessageReceived(const MidiMessage& midiMessage)
{
	// ToDo If key is parametrized as controller?
	if (midiMessage.isNoteOnOrOff())
	{
		auto keyData = getKeyData();
		if (keyData != nullptr && midiMessage.getChannel() == keyData->channelNumber && midiMessage.getNoteNumber() == keyData->noteNumber)
		{
			isHighlighted = midiMessage.isNoteOn();
			repaint();
		}
	}
}

const TerpstraKey* KeyMiniDisplayInsideAllKeysOverview::getKeyData() const
{
	if (boardIndex >= 0 && boardIndex < NUMBEROFBOARDS && keyIndex >= 0 && keyIndex < TerpstraSysExApplication::getApp().getOctaveBoardSize())
	{
		jassert(getParentComponent() != nullptr);
		jassert(getParentComponent()->getParentComponent() != nullptr);
		return &dynamic_cast<MainContentComponent*>(getParentComponent()->getParentComponent())
			->getMappingInEdit().sets[boardIndex].theKeys[keyIndex];
	}

	return nullptr;
}

Colour KeyMiniDisplayInsideAllKeysOverview::getKeyColour() const
{
	auto keyData = getKeyData();
	if ( keyData != nullptr)
		return Colour(keyData->colour);
	else
		return findColour(TerpstraKeyEdit::backgroundColourId);
}

//==============================================================================


AllKeysDisplay::AllKeysDisplay()
	: fullKeyboard(ImageCache::getFromHashCode(LumatoneEditorAssets::KeyboardBase)),
	  keyShape(ImageCache::getFromHashCode(LumatoneEditorAssets::KeyShape)),
	  keybedShadows(ImageCache::getFromHashCode(LumatoneEditorAssets::KeybedShadows)),
	  graphicBaseWidth(fullKeyboard.getWidth()), graphicBaseHeight(fullKeyboard.getHeight()),
	  oct1Key1(graphicBaseWidth * oct1Key1X, graphicBaseHeight * oct1Key1Y),
	  oct1Key56(graphicBaseWidth * oct1Key56X, graphicBaseHeight * oct1Key56Y),
	  oct5Key7(graphicBaseWidth * oct5Key7X, graphicBaseHeight * oct5Key7Y)
{
	tilingGeometry.fitTilingTo(oct1Key1, oct1Key56, oct5Key7, false, 24, 10);

	Array<Point<int>> hexCoordinates;

	for (int octave = 0; octave < NUMBEROFBOARDS; octave++)
		hexCoordinates.addArray(boardGeometry.getOctaveCoordinates(octave));

	allKeyCentres = tilingGeometry.transformPointsFromOrigin(hexCoordinates);

	redrawAllKeys();
}

void AllKeysDisplay::paint(juce::Graphics& g)
{
	if (renderScaled.isValid())
		g.drawImageAt(renderScaled, 0, 0);
}

void AllKeysDisplay::resized()
{
	float w = getWidth();
	float h = getHeight();

	keyWidth = round(w * keyW);
	keyHeight = round(h * keyH);

	redrawAllKeys();
}

Image AllKeysDisplay::redrawAllKeys(bool returnScaled)
{
	renderFullSize = fullKeyboard.createCopy();
	Graphics g(renderFullSize);

	Colour keyColour;
	Point<float> centre;

	int halfKeyWidth = round(keyShape.getWidth() * 0.5f);
	int halfKeyHeight = round(keyShape.getHeight() * 0.5f);

	TerpstraKeyMapping mapping;
	MainContentComponent* main = nullptr;
	
	if (getParentComponent() && getParentComponent()->getParentComponent())
	{
		main = dynamic_cast<MainContentComponent*>(getParentComponent()->getParentComponent());
		mapping = main->getMappingInEdit();
	}

	int keyIndex = 0;
	for (int octave = 0; octave < NUMBEROFBOARDS; octave++)
	{
		for (int i = 0; i < TerpstraSysExApplication::getApp().getOctaveBoardSize(); i++)
		{
			if (main)
			{
				auto key = mapping.sets[octave].theKeys[i];
				g.setColour(Colour(key.colour));
			}
			else
				g.setColour(Colour());

			centre = allKeyCentres[keyIndex];
			g.drawImageAt(keyShape, centre.x - halfKeyWidth, centre.y - halfKeyHeight, true);
			keyIndex++;
		}
	}
	g.drawImageAt(keybedShadows, 0, 0);
	
	rescaleAllKeys();

	if (returnScaled)
		return renderScaled;

	return renderFullSize;
}

Image AllKeysDisplay::rescaleAllKeys()
{
	if (renderFullSize.isValid() && getWidth() > 0 && getHeight() > 0)
		renderScaled = resizeImage(renderFullSize, getWidth(), getHeight(), "lanczos3", 1.5f);

	return renderScaled;
}

Image AllKeysDisplay::getAllKeysRender(bool returnScaled)
{
	if (returnScaled)
		return renderScaled;
	
	return renderFullSize;
}


//[/MiscUserDefs]

//==============================================================================
AllKeysOverview::AllKeysOverview()
	: Component("AllKeysOverview")
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    btnLoadFile.reset (new juce::TextButton ("btnLoadFile"));
    addAndMakeVisible (btnLoadFile.get());

    btnLoadFile->setButtonText (translate("LoadFile"));
	btnLoadFile->getProperties().set(LumatoneEditorStyleIDs::textButtonIconHashCode, LumatoneEditorAssets::LoadIcon);
    btnLoadFile->addListener (this);

    btnSaveFile.reset (new juce::TextButton ("btnSaveFile"));
    addAndMakeVisible (btnSaveFile.get());
    btnSaveFile->setButtonText (translate("SaveFile"));
	btnSaveFile->getProperties().set(LumatoneEditorStyleIDs::textButtonIconHashCode, LumatoneEditorAssets::SaveIcon);
    btnSaveFile->addListener (this);

    buttonReceive.reset (new juce::TextButton ("buttonReceive"));
    addAndMakeVisible (buttonReceive.get());
    buttonReceive->setTooltip (translate("ImportTooltip"));
    buttonReceive->setButtonText (translate("Import from Lumatone"));
	buttonReceive->getProperties().set(LumatoneEditorStyleIDs::textButtonIconHashCode, LumatoneEditorAssets::ImportIcon);
	buttonReceive->getProperties().set(LumatoneEditorStyleIDs::textButtonIconPlacement, LumatoneEditorStyleIDs::TextButtonIconPlacement::RightOfText);
    buttonReceive->addListener (this);

	allKeysDisplay.reset(new AllKeysDisplay());
	addAndMakeVisible(allKeysDisplay.get());

    //[UserPreSize]

	for (int subBoardIndex = 0; subBoardIndex < NUMBEROFBOARDS; subBoardIndex++)
	{
		OctaveBoard* board = octaveBoards.add(new OctaveBoard());

		for (int keyIndex = 0; keyIndex < TerpstraSysExApplication::getApp().getOctaveBoardSize(); keyIndex++)
		{
			auto key = board->keyMiniDisplay.add(new KeyMiniDisplayInsideAllKeysOverview(subBoardIndex, keyIndex));
			addAndMakeVisible(key);
		}

		jassert(board->keyMiniDisplay.size() == TerpstraSysExApplication::getApp().getOctaveBoardSize());
	}

	jassert(octaveBoards.size() == NUMBEROFBOARDS);

    //[/UserPreSize]


    //[Constructor] You can add your own custom stuff here..
	currentSetSelection = -1;
    //[/Constructor]
}

AllKeysOverview::~AllKeysOverview()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    btnLoadFile = nullptr;
    btnSaveFile = nullptr;
    buttonReceive = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AllKeysOverview::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..

	// Draw a line under the selected sub board
	if (currentSetSelection >= 0 && currentSetSelection < NUMBEROFBOARDS)
	{
		Path selectionMarkPath;
		selectionMarkPath.startNewSubPath(octaveBoards[currentSetSelection]->leftPos, octaveLineY);
		selectionMarkPath.lineTo(octaveBoards[currentSetSelection]->rightPos, octaveLineY);

		Colour lineColour = findColour(TerpstraKeyEdit::outlineColourId);
		g.setColour(lineColour);
		g.strokePath(selectionMarkPath, PathStrokeType(1.0f));
	}

    //[/UserPaint]
}

void AllKeysOverview::resized()
{
    //[UserPreResize] Add your own custom resize code here..

	// Prepare position helpers for graphics
	int graphicHeight = roundToInt(getHeight() * imageHeight);
	int graphicWidth = roundToInt(imageAspect * graphicHeight);

	lumatoneBounds.setBounds(
		roundToInt((getWidth() - graphicWidth) * 0.5f), roundToInt(getHeight() * imageY),
		graphicWidth, graphicHeight
	);

	allKeysDisplay->setBounds(lumatoneBounds);

	int btnHeight = round(getHeight() * saveLoadH);
	int btnMargin = round(getWidth() * saveloadMarginW);
	int saveLoadWidth = round(getWidth() * saveLoadW);
	int btnY = lumatoneBounds.getY() - round(getHeight() * btnYFromImageTop);
	
	int halfWidthX = round(getWidth() * 0.5f);

	btnLoadFile->setBounds(halfWidthX - btnMargin - saveLoadWidth, btnY, saveLoadWidth, btnHeight);
	btnSaveFile->setBounds(halfWidthX + btnMargin, btnY, saveLoadWidth, btnHeight);

	octaveLineY = lumatoneBounds.getBottom() + round(getHeight() * octaveLineYRatio);

	int importY = lumatoneBounds.getY() - round(getHeight() * importYFromImageTop);
	int importWidth = round(getWidth() * importW);
	buttonReceive->setBounds(lumatoneBounds.getRight() - importWidth, importY, importWidth, btnHeight);

    //[/UserResized]
}

void AllKeysOverview::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == btnLoadFile.get())
    {
        //[UserButtonCode_btnLoadFile] -- add your button handler code here..
		TerpstraSysExApplication::getApp().openSysExMapping();
        //[/UserButtonCode_btnLoadFile]
    }
    else if (buttonThatWasClicked == btnSaveFile.get())
    {
        //[UserButtonCode_btnSaveFile] -- add your button handler code here..
		TerpstraSysExApplication::getApp().saveSysExMappingAs();
        //[/UserButtonCode_btnSaveFile]
    }
    else if (buttonThatWasClicked == buttonReceive.get())
    {
        //[UserButtonCode_buttonReceive] -- add your button handler code here..
		TerpstraSysExApplication::getApp().requestConfigurationFromDevice();
        //[/UserButtonCode_buttonReceive]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void AllKeysOverview::lookAndFeelChanged()
{
	auto lookAndFeel = dynamic_cast<LumatoneEditorLookAndFeel*>(&getLookAndFeel());
	if (lookAndFeel)
	{
		lookAndFeel->setupTextButton(*btnLoadFile);
		lookAndFeel->setupTextButton(*btnSaveFile);
		lookAndFeel->setupTextButton(*buttonReceive);
	}
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AllKeysOverview" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="928" initialHeight="214">
  <BACKGROUND backgroundColour="ff323e44"/>
  <TEXTBUTTON name="btnLoadFile" id="6c0c074c9f137f23" memberName="btnLoadFile"
              virtualName="" explicitFocusOrder="0" pos="368 8 96 24" buttonText="Load File"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="btnSaveFile" id="abbc33d699ba1e52" memberName="btnSaveFile"
              virtualName="" explicitFocusOrder="0" pos="472 8 96 24" buttonText="Save File"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="buttonReceive" id="6a7ed19ee86a3b97" memberName="buttonReceive"
              virtualName="" explicitFocusOrder="0" pos="584 8 176 24" tooltip="Receive the current configuration from controller"
              buttonText="Import from Lumatone" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

