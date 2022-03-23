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
#include "LumatoneMenu.h"

//==============================================================================

MainContentComponent* TerpstraSysExApplication::getMainContentComponent() const
{
	jassert(mainWindow != nullptr);
	return (MainContentComponent*)(mainWindow->getContentComponent());
}

TerpstraSysExApplication::TerpstraSysExApplication()
	: lookAndFeel(appFonts.fonts, true), tooltipWindow(), hasChangesToSave(false)
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

	lumatoneController = std::make_unique<LumatoneController>();

	// Localisation
	String localisation = getLocalisation(SystemStats::getDisplayLanguage());
	LocalisedStrings::setCurrentMappings(new LocalisedStrings(localisation, false));
	LocalisedStrings::getCurrentMappings()->setFallback(new LocalisedStrings(BinaryData::engb_txt, false));

	// Window aspect ratio
	boundsConstrainer.reset(new ComponentBoundsConstrainer());
	boundsConstrainer->setFixedAspectRatio(DEFAULTMAINWINDOWASPECT);
	boundsConstrainer->setMinimumSize(800, round(800 / DEFAULTMAINWINDOWASPECT));

	// Colour scheme
	//lookAndFeel.setColourScheme(lookAndFeel.getDarkColourScheme());

	//lookAndFeel.setColour(juce::ComboBox::arrowColourId, Colour(0xfff7990d));
	//lookAndFeel.setColour(juce::ToggleButton::tickColourId, Colour(0xfff7990d));

	lookAndFeel.setColour(TerpstraKeyEdit::backgroundColourId, lookAndFeel.findColour(juce::ResizableWindow::backgroundColourId));
	lookAndFeel.setColour(TerpstraKeyEdit::outlineColourId, Colour(0xffd7d9da));
	lookAndFeel.setColour(TerpstraKeyEdit::selectedKeyOutlineId, Colour(0xfff7990d));

	lookAndFeel.setColour(VelocityCurveBeam::beamColourId, Colour(0x66ff5e00));
	lookAndFeel.setColour(VelocityCurveBeam::outlineColourId, Colour(0xffd7d9da));

	// Recent files list
	recentFiles.restoreFromString(propertiesFile->getValue("RecentFiles"));
	recentFiles.removeNonExistentFiles();

	// Save/Load location preferences or default fallback values

	String possibleDirectory = propertiesFile->getValue("UserDocumentsDirectory");
	if (File::isAbsolutePath(possibleDirectory))
	{
		userDocumentsDirectory = File(possibleDirectory);
	}
	if (!userDocumentsDirectory.exists() || userDocumentsDirectory.existsAsFile())
	{
		userDocumentsDirectory = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("Lumatone Editor");
		userDocumentsDirectory.createDirectory();
	}

	possibleDirectory = propertiesFile->getValue("UserMappingsDirectory");
	if (File::isAbsolutePath(possibleDirectory))
	{
		userMappingsDirectory = File(possibleDirectory);
	}
	if (!userMappingsDirectory.exists() || userMappingsDirectory.existsAsFile())
	{
		userMappingsDirectory = userDocumentsDirectory.getChildFile("Mappings");
		userMappingsDirectory.createDirectory();
	}

	possibleDirectory = propertiesFile->getValue("UserPalettesDirectory");
	if (File::isAbsolutePath(possibleDirectory))
	{
		userPalettesDirectory = File(possibleDirectory);
	}
	if (!userPalettesDirectory.exists() || userPalettesDirectory.existsAsFile())
	{
		userPalettesDirectory = userDocumentsDirectory.getChildFile("Palettes");
		userPalettesDirectory.createDirectory();
	}

	reloadColourPalettes();

	// State of main window will be read from properties file when main window is created
}

