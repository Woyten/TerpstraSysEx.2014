/*
  ==============================================================================

  MainComponent.cpp
  Created: XXX.2014
  Author:  hsstraub

  ==============================================================================
*/

#include "MainComponent.h"
#include "ViewConstants.h"
#include "Main.h"


//==============================================================================
MainContentComponent::MainContentComponent()
	: currentSetSelection(-1), currentSingleKeySelection(-1), copiedSubBoardData()
{
	// Key set fields
	for (int i = 0; i < NUMBEROFBOARDS; i++)
	{
		// Macro button
		macroButtons[i].reset(new TerpstraMacroButton());
		addAndMakeVisible(macroButtons[i].get());

		// Paint set fields from right to left
		// (This will not matter any more when the images' backgrounds are transparent)
		// Width and heigth: were taken from image
		terpstraSetSelectors[4-i].reset(new OctaveBoardComponent(4-i));
		addAndMakeVisible(terpstraSetSelectors[4-i].get());
		terpstraSetSelectors[4 - i]->addMouseListener(this, true);
	}

	// Single Key fields
	for (int i = 0; i < TERPSTRABOARDSIZE; i++)
	{
		terpstraKeyFields[i].reset(new TerpstraKeyEdit());
		addAndMakeVisible(terpstraKeyFields[i].get());
		terpstraKeyFields[i]->addMouseListener(this, true);
	}

	// Midi input + output
	midiEditArea = new MidiEditArea();
	addAndMakeVisible(midiEditArea);
	midiEditArea->addSendAllButtonListener(this);

	// Edit function area
	noteEditArea = new NoteEditArea();
	addAndMakeVisible(noteEditArea);

	// Initial size
	setSize(DEFAULTMAINWINDOWWIDTH, DEFAULTMAINWINDOWHEIGHT);

	// Select first board and first key
	changeSetSelection(0);
	changeSingleKeySelection(0);
}

MainContentComponent::~MainContentComponent()
{
	for (int i = 0; i < NUMBEROFBOARDS; i++)
	{
		macroButtons[i] = nullptr;
		terpstraSetSelectors[i] = nullptr;
	}

	for (int i = 0; i < TERPSTRABOARDSIZE; i++)
	{
		terpstraKeyFields[i] = nullptr;
	}

	deleteAllChildren();
}

void MainContentComponent::restoreStateFromPropertiesFile(PropertiesFile* propertiesFile)
{
	setSize(
		propertiesFile->getIntValue("MainWindowWidth", DEFAULTMAINWINDOWWIDTH),
		propertiesFile->getIntValue("MainWindowHeight", DEFAULTMAINWINDOWHEIGHT));

	noteEditArea->restoreStateFromPropertiesFile(propertiesFile);
}

void MainContentComponent::saveStateToPropertiesFile(PropertiesFile* propertiesFile)
{
	propertiesFile->setValue("MainWindowWidth", getWidth());
	propertiesFile->setValue("MainWindowHeight", getHeight());

	noteEditArea->saveStateToPropertiesFile(propertiesFile);
}

// Set the current mapping to be edited to the value passed in parameter
void MainContentComponent::setData(TerpstraKeyMapping& newData)
{
	mappingData = newData;

	noteEditArea->onSetData(newData);

	for ( int i = 0; i < NUMBEROFBOARDS; i++)
        terpstraSetSelectors[i]->repaint();

	changeSetSelection(-1);
	changeSetSelection(0);
}

// Copy the edited mapping to the variable passed as parameter
void MainContentComponent::getData(TerpstraKeyMapping& newData)
{
	newData = mappingData;
}

bool MainContentComponent::deleteCurrentSubBoardData()
{
	if (currentSetSelection >= 0 && currentSetSelection < NUMBEROFBOARDS)
	{
		// Delete subboard data
		mappingData.sets[currentSetSelection] = TerpstraKeys();

		// Refresh display
		changeSetSelection(currentSetSelection, true);

		// Mark that there are changes
		TerpstraSysExApplication::getApp().setHasChangesToSave(true);

		return true;
	}
	else
		return false;
}

bool MainContentComponent::copyCurrentSubBoardData()
{
	if (currentSetSelection >= 0 && currentSetSelection < NUMBEROFBOARDS)
	{
		copiedSubBoardData = mappingData.sets[currentSetSelection];
		return true;
	}
	else
		return false;
}

bool MainContentComponent::pasteCurrentSubBoardData()
{
	if (currentSetSelection >= 0 && currentSetSelection < NUMBEROFBOARDS)
	{
		if (!copiedSubBoardData.isEmpty())
		{
			mappingData.sets[currentSetSelection] = copiedSubBoardData;

			// Refresh display
			changeSetSelection(currentSetSelection, true);

			// Mark that there are changes
			TerpstraSysExApplication::getApp().setHasChangesToSave(true);
		}
		return true;
	}
	else
		return false;
}

