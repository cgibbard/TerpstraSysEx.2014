/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MOSMapping  : public Component,
                    public Button::Listener,
                    public Slider::Listener,
                    public ComboBox::Listener
{
public:
    //==============================================================================
    MOSMapping ();
    ~MOSMapping() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Label> periodLabel;
    std::unique_ptr<Label> generatorLabel;
    std::unique_ptr<Label> sizeLabel;
    std::unique_ptr<Label> rootNoteLabel;
    std::unique_ptr<Label> rootMidiChnLabel;
    std::unique_ptr<Label> periodPerOctLabel;
    std::unique_ptr<Label> generatorOffsetLabel;
    std::unique_ptr<ToggleButton> flipStepsButton;
    std::unique_ptr<ToggleButton> negateXButton;
    std::unique_ptr<ToggleButton> negateVerticalButton;
    std::unique_ptr<Slider> periodSlider;
    std::unique_ptr<ComboBox> generatorBox;
    std::unique_ptr<ComboBox> sizeBox;
    std::unique_ptr<Slider> rootNoteSlider;
    std::unique_ptr<Slider> rootChannelSlider;
    std::unique_ptr<ComboBox> periodPerOctaveBox;
    std::unique_ptr<Slider> generatorOffsetSlider;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MOSMapping)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

