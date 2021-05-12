/*
  ==============================================================================

    DeveloperDlg.h
    Created: 6 May 2021 7:17:34pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "CommandLineInterface.h"

class DeveloperDlg : public Component, private Button::Listener
{
private:
    typedef enum {
        Velocity = 1,
        Fader,
        Aftertouch,
        Lumatouch,
        BoardThresholds,
        WheelThresholds,
        ExprPedalBounds
    } ResetMenuId;

public:

    DeveloperDlg();
    ~DeveloperDlg();

    void paint(Graphics& g) override;

    void resized() override;

    void lookAndFeelChanged() override;

    void buttonClicked(Button* btn) override;

    void sendResetRequest(ResetMenuId resetPropertyId);

private:

    void launchCommandLineDialog();

private:

    std::unique_ptr<Button> btnMidiLog;
    std::unique_ptr<Button> btnShowCommandLine;
    std::unique_ptr<Button> btnPropertyViewer;
    std::unique_ptr<ToggleButton> toggleDemoMode;
    std::unique_ptr<ComboBox> cbTableSelector;
    std::unique_ptr<Button> btnTableReset;

    FlexBox flexbox;
};