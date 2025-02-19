/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.8

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include "../JuceLibraryCode/JuceHeader.h"

#include "LumatoneEditorStyleCommon.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class GeneralOptionsDlg  : public Component,
                           public LumatoneController::FirmwareListener,
                           public juce::Button::Listener
{
public:
    //==============================================================================
    GeneralOptionsDlg ();
    ~GeneralOptionsDlg() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

	// New mapping is loaded. Display data.
	void loadFromMapping();

    void lookAndFeelChanged() override;

    // LumatoneController::FirmwareListener Implementation
    void presetFlagsReceived(PresetFlags presetFlags) override;

    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;
    void buttonClicked (juce::Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    int roundedCornerSize;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::Label> labelGeneralSettingslTitle;
    std::unique_ptr<juce::ToggleButton> buttonAfterTouchActive;
    std::unique_ptr<juce::ToggleButton> buttonLightOnKeyStrokes;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeneralOptionsDlg)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