//==============================================================================
void TerpstraSysExApplication::initialise(const String& commandLine)
{
	// This method is where you should put your application's initialisation code..

	// commandLine parameters
	if (!commandLine.isEmpty())
	{
		auto commandLineParameters = getCommandLineParameterArray();

		for (auto commandLineParameter : commandLineParameters)
		{
			// ToDo switch on/off isomorphic mass assign mode

			// Try to open a config file
			if (File::isAbsolutePath(commandLineParameter))
			{
				currentFile = File(commandLineParameter);
			}
			else
			{
				// If file name is with quotes, try removing the quotes
				if (commandLine.startsWithChar('"') && commandLine.endsWithChar('"'))
					currentFile = File(commandLine.substring(1, commandLine.length() - 1));
			}

			if (currentFile.existsAsFile())
				break;	// There can only be one file, and the file name is supposed to be the last parameter
		}
	}

	commandManager.reset(new ApplicationCommandManager());
	commandManager->registerAllCommandsForTarget(this);
    menuModel.reset(new Lumatone::Menu::MainMenuModel(commandManager.get()));
    
	mainWindow.reset(new MainWindow());
	mainWindow->addKeyListener(commandManager->getKeyMappings());
	mainWindow->restoreStateFromPropertiesFile(propertiesFile);

#if JUCE_MAC
	MenuBarModel::setMacMainMenu(menuModel.get());
#else
	mainWindow->setMenuBar(menuModel.get());
	mainWindow->getMenuBarComponent()->getProperties().set(LumatoneEditorStyleIDs::popupMenuBackgroundColour, 
		lookAndFeel.findColour(LumatoneEditorColourIDs::MenuBarBackground).toString()
	);

#endif

	if (currentFile.existsAsFile())
		openFromCurrentFile();
}

void TerpstraSysExApplication::shutdown()
{
	// Add your application's shutdown code here..

	// Save documents directories (Future: provide option to change them and save after changed by user)
	propertiesFile->setValue("UserDocumentsDirectory", userDocumentsDirectory.getFullPathName());
	propertiesFile->setValue("UserMappingsDirectory", userMappingsDirectory.getFullPathName());
	propertiesFile->setValue("UserPalettesDirectory", userPalettesDirectory.getFullPathName());

	// Save recent files list
	recentFiles.removeNonExistentFiles();
	jassert(propertiesFile != nullptr);
	propertiesFile->setValue("RecentFiles", recentFiles.toString());

	// Save state of main window
	mainWindow->saveStateToPropertiesFile(propertiesFile);

	propertiesFile->saveIfNeeded();
	delete propertiesFile;
	propertiesFile = nullptr;

	LocalisedStrings::setCurrentMappings(nullptr);

#if JUCE_MAC
    MenuBarModel::setMacMainMenu(nullptr);
#else
	mainWindow->setMenuBarComponent(nullptr);
#endif
    menuModel = nullptr;

    mainWindow = nullptr; // (deletes our window)

	if (firmwareUpdateWasPerformed)
		FirmwareTransfer::exitLibSsh2();

//	commandManager = nullptr;
}

//==============================================================================
void TerpstraSysExApplication::systemRequestedQuit()
{
	// This is called when the app is being asked to quit: you can ignore this
	// request and let the app carry on running, or call quit() to allow the app to close.

	// If there are changes: ask for save
	if (hasChangesToSave)
	{
		AlertWindow::showYesNoCancelBox(
			AlertWindow::AlertIconType::QuestionIcon, 
			"Quitting the application", 
			"Do you want to save your changes?", 
			"Yes", "No", "Cancel", nullptr, 
			ModalCallbackFunction::create([&](int retc)
			{
				if (retc == 0)
				{
					// "Cancel". Do not quit.
					return;
				}
				else if (retc == 1)
				{
					// "Yes". Try to save. Cancel if unsuccessful
					saveSysExMapping([&](bool success) { if (success) quit(); });
				}
				else
				{
					// retc == 2: "No" -> end without saving
					quit();
				}
			})
		);

		return;
	}

	quit();
}

void TerpstraSysExApplication::anotherInstanceStarted(const String& commandLine)
{
	// When another instance of the app is launched while this one is running,
	// this method is invoked, and the commandLine parameter tells you what
	// the other instance's command-line arguments were.
}

void TerpstraSysExApplication::reloadColourPalettes()
{
	auto foundPaletteFiles = userPalettesDirectory.findChildFiles(File::TypesOfFileToFind::findFiles, true, '*' + String(PALETTEFILEEXTENSION));

	colourPalettes.clear();

	auto paletteSorter = LumatoneEditorPaletteSorter();
	for (auto file : foundPaletteFiles)
	{
		LumatoneEditorColourPalette palette = LumatoneEditorColourPalette::loadFromFile(file);
		colourPalettes.addSorted(paletteSorter, palette);
	}

}