void MainContentComponent::handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message)
{
	if (message.isController())
	{
		// Established that a controller change has occurred, which is due
		// to a keyboard macro button being pressed. Now Channel will hold
		// a value from 0 to 15, and InData1 (controller number) will hold either 16 or 17.
		// The formula to establish which button is pressed is
		// ButtonNum = channel*2 + (InData1 - 16)
		// One button subcomponent holds 2 buttons
		int buttonSubwinIndex = message.getChannel() - 1;
		if (buttonSubwinIndex >= 0 && buttonSubwinIndex < 5)
		{
			if (message.getControllerNumber() == 16 || message.getControllerNumber() == 17)
			{
				// Highlight controller button on/off. Left side, if controller == 17, right side, if it is 16
				macroButtons[buttonSubwinIndex]->setIsSelected(
					message.getControllerNumber() == 17 ? TerpstraMacroButton::leftbutton : TerpstraMacroButton::rightbutton,
					message.getControllerValue() >= 0x3f);

				// Send parametrization file to controller, if one is specified
				noteEditArea->handleIncomingMidiMessage(source, message);
			}
		}
	}
}

void MainContentComponent::paint (Graphics& g)
{
	g.fillAll(findColour(ResizableWindow::backgroundColourId));
}

void MainContentComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

	int newHeight = getHeight();

	// New height of subset field area, with minimal value
	float newSubsetAreaHeight = jmax(newHeight - MIDIEDITAREAHEIGHT - EDITFUNCTIONAREAHEIGHT, MINIMALTERPSTRAKEYSETAREAHEIGHT);

	// Resize factor for the subset field area and the subset fields
	double newResizeFactor = (double)newSubsetAreaHeight * 1.1 / DEFAULTSUBSETAREAHEIGHT;
	jassert(newResizeFactor > 0.0);
	double newDecreaseFactor = jmin(newResizeFactor, 1.0);
	jassert(newDecreaseFactor > 0.0);

	// New position, width and height of subset fields
	float newSubsetFirstYPos = TERPSTRAKEYSETFLDFIRSTYPOS * newDecreaseFactor;
	float newSubsetWidth = DEFAULTTERPSTRAKEYSETWIDTH * newDecreaseFactor;
	float newSubsetHeight = DEFAULTTERPSTRAKEYSETHEIGHT * newDecreaseFactor;
	float newSubsetXIncrement = DEFAULTTERPSTRAKEYSETXINCREMENT * newDecreaseFactor;

	// New position, width and height of macro buttons
	float newFirstMacrobuttonColPos = DEFAULTFIRSTMACROBUTTONCOLPOS * newDecreaseFactor;
	float newMacroButtonWidth = DEFAULTMACROBUTTONWIDTH * newDecreaseFactor;
	float newMacroButtonHeight = DEFAULTMACROBUTTONHEIGHT * newDecreaseFactor;

	float newMidiEditFirstYPos = newSubsetAreaHeight;
	float newSingleKeyFieldFirstYPos = newSubsetAreaHeight + TERPSTRASINGLEKEYFIELDRIMABOVE * newDecreaseFactor;

	// Key set fields
	for (int i = 0; i < NUMBEROFBOARDS; i++)
	{
		// Macro button
		macroButtons[i]->setBounds(newFirstMacrobuttonColPos + i*newSubsetXIncrement, MACROBUTTONYPOS, newMacroButtonWidth, newMacroButtonHeight);

		// Paint set fields from right to left
		// (This will not matter any more when the images' backgrounds are transparent)
		terpstraSetSelectors[4 - i]->setBounds(
            roundToInt(MAINWINDOWFIRSTCOLPOS + (4 - i)*newSubsetXIncrement),
            roundToInt(newSubsetFirstYPos), newSubsetWidth, newSubsetHeight);
	}

	// Single Key fields

	// Transformation Rotate slightly counterclockwise
	float x = MAINWINDOWFIRSTCOLPOS;
	float y = newSingleKeyFieldFirstYPos;
	AffineTransform transform = AffineTransform::translation(-x, -y);
	transform = transform.rotated(TERPSTRASINGLEKEYROTATIONANGLE);
	transform = transform.translated(x, y);

	int keyIndex = 0;

	// Rows
	int rowCount = boardGeometry.horizontaLineCount();
	for (int rowIndex = 0; rowIndex < rowCount; rowIndex++)
	{
		float xbasepos;
		if (rowIndex % 2 == 0)
			xbasepos = MAINWINDOWFIRSTCOLPOS;
		else
			xbasepos = MAINWINDOWFIRSTCOLPOS + TERPSTRASINGLEKEYFLDSIZE / 2;

		int ybasepos = newSingleKeyFieldFirstYPos + 3 * rowIndex * TERPSTRASINGLEKEYFLDSIZE / 4;

		int subBoardRowSize = boardGeometry.horizontalLineSize(rowIndex);
		for (int posInRow = 0; posInRow < subBoardRowSize; posInRow++)
		{
			x = xbasepos + (boardGeometry.firstColumnOffset(rowIndex) + posInRow)*TERPSTRASINGLEKEYFLDSIZE;
			y = ybasepos;
			transform.transformPoint(x, y);
			terpstraKeyFields[keyIndex]->setBounds(roundToInt(x), roundToInt(y), TERPSTRASINGLEKEYFLDSIZE, TERPSTRASINGLEKEYFLDSIZE);

			keyIndex++;
		}
	}

	jassert(TERPSTRABOARDSIZE == keyIndex);

	// Midi input + output
	midiEditArea->setBounds(EDITAREAFIRSTCOLPOS, newMidiEditFirstYPos, EDITAREAWIDTH, MIDIEDITAREAHEIGHT);

	// Edit function area
	noteEditArea->setBounds(EDITAREAFIRSTCOLPOS, newMidiEditFirstYPos + MIDIEDITAREAHEIGHT, EDITAREAWIDTH, EDITFUNCTIONAREAHEIGHT);
}

