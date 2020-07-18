/*
  ==============================================================================

    MainMenu.cpp
    Created: 13 Jun 2014 7:59:18pm
    Author:  hsstraub

  ==============================================================================
*/

#include "Main.h"
#include "MainMenu.h"

TerpstraSysExMainMenuModel::TerpstraSysExMainMenuModel(ApplicationCommandManager* commandManager)
{
	theManager = commandManager;
	setApplicationCommandManagerToWatch(commandManager);

}

StringArray TerpstraSysExMainMenuModel::getMenuBarNames()
{
	const char* const names[] = { "File", "Edit", "View", "Options", "Help", nullptr };
	return StringArray(names);
}

void TerpstraSysExMainMenuModel::createFileMenu(PopupMenu& menu)
{
	menu.addCommandItem(theManager, openSysExMapping);
	menu.addCommandItem(theManager, saveSysExMapping);
	menu.addCommandItem(theManager, saveSysExMappingAs);
	menu.addCommandItem(theManager, resetSysExMapping);

	menu.addSeparator();

	PopupMenu recentFilesMenu;
	TerpstraSysExApplication::getApp().getRecentFileList().createPopupMenuItems(recentFilesMenu, recentFilesBaseID, true, true);
	menu.addSubMenu("Recent Files", recentFilesMenu);

#if ! JUCE_MAC
	menu.addSeparator();
	menu.addCommandItem(theManager, StandardApplicationCommandIDs::quit);
#endif
}

void TerpstraSysExMainMenuModel::createEditMenu(PopupMenu& menu)
{
	menu.addCommandItem(theManager, deleteOctaveBoard);
	menu.addCommandItem(theManager, copyOctaveBoard);
	menu.addCommandItem(theManager, pasteOctaveBoard);
}

void TerpstraSysExMainMenuModel::createViewMenu(PopupMenu& menu)
{
	PopupMenu colourSchemesMenu;
	colourSchemesMenu.addCommandItem(theManager, lightColourScheme);
	colourSchemesMenu.addCommandItem(theManager, darkColourScheme);

	menu.addSubMenu("Colour Scheme", colourSchemesMenu);
}

void TerpstraSysExMainMenuModel::createOptionsMenu(PopupMenu& menu)
{
	menu.addCommandItem(theManager, generalOptions);
	menu.addCommandItem(theManager, noteOnOffVelocityCurve);
	menu.addCommandItem(theManager, faderVelocityCurve);
	menu.addCommandItem(theManager, aftertouchVelocityCurve);
}

void TerpstraSysExMainMenuModel::createHelpMenu(PopupMenu& menu)
{
	menu.addCommandItem(theManager, aboutSysEx);
}

PopupMenu TerpstraSysExMainMenuModel::getMenuForIndex(int topLevelMenuIndex, const String& menuName)
{
	PopupMenu menu;

	if (menuName == "File")             createFileMenu(menu);
	else if (menuName == "Edit")		createEditMenu(menu);
	else if (menuName == "View")		createViewMenu(menu);
	else if (menuName == "Options")		createOptionsMenu(menu);
	else if (menuName == "Help")		createHelpMenu(menu);
	else                                jassertfalse; // names have changed?

	return menu;
}

void TerpstraSysExMainMenuModel::menuItemSelected(int menuItemID, int topLevelMenuIndex)
{
	if (menuItemID >= recentFilesBaseID && menuItemID < recentFilesBaseID + 100)
	{
		// open a file from the "recent files" menu
		TerpstraSysExApplication::getApp().openRecentFile(menuItemID - recentFilesBaseID);
	}
}