bool TerpstraSysExApplication::saveColourPalette(LumatoneEditorColourPalette& palette, File pathToFile)
{
	bool success = false;

	if (palette.hasBeenModified())
	{
		ValueTree paletteNode = palette.toValueTree();

		if (pathToFile == File())
			pathToFile = File(palette.getPathToFile());

        // If name changed, delete the old one
        if (pathToFile.exists())
        {
            auto currentName = pathToFile.getFileName();
            if (currentName != palette.getName())
            {
                pathToFile.deleteFile();
            }
        }
        
		// New file
		if (!pathToFile.existsAsFile())
		{
            String fileName = "UnnamedPalette";
            
			if (palette.getName().isNotEmpty())
                fileName = palette.getName();
                       
            pathToFile = userPalettesDirectory.getChildFile(fileName);

			// Make sure filename is unique since saving happens automatically
            // Sorry programmers, we're using cardinal numbers here, and the original is implicitly #1 ;)
			int nameId = 1;
			while (pathToFile.withFileExtension(PALETTEFILEEXTENSION).existsAsFile() && nameId < 999999)
			{
                auto fileNameToSave = fileName + "_" + String(++nameId);
                pathToFile = userPalettesDirectory.getChildFile(fileNameToSave);
			}
		}

		success = palette.saveToFile(pathToFile);

		// TODO error handling?
	}

	if (success)
		reloadColourPalettes();

	return success;
}

bool TerpstraSysExApplication::deletePaletteFile(File pathToPalette)
{
	bool success = false;

	if (pathToPalette.existsAsFile())
	{
		success = pathToPalette.deleteFile();
	}

	return success;
}

void TerpstraSysExApplication::getAllCommands(Array <CommandID>& commands)
{
	JUCEApplication::getAllCommands(commands);

	const CommandID ids[] = {
		Lumatone::Menu::commandIDs::openSysExMapping,
		Lumatone::Menu::commandIDs::saveSysExMapping,
		Lumatone::Menu::commandIDs::saveSysExMappingAs,
		Lumatone::Menu::commandIDs::resetSysExMapping,

		Lumatone::Menu::commandIDs::deleteOctaveBoard,
		Lumatone::Menu::commandIDs::copyOctaveBoard,
		Lumatone::Menu::commandIDs::pasteOctaveBoard,
        Lumatone::Menu::commandIDs::pasteOctaveBoardChannels,
        Lumatone::Menu::commandIDs::pasteOctaveBoardNotes,
        Lumatone::Menu::commandIDs::pasteOctaveBoardColours,
        Lumatone::Menu::commandIDs::pasteOctaveBoardTypes,
        
		Lumatone::Menu::commandIDs::undo,
		Lumatone::Menu::commandIDs::redo,

		Lumatone::Debug::commandIDs::toggleDeveloperMode,

		Lumatone::Menu::commandIDs::aboutSysEx
	};

	commands.addArray(ids, numElementsInArray(ids));
}

