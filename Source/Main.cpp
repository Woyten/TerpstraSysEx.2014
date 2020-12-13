/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic startup code for a Juce application.
	Created: xx.xx.2014

  ==============================================================================
*/

#include "Main.h"
#include "GeneralOptionsDlg.h"
#include "VelocityCurveDlgBase.h"
#include "NoteOnOffVelocityCurveDialog.h"

//==============================================================================

TerpstraSysExApplication::TerpstraSysExApplication()
	: tooltipWindow(), hasChangesToSave(false)
{
	PropertiesFile::Options options;
	options.applicationName = "LumatoneSetup";
	options.filenameSuffix = "settings";
	options.osxLibrarySubFolder = "Application Support";
#if JUCE_LINUX
	options.folderName = "~/.config/LumatoneSetup";
#else
	options.folderName = "LumatoneSetup";
#endif
	propertiesFile = new PropertiesFile(options);
	jassert(propertiesFile != nullptr);

	int manufacturerId = propertiesFile->getIntValue("ManufacturerId", 0x002150);
	midiDriver.setManufacturerId(manufacturerId);

	// Colour scheme
	lookAndFeel.setColourScheme(lookAndFeel.getDarkColourScheme());

	lookAndFeel.setColour(juce::ComboBox::arrowColourId, Colour(0xfff7990d));
	lookAndFeel.setColour(juce::ToggleButton::tickColourId, Colour(0xfff7990d));
	// ToDo TabbedButton colours: selected, unselected

	lookAndFeel.setColour(TerpstraKeyEdit::backgroundColourId, lookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));
	lookAndFeel.setColour(TerpstraKeyEdit::outlineColourId, Colour(0xffd7d9da));
	lookAndFeel.setColour(TerpstraKeyEdit::selectedKeyOutlineId, Colour(0xfff7990d));

	lookAndFeel.setColour(VelocityCurveBeam::beamColourId, Colour(0x66ff5e00));
	lookAndFeel.setColour(VelocityCurveBeam::outlineColourId, Colour(0xffd7d9da));

	// Recent files list
	recentFiles.restoreFromString ( propertiesFile->getValue("RecentFiles") );
	recentFiles.removeNonExistentFiles();

	// State of main window will be read from properties file when main window is created
}

//==============================================================================
void TerpstraSysExApplication::initialise(const String& commandLine)
{
    // This method is where you should put your application's initialisation code..
	//commandManager.reset(new ApplicationCommandManager());
	//commandManager->registerAllCommandsForTarget(this);


    mainWindow.reset(new MainWindow());
	//mainWindow->addKeyListener(commandManager->getKeyMappings());

	((MainContentComponent*)(mainWindow->getContentComponent()))->restoreStateFromPropertiesFile(propertiesFile);

	// commandLine: may contain a file name
	if (!commandLine.isEmpty())
	{
		// commandLine is supposed to contain a file name. Try to open it.
		currentFile = File(commandLine);
		if (!currentFile.existsAsFile())
		{
			// If file name is with quotes, try removing the quotes
			if (commandLine.startsWithChar('"') && commandLine.endsWithChar('"'))
				currentFile = File(commandLine.substring(1, commandLine.length() - 1));
		}

		openFromCurrentFile();
	}
}

void TerpstraSysExApplication::shutdown()
{
    // Add your application's shutdown code here..

	// Save recent files list
	recentFiles.removeNonExistentFiles();
	jassert(propertiesFile != nullptr);
	propertiesFile->setValue("RecentFiles", recentFiles.toString());

	// Save state of main window
	((MainContentComponent*)(mainWindow->getContentComponent()))->saveStateToPropertiesFile(propertiesFile);

	propertiesFile->saveIfNeeded();
	delete propertiesFile;
	propertiesFile = nullptr;

    mainWindow = nullptr; // (deletes our window)
	//commandManager = nullptr;
}

