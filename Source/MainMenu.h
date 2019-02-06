/*
  ==============================================================================

    MainMenu.h
    Created: 13 Jun 2014 7:41:00pm
    Author:  hsstraub

  ==============================================================================abaouztSysEx
*/

#ifndef MAINMENU_H_INCLUDED
#define MAINMENU_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

// Main menu
class TerpstraSysExMainMenuModel : public MenuBarModel
{
	public:
		enum commandIDs {
			openSysExMapping = 0x200010,
			saveSysExMapping = 0x200011,
			saveSysExMappingAs = 0x200012,
			resetSysExMapping = 0x200013,
			optionsWindow = 0x200014,

			recentFilesBaseID = 0x200100,

			aboutSysEx = 0x200040
		};

public:
	TerpstraSysExMainMenuModel(ApplicationCommandManager* commandManager);

	virtual StringArray getMenuBarNames();
	PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName);
	void menuItemSelected(int menuItemID, int topLevelMenuIndex);

protected:
	virtual void createFileMenu(PopupMenu& menu);
	virtual void createHelpMenu(PopupMenu& menu);

private:
	ApplicationCommandManager*	theManager;
};



#endif  // MAINMENU_H_INCLUDED
