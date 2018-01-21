/*
  ==============================================================================

    ViewComponents.cpp
    Created: 16 Jun 2014 7:20:34pm
    Author:  hsstraub

  ==============================================================================
*/

#include "ViewComponents.h"
#include "ViewConstants.h"

/*
==============================================================================
TerpstraKeyEdit class
==============================================================================
*/

TerpstraKeyEdit::TerpstraKeyEdit()
	: isSelected(false), keyColour(0)
{
	midiNoteLabel = new Label("midiNoteLabel", "0");
	addAndMakeVisible(midiNoteLabel);
	midiNoteLabel->setBounds((TERPSTRASINGLEKEYFLDSIZE - 30) / 2, TERPSTRASINGLEKEYFLDSIZE / 2 - 15, 30, STANDARDLABELHEIGTH);
	midiNoteLabel->setFont( midiNoteLabel->getFont().boldened());

	midiChannelLabel = new Label("midiChannelLabel", "0");
	addAndMakeVisible(midiChannelLabel);
	midiChannelLabel->setBounds((TERPSTRASINGLEKEYFLDSIZE - 25) / 2, TERPSTRASINGLEKEYFLDSIZE / 2, 25, STANDARDLABELHEIGTH);
}

TerpstraKeyEdit::~TerpstraKeyEdit()
{
	deleteAllChildren();
}

TerpstraKey TerpstraKeyEdit::getValue() const
{
	TerpstraKey newValue;
	newValue.noteNumber = midiNoteLabel->getText().getIntValue();
	newValue.channelNumber = midiChannelLabel->getText().getIntValue();
	newValue.colour = keyColour;

	return newValue;
}

void TerpstraKeyEdit::setValue(TerpstraKey newValue)
{
	midiNoteLabel->setText(String(newValue.noteNumber), juce::NotificationType::sendNotification);
	midiChannelLabel->setText(String(newValue.channelNumber), juce::NotificationType::sendNotification);
	keyColour = newValue.colour;

	repaint();
}

void TerpstraKeyEdit::setIsSelected(bool newValue)
{
	if (this->isSelected != newValue)
	{
		this->isSelected = newValue;
		repaint();
	}
}

void TerpstraKeyEdit::paint(Graphics& g)
{
	// Both values are set in calling function when constructing this, are supposed to be TERPSTRASINGLEKEYFLDSIZE
	float w = this->getWidth();
	float h = this->getHeight();

	// Selected or not: color and thickness of the line
	float lineWidth = isSelected ? TERPSTRASELECTEDKEYFLDLINEWIDTH : TERPSTRASINGLEKEYFLDLINEWIDTH;
	juce::Colour lineColor = isSelected ? Colour(TERPSTRASELECTEDFLDLINECOLOUR) : Colours::black;

	// Draw hexagon
	Path hexPath;
	hexPath.startNewSubPath(w / 2.0f, lineWidth);
	hexPath.lineTo(w - lineWidth, h / 4.0f);
	hexPath.lineTo(w - lineWidth, 3.0f * h / 4.0f);
	hexPath.lineTo(w / 2.0f, h - lineWidth);
	hexPath.lineTo(lineWidth, 3.0f * h / 4.0f);
	hexPath.lineTo(lineWidth, h / 4.0f);
	hexPath.closeSubPath();

	// Rotate slightly counterclockwise around the center
	AffineTransform transform = AffineTransform::translation(-w / 2.0f, -h / 2.0f);
	transform = transform.rotated(TERPSTRASINGLEKEYROTATIONANGLE);
	transform = transform.translated(w / 2.0f, h / 2.0f);
	
	hexPath.applyTransform(transform);
	hexPath.scaleToFit(lineWidth, lineWidth, w - lineWidth, h - lineWidth, true);
	// Color: empty or the parametrized color
	TerpstraKey currentValue = getValue();

	// Parametrized colour
	g.setColour(Colour(MAINWINDOWBGCOLOUR).overlaidWith(Colour(currentValue.colour).withAlpha((uint8)0x40)));

	g.fillPath(hexPath);

	// Draw line
	g.setColour(lineColor);
	g.strokePath(hexPath, PathStrokeType(lineWidth));

	// Something parametrized or not?  
	if (currentValue.isEmpty())
	{
		midiChannelLabel->setAlpha(0.3);
		midiNoteLabel->setAlpha(0.3);
	}
	else
	{
		midiChannelLabel->setAlpha(1.0);
		midiNoteLabel->setAlpha(1.0);
	}
}

void TerpstraKeyEdit::resized()
{
}

/*
==============================================================================
TerpstraKeySetEdit class
==============================================================================
*/

TerpstraKeySetEdit::TerpstraKeySetEdit()
{
	Image imgUnselected = ImageCache::getFromMemory(BinaryData::OctaveGraphic_png, BinaryData::OctaveGraphic_pngSize);
	//Image imgSelected = ImageCache::getFromMemory(BinaryData::OctaveOutline_png, BinaryData::OctaveOutline_pngSize);

	setImages(true, true, true,
		imgUnselected, 0.3f, Colours::transparentBlack,
		imgUnselected, 0.6f, Colours::transparentBlack,
		imgUnselected, 0.9f, Colour(MAINWINDOWSELECTEDCOLOUR),
		0.5f);

	setClickingTogglesState(true);
}

TerpstraKeySetEdit::~TerpstraKeySetEdit()
{

}

/*
==============================================================================
ColourComboBox class
==============================================================================
*/

ColourComboBox::ColourComboBox(const String& componentName) : ComboBox(componentName)
{
}

void ColourComboBox::setTextFieldToColourObject(Colour newColour,	NotificationType notification)
{
	setText(newColour.toDisplayString(false));
	
	// XXX Add to box
}

int ColourComboBox::getColourIDFromText(bool addToBox)
{
	String colourString = getText();
	
	// XXX validation of colour value
	int colourID = colourString.getHexValue32();

	if (addToBox)
	{
		// Add colour to combo box
		int pos;
		for (pos = 0; pos < getNumItems(); pos++)
		{
			if (getItemText(pos) == colourString)
				break;
		}

		if (pos >= getNumItems())
		{
			// Colour is not in list yet - add it
			addItem(colourString, pos + 1);
		}
	}

	return colourID;
}

Colour ColourComboBox::getColourObjectFromText(bool addToBox)
{
	int colourID = getColourIDFromText(addToBox);
	return Colour(colourID);
}