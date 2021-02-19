/*
  ==============================================================================

    ColourPaletteComponent.cpp
    Created: 18 Dec 2020 1:48:01am
    Author:  Vincenzo

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ColourPaletteComponent.h"

//==============================================================================
// ColourPaletteComponent Definitions

ColourPaletteComponent::ColourPaletteComponent(String name)
    : TenHexagonPalette()
{
    setName(name);
    setColourPalette(Array<Colour>());
}

ColourPaletteComponent::ColourPaletteComponent(String name, Array<Colour>& colours)
    : TenHexagonPalette(), referencedPalette(&colours)
{
    setName(name);
    setColourPalette(colours);
}

ColourPaletteComponent::~ColourPaletteComponent()
{

}

//==========================================================================

void ColourPaletteComponent::setSelectedSwatchNumber(int swatchIndex)
{
    Palette::setSelectedSwatchNumber(swatchIndex);
    selectorListeners.call(&ColourSelectionListener::colourChangedCallback, this, getSelectedSwatchColour());
}

void ColourPaletteComponent::setColourPalette(Array<Colour> colourPaletteIn)
{
    if (colourPaletteIn.size() == 0)
    {
        for (int i = 0; i < getNumberOfSwatches(); i++)
            colourPaletteIn.add(Colour(0xff1b1b1b));

        setEnabled(false);
    }
    else
    {
        setEnabled(true);
    }

    Palette::setColourPalette(colourPaletteIn);

    if (referencedPalette)
        *referencedPalette = colourPaletteIn;

    if (isEnabled())
    {
        int selectedSwatch = getSelectedSwatchNumber();
        if (selectedSwatch >= 0 && selectedSwatch < getNumberOfSwatches())
            selectorListeners.call(&ColourSelectionListener::colourChangedCallback, this, getSelectedSwatchColour());
    }
    else
    {
        deselectColour();
    }
}

void ColourPaletteComponent::setSwatchColour(int swatchNumber, Colour newColour)
{
    Palette::setSwatchColour(swatchNumber, newColour);
    
    // Edit referenced palette
    if (referencedPalette)
        referencedPalette->set(swatchNumber, newColour);

    if (getSelectedSwatchNumber() == swatchNumber)
        selectorListeners.call(&ColourSelectionListener::colourChangedCallback, this, getSelectedSwatchColour());
}

//==========================================================================

Colour ColourPaletteComponent::getSelectedColour()
{
    return Palette::getSelectedSwatchColour();
}

void ColourPaletteComponent::deselectColour()
{
    Palette::setSelectedSwatchNumber(-1);
}

//==============================================================================
// PaletteControlGroup Definitions

PaletteControlGroup::PaletteControlGroup(LumatoneEditorColourPalette& paletteIn)
    : palette("Palette" + paletteIn.name, *paletteIn.palette),
    editButton("EditButton_" + paletteIn.name, translate("EditButtonTip")),
    trashButton("TrashButton" + paletteIn.name)
{
    editButton.setButtonText("Edit");
    editButton.getProperties().set(LumatoneEditorStyleIDs::textButtonHyperlinkFlag, 1);

    const Image trashIcon = ImageCache::getFromHashCode(LumatoneEditorAssets::TrashCanIcon);
    trashButton.setImages(false, true, true,
        trashIcon, 1.0f, Colour(),
        trashIcon, 1.0f, Colours::white.withAlpha(0.4f),
        trashIcon, 1.0f, Colour()
    );
}