//==============================================================================
void TerpstraSysExApplication::systemRequestedQuit()
{
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app to close.

	// If there are changes: ask for save
	if (hasChangesToSave)
	{
		int retc = AlertWindow::showYesNoCancelBox(AlertWindow::AlertIconType::QuestionIcon, "Quitting the application", "Do you want to save your changes?");
		if (retc == 0)
		{
			// "Cancel". Do not quit.
			return;
		}
		else if (retc == 1)
		{
			// "Yes". Try to save. Canvel if unsuccessful
			if (!saveSysExMapping())
				return;
		}
		// retc == 2: "No" -> end without saving
	}

	quit();
}

void TerpstraSysExApplication::anotherInstanceStarted(const String& commandLine)
{
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.
}

bool TerpstraSysExApplication::openSysExMapping()
{
	FileChooser chooser("Open a Lumatone key mapping", recentFiles.getFile(0).getParentDirectory(), "*.ltn");
	if (chooser.browseForFileToOpen())
	{
		currentFile = chooser.getResult();
		return openFromCurrentFile();
	}
	return true;
}

bool TerpstraSysExApplication::saveSysExMapping()
{
	if (currentFile.getFileName().isEmpty())
		return saveSysExMappingAs();
	else
		return saveCurrentFile();

}

bool TerpstraSysExApplication::saveSysExMappingAs()
{
	FileChooser chooser("Lumatone Key Mapping Files", recentFiles.getFile(0).getParentDirectory(), "*.ltn");
	if (chooser.browseForFileToSave(true))
	{
		currentFile = chooser.getResult();
		if (saveCurrentFile() )
		{
			// Window title
			updateMainTitle();
			return true;
		}
	}

	return false;
}

bool TerpstraSysExApplication::resetSysExMapping()
{
	// Clear file
	currentFile = File();

	// Clear all edit fields
	((MainContentComponent*)(mainWindow->getContentComponent()))->deleteAll();

	setHasChangesToSave(false);

	// Window title
	updateMainTitle();

	return true;
}

bool TerpstraSysExApplication::deleteSubBoardData()
{
	return ((MainContentComponent*)(mainWindow->getContentComponent()))->deleteCurrentSubBoardData();
}

bool TerpstraSysExApplication::copySubBoardData()
{
	return ((MainContentComponent*)(mainWindow->getContentComponent()))->copyCurrentSubBoardData();
}

bool TerpstraSysExApplication::pasteSubBoardData()
{
	return ((MainContentComponent*)(mainWindow->getContentComponent()))->pasteCurrentSubBoardData();
}

bool TerpstraSysExApplication::generalOptionsDialog()
{
	GeneralOptionsDlg* optionsWindow = new GeneralOptionsDlg();
	optionsWindow->setLookAndFeel(&lookAndFeel);

	DialogWindow::LaunchOptions launchOptions;
	launchOptions.content.setOwned(optionsWindow);
	launchOptions.content->setSize(480, 240);

	launchOptions.dialogTitle = "General options";
	launchOptions.dialogBackgroundColour = lookAndFeel.findColour(juce::ResizableWindow::backgroundColourId);
	launchOptions.escapeKeyTriggersCloseButton = true;
	launchOptions.useNativeTitleBar = false;
	launchOptions.resizable = true;

	DialogWindow* dw = launchOptions.launchAsync();
	dw->centreWithSize(480, 240);

	return true;
}

bool TerpstraSysExApplication::noteOnOffVelocityCurveDialog()
{
	NoteOnOffVelocityCurveDialog* velocityCurveWindow = new NoteOnOffVelocityCurveDialog();
	velocityCurveWindow->setLookAndFeel(&lookAndFeel);

	int dlgWidth = propertiesFile->getIntValue("VelocityCurveWindowWidth", 648);
	int dlgHeight = propertiesFile->getIntValue("VelocityCurveWindowHeight", 424);

	DialogWindow::LaunchOptions launchOptions;
	launchOptions.content.setOwned(velocityCurveWindow);
	launchOptions.content->setSize(dlgWidth, dlgHeight);

	launchOptions.dialogTitle = "Note on/off velocity curve";
	launchOptions.dialogBackgroundColour = lookAndFeel.findColour(ResizableWindow::backgroundColourId);
	launchOptions.escapeKeyTriggersCloseButton = true;
	launchOptions.useNativeTitleBar = false;
	launchOptions.resizable = true;

	DialogWindow* dw = launchOptions.launchAsync();
	dw->centreWithSize(dlgWidth, dlgHeight);

	return true;
}

