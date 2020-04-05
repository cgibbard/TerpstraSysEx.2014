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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "MOSDebugDialog.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AdvancedMOSDialog::AdvancedMOSDialog ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    rootNoteLabel.reset (new Label ("rootNoteLabel",
                                    TRANS("Root MIDI Note")));
    addAndMakeVisible (rootNoteLabel.get());
    rootNoteLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    rootNoteLabel->setJustificationType (Justification::centredRight);
    rootNoteLabel->setEditable (false, false, false);
    rootNoteLabel->setColour (TextEditor::textColourId, Colours::black);
    rootNoteLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    rootNoteLabel->setBounds (-4, 103, 135, 24);

    rootMidiChnLabel.reset (new Label ("rootMidiChnLabel",
                                       TRANS("Root MIDI Channel")));
    addAndMakeVisible (rootMidiChnLabel.get());
    rootMidiChnLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    rootMidiChnLabel->setJustificationType (Justification::centredRight);
    rootMidiChnLabel->setEditable (false, false, false);
    rootMidiChnLabel->setColour (TextEditor::textColourId, Colours::black);
    rootMidiChnLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    rootMidiChnLabel->setBounds (-4, 135, 135, 24);

    periodPerOctLabel.reset (new Label ("periodPerOctLabel",
                                        TRANS("Periods / Octave")));
    addAndMakeVisible (periodPerOctLabel.get());
    periodPerOctLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    periodPerOctLabel->setJustificationType (Justification::centredLeft);
    periodPerOctLabel->setEditable (false, false, false);
    periodPerOctLabel->setColour (TextEditor::textColourId, Colours::black);
    periodPerOctLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    periodPerOctLabel->setBounds (236, 7, 119, 24);

    rootNoteSlider.reset (new Slider ("rootNoteSlider"));
    addAndMakeVisible (rootNoteSlider.get());
    rootNoteSlider->setTooltip (TRANS("The MIDI note that your scale starts on."));
    rootNoteSlider->setRange (0, 127, 1);
    rootNoteSlider->setSliderStyle (Slider::IncDecButtons);
    rootNoteSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    rootNoteSlider->addListener (this);

    rootNoteSlider->setBounds (140, 103, 150, 24);

    rootChannelSlider.reset (new Slider ("rootChannelSlider"));
    addAndMakeVisible (rootChannelSlider.get());
    rootChannelSlider->setTooltip (TRANS("The MIDI channel that contains your root note."));
    rootChannelSlider->setRange (1, 16, 1);
    rootChannelSlider->setSliderStyle (Slider::IncDecButtons);
    rootChannelSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    rootChannelSlider->addListener (this);

    rootChannelSlider->setBounds (140, 135, 150, 24);

    periodPerOctaveBox.reset (new ComboBox ("periodPerOctaveBox"));
    addAndMakeVisible (periodPerOctaveBox.get());
    periodPerOctaveBox->setTooltip (TRANS("If your period has factors, you can get more scale patterns by using a factor as the period."));
    periodPerOctaveBox->setEditableText (false);
    periodPerOctaveBox->setJustificationType (Justification::centredLeft);
    periodPerOctaveBox->setTextWhenNothingSelected (String());
    periodPerOctaveBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    periodPerOctaveBox->addItem (TRANS("1"), 1);
    periodPerOctaveBox->addListener (this);

    periodPerOctaveBox->setBounds (356, 7, 64, 24);

    periodLabel.reset (new Label ("periodLabel",
                                  TRANS("Period")));
    addAndMakeVisible (periodLabel.get());
    periodLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    periodLabel->setJustificationType (Justification::centredRight);
    periodLabel->setEditable (false, false, false);
    periodLabel->setColour (TextEditor::textColourId, Colours::black);
    periodLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    periodLabel->setBounds (36, 9, 95, 24);

    generatorLabel.reset (new Label ("generatorLabel",
                                     TRANS("Generator")));
    addAndMakeVisible (generatorLabel.get());
    generatorLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    generatorLabel->setJustificationType (Justification::centredRight);
    generatorLabel->setEditable (false, false, false);
    generatorLabel->setColour (TextEditor::textColourId, Colours::black);
    generatorLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    generatorLabel->setBounds (36, 41, 95, 24);

    sizeLabel.reset (new Label ("sizeLabel",
                                TRANS("Scale Size")));
    addAndMakeVisible (sizeLabel.get());
    sizeLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    sizeLabel->setJustificationType (Justification::centredRight);
    sizeLabel->setEditable (false, false, false);
    sizeLabel->setColour (TextEditor::textColourId, Colours::black);
    sizeLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    sizeLabel->setBounds (36, 73, 95, 24);

    generatorOffsetLabel.reset (new Label ("generatorOffsetLabel",
                                           TRANS("Offset")));
    addAndMakeVisible (generatorOffsetLabel.get());
    generatorOffsetLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    generatorOffsetLabel->setJustificationType (Justification::centredLeft);
    generatorOffsetLabel->setEditable (false, false, false);
    generatorOffsetLabel->setColour (TextEditor::textColourId, Colours::black);
    generatorOffsetLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    generatorOffsetLabel->setBounds (236, 41, 63, 24);

    flipStepsButton.reset (new ToggleButton ("flipStepsButton"));
    addAndMakeVisible (flipStepsButton.get());
    flipStepsButton->setButtonText (TRANS("Flip Steps"));
    flipStepsButton->addListener (this);

    flipStepsButton->setBounds (320, 88, 103, 24);

    negateXButton.reset (new ToggleButton ("negateXButton"));
    addAndMakeVisible (negateXButton.get());
    negateXButton->setButtonText (TRANS("Negate Horizontal"));
    negateXButton->addListener (this);

    negateXButton->setBounds (320, 112, 191, 24);

    negateVerticalButton.reset (new ToggleButton ("negateVerticalButton"));
    addAndMakeVisible (negateVerticalButton.get());
    negateVerticalButton->setButtonText (TRANS("Negate Right Upwards"));
    negateVerticalButton->addListener (this);

    negateVerticalButton->setBounds (320, 136, 175, 24);

    periodSlider.reset (new Slider ("periodSlider"));
    addAndMakeVisible (periodSlider.get());
    periodSlider->setTooltip (TRANS("This should be how many unique notes you have in the tuning."));
    periodSlider->setRange (5, 400, 1);
    periodSlider->setSliderStyle (Slider::IncDecButtons);
    periodSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 45, 20);
    periodSlider->addListener (this);

    periodSlider->setBounds (140, 9, 88, 24);

    generatorBox.reset (new ComboBox ("generatorBox"));
    addAndMakeVisible (generatorBox.get());
    generatorBox->setTooltip (TRANS("The degree which is the basis for pattern generation. If using a custom, non-equal tuning, the chosen degree may not align with the tuning."));
    generatorBox->setEditableText (false);
    generatorBox->setJustificationType (Justification::centredLeft);
    generatorBox->setTextWhenNothingSelected (String());
    generatorBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    generatorBox->addListener (this);

    generatorBox->setBounds (140, 41, 88, 24);

    sizeBox.reset (new ComboBox ("sizeBox"));
    addAndMakeVisible (sizeBox.get());
    sizeBox->setTooltip (TRANS("The number of notes in the \"natural\" scale."));
    sizeBox->setEditableText (false);
    sizeBox->setJustificationType (Justification::centredLeft);
    sizeBox->setTextWhenNothingSelected (String());
    sizeBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    sizeBox->addListener (this);

    sizeBox->setBounds (140, 73, 88, 24);

    generatorOffsetSlider.reset (new Slider ("generatorOffsetSlider"));
    addAndMakeVisible (generatorOffsetSlider.get());
    generatorOffsetSlider->setTooltip (TRANS("Rotates the pattern generation, which produces different modes in regards to the root note."));
    generatorOffsetSlider->setRange (-12, 12, 1);
    generatorOffsetSlider->setSliderStyle (Slider::IncDecButtons);
    generatorOffsetSlider->setTextBoxStyle (Slider::TextBoxLeft, false, 65, 20);
    generatorOffsetSlider->addListener (this);

    generatorOffsetSlider->setBounds (292, 41, 128, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

AdvancedMOSDialog::~AdvancedMOSDialog()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    rootNoteLabel = nullptr;
    rootMidiChnLabel = nullptr;
    periodPerOctLabel = nullptr;
    rootNoteSlider = nullptr;
    rootChannelSlider = nullptr;
    periodPerOctaveBox = nullptr;
    periodLabel = nullptr;
    generatorLabel = nullptr;
    sizeLabel = nullptr;
    generatorOffsetLabel = nullptr;
    flipStepsButton = nullptr;
    negateXButton = nullptr;
    negateVerticalButton = nullptr;
    periodSlider = nullptr;
    generatorBox = nullptr;
    sizeBox = nullptr;
    generatorOffsetSlider = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AdvancedMOSDialog::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffbad0de));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AdvancedMOSDialog::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AdvancedMOSDialog::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == rootNoteSlider.get())
    {
        //[UserSliderCode_rootNoteSlider] -- add your slider handling code here..
        //[/UserSliderCode_rootNoteSlider]
    }
    else if (sliderThatWasMoved == rootChannelSlider.get())
    {
        //[UserSliderCode_rootChannelSlider] -- add your slider handling code here..
        //[/UserSliderCode_rootChannelSlider]
    }
    else if (sliderThatWasMoved == periodSlider.get())
    {
        //[UserSliderCode_periodSlider] -- add your slider handling code here..
		periodChanged(sliderThatWasMoved->getValue());
        //[/UserSliderCode_periodSlider]
    }
    else if (sliderThatWasMoved == generatorOffsetSlider.get())
    {
        //[UserSliderCode_generatorOffsetSlider] -- add your slider handling code here..
		
        //[/UserSliderCode_generatorOffsetSlider]
    }

    //[UsersliderValueChanged_Post]
	sendScaleData();
    //[/UsersliderValueChanged_Post]
}

