/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 4.3.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
#include "Main.h"
//[/Headers]

#include "VelocityCurveDlg.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
VelocityCurveDlg::VelocityCurveDlg ()
{
    //[Constructor_pre] You can add your own custom stuff here..
	keyType = TerpstraKey::noteOnNoteOff;
    //[/Constructor_pre]

    addAndMakeVisible (lblDescription = new Label ("lblDescription",
                                                   TRANS("Click with the mouse in the graphics to draw the velocity curve.")));
    lblDescription->setFont (Font (15.00f, Font::plain));
    lblDescription->setJustificationType (Justification::centredLeft);
    lblDescription->setEditable (false, false, false);
    lblDescription->setColour (TextEditor::textColourId, Colours::black);
    lblDescription->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]

	for (int x = 0; x < 128; x++)
	{
		velocityBeamTable[x] = new  VelocityCurveBeam();
		addAndMakeVisible(velocityBeamTable[x]);
		velocityBeamTable[x]->addMouseListener(this, true);
	}

    //[/UserPreSize]

    setSize (640, 320);


    //[Constructor] You can add your own custom stuff here..

	// Set values according to the properties files
	restoreStateFromPropertiesFile(TerpstraSysExApplication::getApp().getPropertiesFile());

    //[/Constructor]
}

VelocityCurveDlg::~VelocityCurveDlg()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
	// Save values to properties file
	saveStateToPropertiesFile(TerpstraSysExApplication::getApp().getPropertiesFile());
    //[/Destructor_pre]

    lblDescription = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
	deleteAllChildren();
    //[/Destructor]
}

//==============================================================================
void VelocityCurveDlg::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffbad0de));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void VelocityCurveDlg::resized()
{
    //[UserPreResize] Add your own custom resize code here..
	int w = this->getWidth();
	int h = this->getHeight();
    //[/UserPreResize]

    lblDescription->setBounds (8, 8, 416, 32);
    //[UserResized] Add your own custom resize handling here..

	Path internalPath1;
	internalPath1.startNewSubPath(graphicsXPadding, graphicsYPadding);
	internalPath1.lineTo(graphicsXPadding, h - graphicsYPadding);
	internalPath1.lineTo(w - graphicsXPadding, h - graphicsYPadding);
	internalPath1.lineTo(w - graphicsXPadding, graphicsYPadding);
	internalPath1.closeSubPath();

	float velocityBeamWidth = (w - 2 * graphicsXPadding) / 128;
	float velocityGraphicsHeight = h - 2 * graphicsYPadding;

	for (int x = 0; x < 128; x++)
	{
		velocityBeamTable[x]->setBounds(
			graphicsXPadding + x*velocityBeamWidth,
			graphicsYPadding,
			velocityBeamWidth,
			velocityGraphicsHeight);
	}
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void VelocityCurveDlg::restoreStateFromPropertiesFile(PropertiesFile* propertiesFile)
{
	String keyName = keyType == TerpstraKey::continuousController ? "FaderVelocityCurveTable" : "NoteOnOffVelocityCurveTable";

	StringArray velocityCurveValueArray = StringArray::fromTokens(propertiesFile->getValue(keyName), false);
	if (velocityCurveValueArray.size() > 0)
	{
		jassert(velocityCurveValueArray.size() >= 128);

		for (int x = 0; x < 128; x++)
			setBeamValue(x, velocityCurveValueArray[x].getIntValue(), false);
	}
	else
	{
		// Initialize velocity lookup table
		for (int x = 0; x < 128; x++)
			setBeamValue(x, x, false );
	}

	setSize(
		propertiesFile->getIntValue("VelocityCurveWindowWidth", 640),
		propertiesFile->getIntValue("VelocityCurveWindowHeight", 320));
}

void VelocityCurveDlg::saveStateToPropertiesFile(PropertiesFile* propertiesFile)
{
	String velocityCurveString;

	for (int x = 0; x < 128; x++)
		velocityCurveString += String(velocityBeamTable[x]->getValue()) + " ";

	String keyName = keyType == TerpstraKey::continuousController ? "FaderVelocityCurveTable" : "NoteOnOffVelocityCurveTable";

	propertiesFile->setValue(keyName, velocityCurveString);

	propertiesFile->setValue("VelocityCurveWindowWidth", getWidth());
	propertiesFile->setValue("VelocityCurveWindowHeight", getHeight());
}

void VelocityCurveDlg::setBeamValue(int pos, int newValue, bool sendToController)
{
	jassert(pos >= 0 && pos < 128 && newValue >= 0 && newValue < 128);

	if (newValue != velocityBeamTable[pos]->getValue())
	{
		velocityBeamTable[pos]->setValue(newValue);

		if (sendToController)
			TerpstraSysExApplication::getApp().getMidiDriver().sendVelocityConfig(keyType, pos, newValue);
	}
}

void VelocityCurveDlg::setBeamValueAtLeast(int pos, int newValue, bool sendToController)
{
	jassert(pos >= 0 && pos < 128 && newValue >= 0 && newValue < 128);

	if (velocityBeamTable[pos]->getValue() < newValue)
		setBeamValue(pos, newValue, sendToController);
}

void VelocityCurveDlg::setBeamValueAtMost(int pos, int newValue, bool sendToController)
{
	jassert(pos >= 0 && pos < 128 && newValue >= 0 && newValue < 128);

	if (velocityBeamTable[pos]->getValue() > newValue)
		setBeamValue(pos, newValue, sendToController);
}

void VelocityCurveDlg::mouseDown(const MouseEvent &event)
{
	int h = this->getHeight();
	float velocityGraphicsHeight = h - 2 * graphicsYPadding;

	for (int x = 0; x < 128; x++)
	{
		if (event.eventComponent == velocityBeamTable[x] || event.eventComponent->getParentComponent() == velocityBeamTable[x])
		{
			int newBeamValue = (velocityGraphicsHeight - event.getMouseDownY()) * 128 / velocityGraphicsHeight;
			setBeamValue(x, newBeamValue, true);

			// Change other beams' values so curve stays monotonous
			for(int x2 = 0; x2 < x; x2++)
				setBeamValueAtMost(x2, newBeamValue, true);

			for (int x2 = x+1; x2 < 128; x2++)
				setBeamValueAtLeast(x2, newBeamValue, true);

			break;
		}
	}
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="VelocityCurveDlg" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="640" initialHeight="320">
  <BACKGROUND backgroundColour="ffbad0de"/>
  <LABEL name="lblDescription" id="e1affcc7a142cab2" memberName="lblDescription"
         virtualName="" explicitFocusOrder="0" pos="8 8 416 32" edTextCol="ff000000"
         edBkgCol="0" labelText="Click with the mouse in the graphics to draw the velocity curve."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="15" bold="0" italic="0" justification="33"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