void MainContentComponent::buttonClicked(Button *button)
{
	if (midiEditArea->isSendAllButton(button))
	{
		TerpstraSysExApplication::getApp().getMidiDriver().sendAndSaveCompleteMapping(mappingData);
	}
	/*
	else
	{
		for (int i = 0; i < NUMBEROFBOARDS; i++)
		{
			if (button == terpstraSetSelectors[i])
			{
				changeSetSelection(i);
				break;
			}
		}
	}
	*/
}

void MainContentComponent::mouseDown(const MouseEvent &event)
{
	bool mappingChanged = false;

	// Selection of subset components
	auto eventComponentParent = event.eventComponent->getParentComponent();
	for (int i = 0; i < NUMBEROFBOARDS; i++)
	{
		if (event.eventComponent == terpstraSetSelectors[i].get() || eventComponentParent == terpstraSetSelectors[i].get())
		{
			changeSetSelection(i);
			return;
		}
	}

	// Selection of single key fields
	for (int i = 0; i < TERPSTRABOARDSIZE; i++)
	{
		if (event.eventComponent == terpstraKeyFields[i].get() || event.eventComponent->getParentComponent() == terpstraKeyFields[i].get())
		{
			// Select field
			changeSingleKeySelection(i);

			// Perform the edit, according to edit mode. Including sending to device
			mappingChanged = this->noteEditArea->performMouseDown(currentSetSelection, i);

			// Refresh display
			changeSetSelection(currentSetSelection, true);

			// Mark that there are changes
			if (mappingChanged)
				TerpstraSysExApplication::getApp().setHasChangesToSave(true);

			break;
		}
	}
}

void MainContentComponent::mouseUp(const MouseEvent &event)
{
	bool mappingChanged = false;

	// Selection of single key fields
	for (int i = 0; i < TERPSTRABOARDSIZE; i++)
	{
		if (event.eventComponent == terpstraKeyFields[i].get() || event.eventComponent->getParentComponent() == terpstraKeyFields[i].get())
		{
			// Perform the edit, according to edit mode. Including sending to device
			mappingChanged = this->noteEditArea->performMouseUp(currentSetSelection, i);

			// Refresh display
			changeSetSelection(currentSetSelection, true);

			// Mark that there are changes
			if (mappingChanged)
				TerpstraSysExApplication::getApp().setHasChangesToSave(true);

			break;
		}
	}
}

void MainContentComponent::changeSetSelection(int newSelection, bool forceRefresh)
{
	if (newSelection != currentSetSelection || forceRefresh)
	{
		// Unselect previous set
		// saving the data was done in click event on the single key fields
		if (currentSetSelection >= 0 && currentSetSelection < NUMBEROFBOARDS)
			terpstraSetSelectors[currentSetSelection]->setIsSelected(false);

		// Set data of new selection
		if (newSelection >= 0 && newSelection < NUMBEROFBOARDS )
		{
			for (int i = 0; i < TERPSTRABOARDSIZE; i++)
				terpstraKeyFields[i]->setValue(mappingData.sets[newSelection].theKeys[i]);
		}

		currentSetSelection = newSelection;
	}

	// Set toggle state in any case (override default imagebutton functionality)
	if ( currentSetSelection >= 0 && currentSetSelection < NUMBEROFBOARDS )
		terpstraSetSelectors[currentSetSelection]->setIsSelected(true);
}

void MainContentComponent::changeSingleKeySelection(int newSelection)
{
	// Unselect previous key
	if (currentSingleKeySelection >= 0 && currentSingleKeySelection < TERPSTRABOARDSIZE )
		terpstraKeyFields[currentSingleKeySelection]->setIsSelected(false);

	// Select new key
	currentSingleKeySelection = newSelection;
	if (currentSingleKeySelection >= 0 && currentSingleKeySelection < TERPSTRABOARDSIZE)
		terpstraKeyFields[currentSingleKeySelection]->setIsSelected(true);
}