void AdvancedMOSDialog::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == periodPerOctaveBox.get())
    {
        //[UserComboBoxCode_periodPerOctaveBox] -- add your combo box handling code here..
        //[/UserComboBoxCode_periodPerOctaveBox]
    }
    else if (comboBoxThatHasChanged == generatorBox.get())
    {
        //[UserComboBoxCode_generatorBox] -- add your combo box handling code here..
		generatorChanged(comboBoxThatHasChanged->getSelectedId());
        //[/UserComboBoxCode_generatorBox]
    }
    else if (comboBoxThatHasChanged == sizeBox.get())
    {
        //[UserComboBoxCode_sizeBox] -- add your combo box handling code here..
		scaleSizeChanged(comboBoxThatHasChanged->getSelectedId());
        //[/UserComboBoxCode_sizeBox]
    }

    //[UsercomboBoxChanged_Post]
	sendScaleData();
    //[/UsercomboBoxChanged_Post]
}

void AdvancedMOSDialog::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == flipStepsButton.get())
    {
        //[UserButtonCode_flipStepsButton] -- add your button handler code here..
        //[/UserButtonCode_flipStepsButton]
    }
    else if (buttonThatWasClicked == negateXButton.get())
    {
        //[UserButtonCode_negateXButton] -- add your button handler code here..
        //[/UserButtonCode_negateXButton]
    }
    else if (buttonThatWasClicked == negateVerticalButton.get())
    {
        //[UserButtonCode_negateVerticalButton] -- add your button handler code here..
        //[/UserButtonCode_negateVerticalButton]
    }

    //[UserbuttonClicked_Post]
	sendScaleData();
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void AdvancedMOSDialog::setScaleStructure(ScaleStructure* structureIn)
{
	scaleStructure = structureIn;

	if (scaleStructure != nullptr)
	{
		scaleStructure->setGeneratorIndex(scaleStructure->getSuggestedGeneratorIndex());
		scaleStructure->setSizeIndex(scaleStructure->getSuggestedSizeIndex());
		scaleStructure->applySuggestedSizeGrouping();

		updateAllControls();
	}
}