void TerpstraSysExApplication::getCommandInfo(CommandID commandID, ApplicationCommandInfo& result)
{
	switch (commandID)
	{
	case Lumatone::Menu::commandIDs::openSysExMapping:
		result.setInfo("Load file mapping", "Open a Lumatone key mapping", "File", 0);
		result.addDefaultKeypress('o', ModifierKeys::commandModifier);
		break;

	case Lumatone::Menu::commandIDs::saveSysExMapping:
		result.setInfo("Save mapping", "Save the current mapping to file", "File", 0);
		result.addDefaultKeypress('s', ModifierKeys::commandModifier);
		break;

	case Lumatone::Menu::commandIDs::saveSysExMappingAs:
		result.setInfo("Save mapping as...", "Save the current mapping to new file", "File", 0);
		result.addDefaultKeypress('a', ModifierKeys::commandModifier);
		break;

	case Lumatone::Menu::commandIDs::resetSysExMapping:
		result.setInfo("New", "Start new mapping. Clear all edit fields, do not save current edits.", "File", 0);
		result.addDefaultKeypress('n', ModifierKeys::commandModifier);
		break;

	case Lumatone::Menu::commandIDs::deleteOctaveBoard:
		result.setInfo("Delete", "Delete section data", "Edit", 0);
		result.addDefaultKeypress(KeyPress::deleteKey, ModifierKeys::noModifiers);
		break;

	case Lumatone::Menu::commandIDs::copyOctaveBoard:
		result.setInfo("Copy section", "Copy current octave board data", "Edit", 0);
		result.addDefaultKeypress('c', ModifierKeys::commandModifier);
		break;

	case Lumatone::Menu::commandIDs::pasteOctaveBoard:
		result.setInfo("Paste section", "Paste copied section data", "Edit", 0);
		result.addDefaultKeypress('v', ModifierKeys::commandModifier);
        result.setActive(canPasteSubBoardData());
		break;

    case Lumatone::Menu::commandIDs::pasteOctaveBoardNotes:
        result.setInfo("Paste notes", "Paste copied section notes", "Edit", 0);
        result.addDefaultKeypress('v', ModifierKeys::commandModifier | ModifierKeys::shiftModifier);
        result.setActive(canPasteSubBoardData());
        break;

    case Lumatone::Menu::commandIDs::pasteOctaveBoardChannels:
        result.setInfo("Paste channels", "Paste copied section channels", "Edit", 0);
        result.addDefaultKeypress('v', ModifierKeys::commandModifier | ModifierKeys::altModifier);
        result.setActive(canPasteSubBoardData());
        break;
            
    case Lumatone::Menu::commandIDs::pasteOctaveBoardColours:
        result.setInfo("Paste colours", "Paste copied section colours", "Edit", 0);
        result.addDefaultKeypress('v', ModifierKeys::altModifier);
        result.setActive(canPasteSubBoardData());
        break;
            
    case Lumatone::Menu::commandIDs::pasteOctaveBoardTypes:
        result.setInfo("Paste types", "Paste copied section key types", "Edit", 0);
        result.addDefaultKeypress('v', ModifierKeys::altModifier | ModifierKeys::shiftModifier);
        result.setActive(canPasteSubBoardData());
        break;
            
	case Lumatone::Menu::commandIDs::undo:
		result.setInfo("Undo", "Undo latest edit", "Edit", 0);
		result.addDefaultKeypress('z', ModifierKeys::commandModifier);
		result.setActive(undoManager.canUndo());
		break;

	case Lumatone::Menu::commandIDs::redo:
		result.setInfo("Redo", "Redo latest edit", "Edit", 0);
		result.addDefaultKeypress('y', ModifierKeys::commandModifier);
		result.addDefaultKeypress('z', ModifierKeys::commandModifier + ModifierKeys::shiftModifier);
		result.setActive(undoManager.canRedo());
		break;

	case Lumatone::Menu::commandIDs::aboutSysEx:
		result.setInfo("About Lumatone Editor", "Shows version and copyright", "Help", 0);
		break;

	case Lumatone::Debug::commandIDs::toggleDeveloperMode:
		result.setInfo("Toggle Developer Mode", "Show/hide controls for tweaking internal parameters", "Edit", 0);
		result.addDefaultKeypress('m',
                   juce::ModifierKeys::ctrlModifier + juce::ModifierKeys::altModifier + juce::ModifierKeys::shiftModifier);
		result.setActive(true);
		break;

	default:
		JUCEApplication::getCommandInfo(commandID, result);
		break;
	}
}

bool TerpstraSysExApplication::perform(const InvocationInfo& info)
{
	switch (info.commandID)
	{
	case Lumatone::Menu::commandIDs::openSysExMapping:
		return openSysExMapping();
	case Lumatone::Menu::commandIDs::saveSysExMapping:
		return saveSysExMapping();
	case Lumatone::Menu::commandIDs::saveSysExMappingAs:
		return saveSysExMappingAs();
	case Lumatone::Menu::commandIDs::resetSysExMapping:
		return resetSysExMapping();

	case Lumatone::Menu::commandIDs::deleteOctaveBoard:
		return deleteSubBoardData();
	case Lumatone::Menu::commandIDs::copyOctaveBoard:
		return copySubBoardData();
	case Lumatone::Menu::commandIDs::pasteOctaveBoard:
		return pasteSubBoardData();
    case Lumatone::Menu::commandIDs::pasteOctaveBoardNotes:
    case Lumatone::Menu::commandIDs::pasteOctaveBoardChannels:
    case Lumatone::Menu::commandIDs::pasteOctaveBoardColours:
    case Lumatone::Menu::commandIDs::pasteOctaveBoardTypes:
        return pasteModifiedSubBoardData(info.commandID);

	case Lumatone::Menu::commandIDs::undo:
		return undo();

	case Lumatone::Menu::commandIDs::redo:
		return redo();

	case Lumatone::Menu::commandIDs::aboutSysEx:
		return aboutTerpstraSysEx();

	case Lumatone::Debug::commandIDs::toggleDeveloperMode:
		return toggleDeveloperMode();
	default:
		return JUCEApplication::perform(info);
	}
}