bool TerpstraSysExApplication::faderVelocityCurveDialog()
{
	VelocityCurveDlgBase* velocityCurveWindow = new VelocityCurveDlgBase(TerpstraMidiDriver::VelocityCurveType::fader);
	velocityCurveWindow->setLookAndFeel(&lookAndFeel);

	int dlgWidth = propertiesFile->getIntValue("FaderVelocityCurveWindowWidth", 648);
	int dlgHeight = propertiesFile->getIntValue("FaderVelocityCurveWindowHeight", 424);

	DialogWindow::LaunchOptions launchOptions;
	launchOptions.content.setOwned(velocityCurveWindow);
	launchOptions.content->setSize(dlgWidth, dlgHeight);

	launchOptions.dialogTitle = "Fader velocity curve";
	launchOptions.dialogBackgroundColour = lookAndFeel.findColour(ResizableWindow::backgroundColourId);
	launchOptions.escapeKeyTriggersCloseButton = true;
	launchOptions.useNativeTitleBar = false;
	launchOptions.resizable = true;

	DialogWindow* dw = launchOptions.launchAsync();
	dw->centreWithSize(dlgWidth, dlgHeight);

	return true;
}

bool TerpstraSysExApplication::aftertouchVelocityCurveDialog()
{
	VelocityCurveDlgBase* velocityCurveWindow = new VelocityCurveDlgBase(TerpstraMidiDriver::VelocityCurveType::afterTouch);
	velocityCurveWindow->setLookAndFeel(&lookAndFeel);

	int dlgWidth = propertiesFile->getIntValue("AftertouchVelocityCurveWindowWidth", 768);
	int dlgHeight = propertiesFile->getIntValue("AftertouchVelocityCurveWindowHeight", 424);

	DialogWindow::LaunchOptions launchOptions;
	launchOptions.content.setOwned(velocityCurveWindow);
	launchOptions.content->setSize(dlgWidth, dlgHeight);

	launchOptions.dialogTitle = "Aftertouch parameters";
	launchOptions.dialogBackgroundColour = lookAndFeel.findColour(ResizableWindow::backgroundColourId);
	launchOptions.escapeKeyTriggersCloseButton = true;
	launchOptions.useNativeTitleBar = false;
	launchOptions.resizable = true;

	DialogWindow* dw = launchOptions.launchAsync();
	dw->centreWithSize(dlgWidth, dlgHeight);

	return true;
}

// open a file from the "recent files" menu
bool TerpstraSysExApplication::openRecentFile(int recentFileIndex)
{
	jassert(recentFileIndex >= 0 && recentFileIndex < recentFiles.getNumFiles());
	currentFile = recentFiles.getFile(recentFileIndex);
	return openFromCurrentFile();
}

// Open a SysEx mapping from the file specified in currentFile
bool TerpstraSysExApplication::openFromCurrentFile()
{
	if (currentFile.existsAsFile())
	{
		// XXX StringArray format: platform-independent?
		StringArray stringArray;
		currentFile.readLines(stringArray);
		TerpstraKeyMapping keyMapping;
		keyMapping.fromStringArray(stringArray);

		((MainContentComponent*)(mainWindow->getContentComponent()))->setData(keyMapping);

		// Window title
		updateMainTitle();

		// Send configuration to controller, if connected
		sendCurrentMappingToDevice();

		// Mark file as unchanged
		setHasChangesToSave(false);

		// Add file to recent files list
		recentFiles.addFile(currentFile);

		return true;
	}
	else
	{
		// Show error message
		AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon, "Open File Error", "The file " + currentFile.getFullPathName() + " could not be opened.");

		// XXX Update Window title in any case? Make file name empty/make data empty in case of error?
		return false;
	}
}