void AdvancedMOSDialog::updateAllControls()
{
	periodSlider->setValue(scaleStructure->getPeriod(), dontSendNotification);
	
	generatorBox->clear();
	int i = 0;
	for (auto generator : scaleStructure->getCoprimeGenerators())
	{
		generatorBox->addItem(String(generator), ++i);
	}
	generatorBox->setSelectedId(scaleStructure->getGeneratorIndex() + 1, dontSendNotification);

	sizeBox->clear();
	i = 0;
	for (auto size : scaleStructure->getScaleSizes())
	{
		sizeBox->addItem(String(size), ++i);
	}
	sizeBox->setSelectedId(scaleStructure->getCurrentSizeIndex() + 1, dontSendNotification);

	generatorOffsetSlider->setRange(-scaleStructure->getCurrentScaleSize(), 0, 1);
	generatorOffsetSlider->setValue(scaleStructure->getGeneratorOffset(), dontSendNotification);

	// TODO: Fraction periods / periods per octave
}

void AdvancedMOSDialog::periodChanged(int newPeriod)
{
	scaleStructure->resetPeriod(newPeriod);

	generatorBox->clear();
	int i = 0;
	for (auto generator : scaleStructure->getCoprimeGenerators())
	{
		generatorBox->addItem(String(generator), ++i);
	}

	// Auto-select suggested generator
	generatorBox->setSelectedId(scaleStructure->getSuggestedGeneratorIndex() + 1);
}

void AdvancedMOSDialog::generatorChanged(int newGeneratorIndex)
{
	scaleStructure->setGeneratorIndex(newGeneratorIndex - 1);

	sizeBox->clear();
	int i = 0;
	for (auto size : scaleStructure->getScaleSizes())
	{
		sizeBox->addItem(String(size), ++i);
	}

	// Auto-select suggested scale size
	sizeBox->setSelectedId(scaleStructure->getSuggestedSizeIndex() + 1);
}