bool TerpstraSysExApplication::openSysExMapping()
{
	chooser = std::make_unique<FileChooser>("Open a Lumatone key mapping", recentFiles.getFile(0).getParentDirectory(), "*.ltn;*.tsx");
	chooser->launchAsync(FileBrowserComponent::FileChooserFlags::canSelectFiles | FileBrowserComponent::FileChooserFlags::openMode,
		[&](const FileChooser& chooser)
		{
			currentFile = chooser.getResult();
			openFromCurrentFile();
		});

	return true;
}

bool TerpstraSysExApplication::saveSysExMapping(std::function<void(bool success)> saveFileCallback)
{
	if (currentFile.getFileName().isEmpty())
		return saveSysExMappingAs(saveFileCallback);
	else
		return saveCurrentFile(saveFileCallback);

}

bool TerpstraSysExApplication::saveSysExMappingAs(std::function<void(bool)> saveFileCallback)
{
	chooser = std::make_unique<FileChooser>("Lumatone Key Mapping Files", recentFiles.getFile(0).getParentDirectory(), "*.ltn");
	chooser->launchAsync(FileBrowserComponent::FileChooserFlags::saveMode | FileBrowserComponent::FileChooserFlags::warnAboutOverwriting,
		[this, saveFileCallback](const FileChooser& chooser)
		{
			currentFile = chooser.getResult();
			bool saved = saveCurrentFile();
			if (saved)
			{
				// Window title
				updateMainTitle();
			}

			saveFileCallback(saved);
		});

	return true;
}

bool TerpstraSysExApplication::resetSysExMapping()
{
	// Clear file
	currentFile = File();

	// Clear all edit fields
	((MainContentComponent*)(mainWindow->getContentComponent()))->deleteAll();

	setHasChangesToSave(false);

	// Clear undoable actions
	// ToDo (?)
	undoManager.clearUndoHistory();


	// Window title
	updateMainTitle();

	return true;
}

bool TerpstraSysExApplication::deleteSubBoardData()
{
	return performUndoableAction(((MainContentComponent*)(mainWindow->getContentComponent()))->createDeleteCurrentSectionAction());
}

bool TerpstraSysExApplication::copySubBoardData()
{
	return ((MainContentComponent*)(mainWindow->getContentComponent()))->copyCurrentSubBoardData();
}

bool TerpstraSysExApplication::pasteSubBoardData()
{
	return performUndoableAction(((MainContentComponent*)(mainWindow->getContentComponent()))->createPasteCurrentSectionAction());
}

bool TerpstraSysExApplication::pasteModifiedSubBoardData(CommandID commandID)
{
    switch (commandID)
    {
    case Lumatone::Menu::pasteOctaveBoardNotes:
    case Lumatone::Menu::pasteOctaveBoardColours:
    case Lumatone::Menu::pasteOctaveBoardChannels:
    case Lumatone::Menu::pasteOctaveBoardTypes:
        return performUndoableAction(((MainContentComponent*)(mainWindow->getContentComponent()))->createModifiedPasteCurrentSectionAction(commandID));
    default:
        jassertfalse;
        return false;
    }
}

bool TerpstraSysExApplication::canPasteSubBoardData() const
{
    if (mainWindow != nullptr)
        return getMainContentComponent()->canPasteCopiedSubBoard();
    return false;
}

bool TerpstraSysExApplication::performUndoableAction(UndoableAction* editAction)
{
	if (editAction != nullptr)
	{
		undoManager.beginNewTransaction();
		if (undoManager.perform(editAction))	// UndoManager will check for nullptr and also for disposing of the object
		{
			setHasChangesToSave(true);
			((MainContentComponent*)(mainWindow->getContentComponent()))->refreshAllFields();
			return true;
		}
	}

	return false;
}

