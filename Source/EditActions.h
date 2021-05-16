/*
  ==============================================================================

    EditActions.h
    Created: 9 May 2021 9:49:13pm
    Author:  hsstraub

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "KeyboardDataStructure.h"

namespace Lumatone {

	class SingleNoteAssignAction : public UndoableAction
	{
	public:
		SingleNoteAssignAction(
			int setSelection,
			int keySelection,
			bool setKeyType,
			bool setChannel, 
			bool setNote,
			bool setColour,
			TerpstraKey::KEYTYPE newKeyType = TerpstraKey::KEYTYPE::noteOnNoteOff, 
			int newChannelNumber = 0, 
			int newNoteNumber = 0,
			TerpstraKey::COLOURTYPE newColour = juce::Colour());

		SingleNoteAssignAction(const SingleNoteAssignAction& second)
			: setSelection(second.setSelection)
			, keySelection(second.keySelection)
			, setKeyType(second.setKeyType)
			, setChannel(second.setChannel)
			, setNote(second.setNote)
			, setColour(second.setColour)
			, previousData(second.previousData)
			, newData(second.newData)
		{}

		bool isValid() const;

		virtual bool perform() override;
		virtual bool undo() override;
		int getSizeInUnits() override { return sizeof(SingleNoteAssignAction); }

	private:
		int setSelection = - 1;
		int keySelection = -1;

		bool setKeyType = false;
		bool setChannel = false;
		bool setNote = false;
		bool setColour = false;

		TerpstraKey previousData;
		TerpstraKey newData;
	};

	class SectionEditAction : public UndoableAction
	{
	public:
		SectionEditAction(int setSelection, TerpstraKeys& newSectionValue);

		SectionEditAction(const SectionEditAction& second)
			: setSelection(second.setSelection)
			, previousData(second.previousData)
			, newData(second.newData)
		{}

		bool isValid() const;

		virtual bool perform() override;
		virtual bool undo() override;
		int getSizeInUnits() override { return sizeof(SectionEditAction); }

	private:
		int setSelection = -1;

		TerpstraKeys previousData;
		TerpstraKeys newData;
	};

}