void AdvancedMOSDialog::scaleSizeChanged(int newSizeIndex)
{
	generatorOffsetSlider->setRange(-scaleStructure->getScaleSize(newSizeIndex), 0, 1);
}

void AdvancedMOSDialog::sendScaleData()
{

}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AdvancedMOSDialog" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ffbad0de"/>
  <LABEL name="rootNoteLabel" id="8377e5f1f9491470" memberName="rootNoteLabel"
         virtualName="" explicitFocusOrder="0" pos="-4 103 135 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Root MIDI Note" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="rootMidiChnLabel" id="35bddb8c094b5f43" memberName="rootMidiChnLabel"
         virtualName="" explicitFocusOrder="0" pos="-4 135 135 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Root MIDI Channel" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="periodPerOctLabel" id="d28e924208f1a91d" memberName="periodPerOctLabel"
         virtualName="" explicitFocusOrder="0" pos="236 7 119 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Periods / Octave" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <SLIDER name="rootNoteSlider" id="22af7e73878d1afe" memberName="rootNoteSlider"
          virtualName="" explicitFocusOrder="0" pos="140 103 150 24" tooltip="The MIDI note that your scale starts on."
          min="0.0" max="127.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="rootChannelSlider" id="3ff9bfddacae32d4" memberName="rootChannelSlider"
          virtualName="" explicitFocusOrder="0" pos="140 135 150 24" tooltip="The MIDI channel that contains your root note."
          min="1.0" max="16.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <COMBOBOX name="periodPerOctaveBox" id="b1321dacf0c0d913" memberName="periodPerOctaveBox"
            virtualName="" explicitFocusOrder="0" pos="356 7 64 24" tooltip="If your period has factors, you can get more scale patterns by using a factor as the period."
            editable="0" layout="33" items="1" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="periodLabel" id="36aaf36840c58c00" memberName="periodLabel"
         virtualName="" explicitFocusOrder="0" pos="36 9 95 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Period" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="generatorLabel" id="e47cda3a5305c947" memberName="generatorLabel"
         virtualName="" explicitFocusOrder="0" pos="36 41 95 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Generator" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="sizeLabel" id="8b965976a4bcd82" memberName="sizeLabel"
         virtualName="" explicitFocusOrder="0" pos="36 73 95 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Scale Size" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="34"/>
  <LABEL name="generatorOffsetLabel" id="79f13fe40da1d10c" memberName="generatorOffsetLabel"
         virtualName="" explicitFocusOrder="0" pos="236 41 63 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Offset" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <TOGGLEBUTTON name="flipStepsButton" id="118a99a68f95a3c6" memberName="flipStepsButton"
                virtualName="" explicitFocusOrder="0" pos="320 88 103 24" buttonText="Flip Steps"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="negateXButton" id="60c3477c9ae517e6" memberName="negateXButton"
                virtualName="" explicitFocusOrder="0" pos="320 112 191 24" buttonText="Negate Horizontal"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <TOGGLEBUTTON name="negateVerticalButton" id="c8c9da0fc9df050a" memberName="negateVerticalButton"
                virtualName="" explicitFocusOrder="0" pos="320 136 175 24" buttonText="Negate Right Upwards"
                connectedEdges="0" needsCallback="1" radioGroupId="0" state="0"/>
  <SLIDER name="periodSlider" id="da86a5ed922db1ef" memberName="periodSlider"
          virtualName="" explicitFocusOrder="0" pos="140 9 88 24" tooltip="This should be how many unique notes you have in the tuning."
          min="5.0" max="400.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="45" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <COMBOBOX name="generatorBox" id="f975c68570a4b289" memberName="generatorBox"
            virtualName="" explicitFocusOrder="0" pos="140 41 88 24" tooltip="The degree which is the basis for pattern generation. If using a custom, non-equal tuning, the chosen degree may not align with the tuning."
            editable="0" layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="sizeBox" id="f2b8d40150c48ee3" memberName="sizeBox" virtualName=""
            explicitFocusOrder="0" pos="140 73 88 24" tooltip="The number of notes in the &quot;natural&quot; scale."
            editable="0" layout="33" items="" textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <SLIDER name="generatorOffsetSlider" id="9f692e9c4acd1070" memberName="generatorOffsetSlider"
          virtualName="" explicitFocusOrder="0" pos="292 41 128 24" tooltip="Rotates the pattern generation, which produces different modes in regards to the root note."
          min="-12.0" max="12.0" int="1.0" style="IncDecButtons" textBoxPos="TextBoxLeft"
          textBoxEditable="1" textBoxWidth="65" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