bool TerpstraSysExApplication::undo()
{
	if (undoManager.undo())
	{
		setHasChangesToSave(true);
		((MainContentComponent*)(mainWindow->getContentComponent()))->refreshAllFields();
		return true;
	}
	else
		return false;
}

bool TerpstraSysExApplication::redo()
{
	if (undoManager.redo())
	{
		setHasChangesToSave(true);
		((MainContentComponent*)(mainWindow->getContentComponent()))->refreshAllFields();
		return true;
	}
	else
		return false;
}

bool TerpstraSysExApplication::toggleDeveloperMode()
{
	bool newMode = !propertiesFile->getBoolValue("DeveloperMode");
	propertiesFile->setValue("DeveloperMode", newMode);
	return ((MainContentComponent*)(mainWindow->getContentComponent()))->setDeveloperMode(newMode);
}

void TerpstraSysExApplication::setEditMode(sysExSendingMode editMode)
{
    lumatoneController->setSysExSendingMode(editMode);
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
	VelocityCurveDlgBase* velocityCurveWindow = new VelocityCurveDlgBase(TerpstraVelocityCurveConfig::VelocityCurveType::fader);
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
	VelocityCurveDlgBase* velocityCurveWindow = new VelocityCurveDlgBase(TerpstraVelocityCurveConfig::VelocityCurveType::afterTouch);
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
		sendCurrentConfigurationToDevice();

		// Mark file as unchanged
		setHasChangesToSave(false);

		// Clear undo history
		undoManager.clearUndoHistory();

		// Add file to recent files list
		recentFiles.addFile(currentFile);

		return true;
	}
	else
	{
		// Show error message
		AlertWindow::showMessageBoxAsync(AlertWindow::AlertIconType::WarningIcon, "Open File Error", "The file " + currentFile.getFullPathName() + " could not be opened.");

		// XXX Update Window title in any case? Make file name empty/make data empty in case of error?
		return false;
	}
}

bool TerpstraSysExApplication::setCurrentFile(File fileToOpen)
{
    currentFile = fileToOpen;
    return openFromCurrentFile();
}

// Saves the current mapping to file, specified in currentFile.
bool TerpstraSysExApplication::saveCurrentFile(std::function<void(bool success)> saveFileCallback)
{
	if (currentFile.existsAsFile())
		currentFile.deleteFile();
	bool retc = currentFile.create();
	// XXX error handling

	TerpstraKeyMapping keyMapping;
	((MainContentComponent*)(mainWindow->getContentComponent()))->getData(keyMapping);

    bool appendSuccess = true;
	StringArray stringArray = keyMapping.toStringArray();
	for (int i = 0; i < stringArray.size(); i++)
		appendSuccess = appendSuccess && currentFile.appendText(stringArray[i] + "\n");
        
	setHasChangesToSave(!appendSuccess);
    saveFileCallback(appendSuccess);

	// ToDo undo history?

	// Add file to recent files list - or put it on top of the list
	recentFiles.addFile(currentFile);

	return retc;
}

void TerpstraSysExApplication::sendCurrentConfigurationToDevice()
{
	auto theConfig = ((MainContentComponent*)(mainWindow->getContentComponent()))->getMappingInEdit();

	// MIDI channel, MIDI note, colour and key type config for all keys
	getLumatoneController()->sendCompleteMapping(theConfig);

	// General options
	getLumatoneController()->setAftertouchEnabled(theConfig.afterTouchActive);
	getLumatoneController()->sendLightOnKeyStrokes(theConfig.lightOnKeyStrokes);
	getLumatoneController()->sendInvertFootController(theConfig.invertExpression);
	getLumatoneController()->sendExpressionPedalSensivity(theConfig.expressionControllerSensivity);
    getLumatoneController()->invertSustainPedal(theConfig.invertSustain);

	// Velocity curve config
	getLumatoneController()->setVelocityIntervalConfig(theConfig.velocityIntervalTableValues);

	((MainContentComponent*)(mainWindow->getContentComponent()))->getCurvesArea()->sendConfigToController();
}

