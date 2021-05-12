/*
  ==============================================================================

    CommandLineInterface.cpp
    Created: 6 May 2021 7:16:03pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "CommandLineInterface.h"
#include "../Main.h"

CommandParser::CommandParser()
    : midiDriver(TerpstraSysExApplication::getApp().getLumatoneController().getMidiDriver())
{
    midiDriver.addListener(this);
}

CommandParser::~CommandParser()
{
    midiDriver.removeListener(this);
}

FirmwareSupport::Error CommandParser::parseAndSendCommand(String command)
{
    auto definition = parseCommand(command);
    if (definition.errorStatus == FirmwareSupport::Error::noError)
    {
        sendCommand(definition);
        return FirmwareSupport::Error::noError;
    }

    return definition.errorStatus;
}

StringArray CommandParser::getResponseQueue(bool clearQueue)
{
    auto queue = logQueue;
    if (clearQueue)
        logQueue.clear();
    return queue;
}

void CommandParser::midiMessageReceived(MidiInput* source, const MidiMessage& midiMessage)
{
    // For now just log any received message that is not a test message
    if (midiMessage.isSysEx())
    {
        auto sysExData = midiMessage.getSysExData();
        switch (sysExData[CMD_ID])
        {
        // For now, ignore monitoring messages by simply ignoring these two commands
        case GET_SERIAL_IDENTITY:
            break;

        case LUMA_PING:
            break;

        default:
            logQueue.add(midiMessage.getDescription());
        }
    }
}

CommandParser::CommandDefinition CommandParser::parseCommand(String command)
{
    auto tokens = StringArray::fromTokens(command.trim(), false);

    // Command structure
    // If first token represents a byte interpret as raw byte message.
    //   - If it starts with SysEx header F0, it will need to be a complete message
    //   - If not it's assumed to be <cmd_id> <board_id> <byte0> <byte1> ... <byteN>
    // If first token is "send", the next token needs to be a valid command name
    //   - TODO

    CommandDefinition definition;

    int payloadStart = -1;

    // Raw SysEx Message
    if (tokens[0] == "F0")
    {
        if (tokens[tokens.size() - 1] == "F7")
        {
            payloadStart = PAYLOAD_INIT + 1;
            definition.cmdNumber = tokens[CMD_ID + 1].getHexValue32();
            definition.targetBoard = tokens[BOARD_IND + 1].getHexValue32();
            definition.payloadLength = (tokens.size() - 1) - payloadStart;
        }
    }

    // Skip manufacturer's id
    else if (tokens[0].containsOnly("aAbBcCdDeEfF0123456789"))
    {
        payloadStart = 2;
        definition.cmdNumber = tokens[0].getHexValue32();
        definition.targetBoard = tokens[1].getHexValue32();
        definition.payloadLength = tokens.size() - 2;
    }

    // Word-command
    else
    {
        // TODO
    }


    if (definition.cmdNumber < 0 && definition.cmdNumber > GET_EXPRESSION_PEDAL_THRESHOLD)
        definition.errorStatus = FirmwareSupport::Error::unknownCommand;

    if (definition.targetBoard < 0 || definition.targetBoard > 0x05)
        definition.errorStatus = FirmwareSupport::Error::messageHasInvalidBoardIndex;


    // TODO abstract into command
    definition.errorStatus = FirmwareSupport::Error::noError;

    if (definition.errorStatus == FirmwareSupport::Error::noError)
    {
        int endIndex = payloadStart + definition.payloadLength;
        for (int i = payloadStart; i < endIndex && i < tokens.size(); i++)
        {
            definition.argByte.add(tokens[i].getHexValue32());
        }

        DBG("Parsed: " + definition.toString());
    }
    else
    {
    }

    return definition;
}

void CommandParser::sendCommand(CommandDefinition commandDefinition)
{
    if (commandDefinition.errorStatus == FirmwareSupport::Error::noError)
    {
        if (commandDefinition.commandIsWrapped)
        {
            // TODO
        }
        else
        {
            MidiMessage message = commandToSysEx(commandDefinition);
            midiDriver.sendMessageWithAcknowledge(message);
        }
    }
}

MidiMessage CommandParser::commandToSysEx(CommandDefinition definition)
{
    // Upper-bound size
    uint8 sysExData[255];

    sysExData[MANU_0] = MANUFACTURER_ID_0;
    sysExData[MANU_1] = MANUFACTURER_ID_1;
    sysExData[MANU_2] = MANUFACTURER_ID_2;
    sysExData[BOARD_IND] = definition.targetBoard;
    sysExData[CMD_ID] = definition.cmdNumber;

    for (int i = 0; i < definition.payloadLength; i++)
    {
        sysExData[(PAYLOAD_INIT - 1) + i] = definition.argByte[i];
    }

    if (definition.payloadLength < 4)
    {
        for (int i = 0; i < 4 - definition.payloadLength; i++)
        {
            sysExData[i + (PAYLOAD_INIT - 1) + definition.payloadLength] = '\0';
        }

        definition.payloadLength = 4;
    }

    return MidiMessage::createSysExMessage(sysExData, 5 + definition.payloadLength);
}


CommandLineComponent::CommandLineComponent()
    : Component("Lumatone Developer Command Line")
{
    commandLog.reset(new TextEditor("CommandLineLogger"));
    commandLog->setMultiLine(true, false);
    commandLog->setReadOnly(true);
    addAndMakeVisible(commandLog.get());

    inputBox.reset(new TextEditor("CommandInputBox"));
    inputBox->setMultiLine(false);
    addAndMakeVisible(inputBox.get());
    inputBox->addListener(this);

    startTimer(100);
}

CommandLineComponent::~CommandLineComponent()
{

}

void CommandLineComponent::paint(juce::Graphics& g)
{

}

void CommandLineComponent::resized()
{
    commandLog->setBounds(getLocalBounds().withBottom(getHeight() * 0.9f));
    inputBox->setBounds(getLocalBounds().withTop(commandLog->getBottom()));
}

void CommandLineComponent::timerCallback()
{
    auto msgQueue = parser.getResponseQueue();
    for (auto msg : msgQueue)
    {
        commandLog->setText(commandLog->getText() + "\n\t" + msg);
    }
}

void CommandLineComponent::textEditorReturnKeyPressed(TextEditor& editor)
{
    parser.parseAndSendCommand(editor.getText());
    commandLog->setText(commandLog->getText() + "\n" + editor.getText());
    editor.clear();
}

void CommandLineComponent::textEditorEscapeKeyPressed(TextEditor& editor)
{
    editor.clear();
}