// Saves the current mapping to file, specified in currentFile.
bool TerpstraSysExApplication::saveCurrentFile()
{
	if (currentFile.existsAsFile())
		currentFile.deleteFile();
	bool retc = currentFile.create();
	// XXX error handling

	TerpstraKeyMapping keyMapping;
	((MainContentComponent*)(mainWindow->getContentComponent()))->getData(keyMapping);

	StringArray stringArray = keyMapping.toStringArray();
	for (int i = 0; i < stringArray.size(); i++)
		currentFile.appendText(stringArray[i] + "\n");

	setHasChangesToSave(false);

	// Add file to recent files list - or put it on top of the list
	recentFiles.addFile(currentFile);

	return retc;
}

void TerpstraSysExApplication::sendCurrentMappingToDevice()
{
	auto theConfig = ((MainContentComponent*)(mainWindow->getContentComponent()))->getMappingInEdit();
	
	// MIDI channel, MIDI note, colour and key type config for all keys
	getMidiDriver().sendCompleteMapping(theConfig);

	// General options
	getMidiDriver().sendAfterTouchActivation(theConfig.afterTouchActive);
	getMidiDriver().sendLightOnKeyStrokes(theConfig.lightOnKeyStrokes);
	getMidiDriver().sendInvertFootController(theConfig.invertFootController);
	getMidiDriver().sendExpressionPedalSensivity(theConfig.expressionControllerSensivity);

	// Velocity curve config
	TerpstraSysExApplication::getApp().getMidiDriver().sendVelocityIntervalConfig(theConfig.velocityIntervalTableValues);	
	// ToDo Note on/off velocity configuration
	// ToDo Fader configuration
	// ToDo Aftertouch configuration
}

void TerpstraSysExApplication::updateMainTitle()
{
	String windowTitle("Lumatone Setup Utility");
	if (!currentFile.getFileName().isEmpty() )
		windowTitle << " - " << currentFile.getFileName();
	if (hasChangesToSave)
		windowTitle << "*";
	mainWindow->setName(windowTitle);
}

void TerpstraSysExApplication::setHasChangesToSave(bool value)
{
	if (value != hasChangesToSave)
	{
		hasChangesToSave = value;
		updateMainTitle();
	}
}

bool TerpstraSysExApplication::aboutTerpstraSysEx()
{
	String m;

	m << "Lumatone Keyboard Setup Utility" << newLine
		<< newLine
		<< "Version " << String((JUCE_APP_VERSION_HEX >> 16) & 0xff) << "."
		<< String((JUCE_APP_VERSION_HEX >> 8) & 0xff) << "."
		<< String(JUCE_APP_VERSION_HEX & 0xff) << newLine

		<< newLine
		<< "Original design @ Dylan Horvath 2007" << newLine
		<< "Reengineered @ Hans Straub 2014 - 2020" << newLine
		<< "Scale structure editor @ Vincenzo Sicurella" << newLine
		<< "Mac version by Brett Park" << newLine
		<< newLine
		<< "For help on using this program, or any questions relating to the Lumatone keyboard, go to http://lumatone.io or http://terpstrakeyboard.com .";

	DialogWindow::LaunchOptions options;
	Label* label = new Label();
	label->setLookAndFeel(&lookAndFeel);
	label->setText(m, dontSendNotification);
	options.content.setOwned(label);

	Rectangle<int> area(0, 0, 400, 200);
	options.content->setSize(area.getWidth(), area.getHeight());

	options.dialogTitle = "About LumatoneSetup";
	options.dialogBackgroundColour = lookAndFeel.findColour(ResizableWindow::backgroundColourId);

	options.escapeKeyTriggersCloseButton = true;
	options.useNativeTitleBar = false;
	options.resizable = true;

	//const RectanglePlacement placement(RectanglePlacement::xRight + RectanglePlacement::yBottom + RectanglePlacement::doNotResize);

	DialogWindow* dw = options.launchAsync();
	dw->centreWithSize(400, 260);

	return true;
}


//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (TerpstraSysExApplication)