void TerpstraSysExApplication::requestConfigurationFromDevice()
{
	// if editing operations were done that have not been saved, give the possibility to save them
	if (hasChangesToSave)
	{
		AlertWindow::showYesNoCancelBox(
			AlertWindow::AlertIconType::QuestionIcon,
			"Request configuration from device",
			"Lumatone's layout will now be imported. This will overwrite your unsaved changes. Do you want to save them first?",
			"Save to file", "Import anyway", "Cancel import", nullptr,
			ModalCallbackFunction::create([&](int retc)
			{
				if (retc == 0)
				{
					// "Cancel". Do not receive config, go offline
					DBG("Layout import cancelled");
                    setEditMode(sysExSendingMode::offlineEditor);
					return;
				}
				else if (retc == 1)
				{
					// "Yes". Try to save. Cancel if unsuccessful
					saveSysExMapping([this](bool success) 
					{ 
						if (success)
							this->requestConfigurationFromDevice();
						else
							DBG("Cancelled layout import");
					});
				}
				else
				{
					// retc == 2: "No" -> no saving, overwrite
					DBG("Overwriting current edits");
					setHasChangesToSave(false);
					requestConfigurationFromDevice();
				}
			})
		);

		return;
	}

	TerpstraSysExApplication::getApp().resetSysExMapping();

	// Request MIDI channel, MIDI note, colour and key type config for all keys
	getLumatoneController()->sendGetCompleteMappingRequest();

	// General options
	getLumatoneController()->getPresetFlags();
	getLumatoneController()->getExpressionPedalSensitivity();

	// Velocity curve config
	getLumatoneController()->sendVelocityIntervalConfigRequest();
	getLumatoneController()->sendVelocityConfigRequest();
	getLumatoneController()->sendFaderConfigRequest();
	getLumatoneController()->sendAftertouchConfigRequest();

}

void TerpstraSysExApplication::updateMainTitle()
{
	String windowTitle("Lumatone Editor");
	if (!currentFile.getFileName().isEmpty())
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

//https://forum.juce.com/t/closing-dialog-windows-on-shutdown/27326/6
void TerpstraSysExApplication::setOpenDialogWindow(DialogWindow* dialogWindowIn)
{
    dialogWindow.reset(dialogWindowIn);

	// attach callback to window to release std::unique_ptr when the window is closed
	ModalComponentManager::getInstance()->attachCallback(dialogWindow.get(), ModalCallbackFunction::create([&](int r)
	{
        dialogWindow.release();
	}));
}

bool TerpstraSysExApplication::aboutTerpstraSysEx()
{
	String m;

	m << "Lumatone Editor" << newLine
		<< newLine
		<< "Version " << String((JUCE_APP_VERSION_HEX >> 16) & 0xff) << "."
		<< String((JUCE_APP_VERSION_HEX >> 8) & 0xff) << "."
		<< String(JUCE_APP_VERSION_HEX & 0xff) << newLine

		<< "@ Hans Straub, Vincenzo Sicurella 2014 - 2021" << newLine
		<< newLine
		<< "Based on the program 'TerpstraSysEx' @ Dylan Horvath 2007" << newLine
		<< newLine
		<< "For help on using this program, or any questions relating to the Lumatone keyboard, go to:" << newLine
		<< newLine
		<< "http://lumatone.io";

	DialogWindow::LaunchOptions options;
	Label* label = new Label();
	label->setLookAndFeel(&lookAndFeel);
	label->setText(m, dontSendNotification);
	label->setFont(lookAndFeel.getAppFont(LumatoneEditorFont::FranklinGothic));
	options.content.setOwned(label);

	juce::Rectangle<int> area(0, 0, 400, 200);
	options.content->setSize(area.getWidth(), area.getHeight());

	resizeLabelWithHeight(label, roundToInt(area.getHeight() * 0.24f));

	options.dialogTitle = "About Lumatone Editor";
	options.dialogBackgroundColour = lookAndFeel.findColour(LumatoneEditorColourIDs::DarkBackground);

	options.escapeKeyTriggersCloseButton = true;
	options.useNativeTitleBar = false;
	options.resizable = false;


	auto dw = options.launchAsync();
	dw->setLookAndFeel(&lookAndFeel);
	dw->centreWithSize(400, 260);

	setOpenDialogWindow(dw);

	return true;
}

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(TerpstraSysExApplication)
