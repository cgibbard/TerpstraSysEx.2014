/*
  ==============================================================================

    CommandLineInterface.h
    Created: 6 May 2021 7:16:03pm
    Author:  Vincenzo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../LumatoneController.h"

class CommandParser : private TerpstraMidiDriver::Listener, private LumatoneController::FirmwareListener
{

public:

    struct CommandDefinition
    {
        bool commandIsWrapped = false;

        int cmdNumber = -1;
        uint8 targetBoard = -1;
        size_t payloadLength = 0;

        Array<uint8> argByte; // Raw commands
        Array<var> argVar; // For wrapped commands

        FirmwareSupport::Error errorStatus = FirmwareSupport::Error::messageIsNotSysEx;

        String toString()
        {
            String str = "CMD: " + String(cmdNumber) + ", BRD: " + String(targetBoard) + ", PAY:";
            for (int i = 0; i < argByte.size(); i++) str += (" " + String::toHexString((void*)(argByte.getRawDataPointer() + i), 1));
            return str;
        }
    };


public:

    CommandParser();
    ~CommandParser();

    FirmwareSupport::Error parseAndSendCommand(String command);
    
    StringArray getResponseQueue(bool clearQueue = true);

private:

    //============================================================================
    // Implementation of TerpstraMidiDriver::Listener
    
    virtual void midiMessageReceived(MidiInput* source, const MidiMessage& midiMessage) override;

    virtual void midiMessageSent(const MidiMessage& midiMessage) override {};

    virtual void midiSendQueueSize(int queueSize) override {};

    virtual void generalLogMessage(String textMessage, HajuErrorVisualizer::ErrorLevel errorLevel) override {};

    virtual void noAnswerToMessage(const MidiMessage& midiMessage) override {};

private:

    CommandDefinition parseCommand(String command);

    void sendCommand(CommandDefinition commandDefinition);

    MidiMessage commandToSysEx(CommandDefinition definition);

private:

    LumatoneFirmwareVersion currentVersion;
    StringArray logQueue;

    TerpstraMidiDriver& midiDriver;
};

class CommandLineComponent : public juce::Component, public juce::Timer, public juce::TextEditor::Listener
{
public:

    CommandLineComponent();
    ~CommandLineComponent();

    void paint(juce::Graphics& g) override;

    void resized() override;

    void timerCallback() override;

    void textEditorReturnKeyPressed(TextEditor& editor) override;

    void textEditorEscapeKeyPressed(TextEditor& editor) override;


private:
    
    CommandParser parser;

    std::unique_ptr<TextEditor> commandLog;
    std::unique_ptr<TextEditor> inputBox;
};
