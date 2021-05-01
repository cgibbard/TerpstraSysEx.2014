/*
  ==============================================================================

    LumatoneCommandManager.cpp
    Created: 17 Apr 2021 10:17:31pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "LumatoneController.h"
#include "Main.h"


LumatoneController::LumatoneController()
    : errorVisualizer(TerpstraSysExApplication::getApp().getLookAndFeel())
{
    midiDriver.addListener(this);
}

LumatoneController::~LumatoneController()
{

}

void LumatoneController::setSysExSendingMode(sysExSendingMode newMode)
{
    //if (newMode != editingMode)
    //{
    //    editingMode = newMode;
    //    if (editingMode== sysExSendingMode::offlineEditor)
    //    {
    //        clearMIDIMessageBuffer();	// ToDo remove only SysEx messages (leave NoteOn/NoteOff)?
    //        stopTimer();
    //        hasMsgWaitingForAck = false;
    //    }
    //}
}

// Takes a generic firmware version and parses it into a recognized firmware version
void LumatoneController::setFirmwareVersion(FirmwareVersion firmwareVersionIn)
{
    firmwareVersion = firmwareVersionIn;
    setFirmwareVersion(firmwareSupport.getLumatoneFirmwareVersion(firmwareVersion), false);
}

// Takes a recognized firmware version
void LumatoneController::setFirmwareVersion(LumatoneFirmwareVersion lumatoneVersion, bool parseVersion)
{
    determinedVersion = lumatoneVersion;
    octaveSize = firmwareSupport.getOctaveSize(determinedVersion);

    if (parseVersion)
        firmwareVersion = FirmwareVersion::fromDeterminedVersion(determinedVersion);

    firmwareListeners.call(&FirmwareListener::firmwareRevisionReceived, firmwareVersion.major, firmwareVersion.minor, firmwareVersion.revision);
}


void LumatoneController::setMidiInput(int deviceIndex)
{
    midiDriver.setMidiInput(deviceIndex);
    currentDevicePairEstablished = false;
    testCurrentDeviceConnection();
}

void LumatoneController::setMidiOutput(int deviceIndex)
{
    midiDriver.setMidiOutput(deviceIndex);
    currentDevicePairEstablished = false;
    testCurrentDeviceConnection();
}

/*
==============================================================================
Combined (hi-level) commands
*/


void LumatoneController::sendAllParamsOfBoard(int boardIndex, TerpstraKeys boardData)
{
    if (determinedVersion >= LumatoneFirmwareVersion::VERSION_1_0_11)
    {
        for (int keyIndex = 0; keyIndex < octaveSize; keyIndex++)
        {
            auto key = &boardData.theKeys[keyIndex];
            midiDriver.sendKeyFunctionParameters(boardIndex, keyIndex, key->noteNumber, key->channelNumber, key->keyType);
            midiDriver.sendKeyLightParameters(boardIndex, keyIndex, key->colour.getRed(), key->colour.getGreen(), key->colour.getBlue());
        }
    }
    else
    {
        for (int keyIndex = 0; keyIndex < octaveSize; keyIndex++)
        {
            auto key = &boardData.theKeys[keyIndex];
            midiDriver.sendKeyFunctionParameters(boardIndex, keyIndex, key->noteNumber, key->channelNumber, key->keyType);
            midiDriver.sendKeyLightParameters_Version_1_0_0(boardIndex, keyIndex, key->colour.getRed() / 2, key->colour.getGreen() / 2, key->colour.getBlue() / 2);
        }
    }
}

void LumatoneController::sendCompleteMapping(TerpstraKeyMapping mappingData)
{
    for (int boardIndex = 1; boardIndex <= NUMBEROFBOARDS; boardIndex++)
        sendAllParamsOfBoard(boardIndex, mappingData.sets[boardIndex - 1]);
}

void LumatoneController::sendGetMappingOfBoardRequest(int boardIndex)
{
    getRedLEDConfig(boardIndex);
    getGreenLEDConfig(boardIndex);
    getBlueLEDConfig(boardIndex);
    getChannelConfig(boardIndex);
    getNoteConfig(boardIndex);
    getKeyTypeConfig(boardIndex);
}

void LumatoneController::sendGetCompleteMappingRequest()
{
    for (int boardIndex = 1; boardIndex <= NUMBEROFBOARDS; boardIndex++)
        sendGetMappingOfBoardRequest(boardIndex);
}

void LumatoneController::resetVelocityConfig(TerpstraVelocityCurveConfig::VelocityCurveType velocityCurveType)
{
    switch (velocityCurveType)
    {
    case TerpstraVelocityCurveConfig::VelocityCurveType::noteOnNoteOff:
        resetVelocityConfig();
        break;
    case TerpstraVelocityCurveConfig::VelocityCurveType::fader:
        resetFaderConfig();
        break;
    case TerpstraVelocityCurveConfig::VelocityCurveType::afterTouch:
        resetAftertouchConfig();
        break;
    case TerpstraVelocityCurveConfig::VelocityCurveType::lumaTouch:
        resetLumatouchConfig();
        break;
    default:
        jassert(false);
        break;
    }
}

int LumatoneController::sendTestMessageToDevice(int deviceIndex, int pingId)
{
    int value = (pingId < 0)
        ? deviceIndex
        : pingId;

    if (determinedVersion >= LumatoneFirmwareVersion::VERSION_1_0_9)
    {
        midiDriver.ping(value, deviceIndex);
    }
    else
    {
        midiDriver.sendGetSerialIdentityRequest(deviceIndex);
    }

    return value;
}

void LumatoneController::testCurrentDeviceConnection()
{
    //TODO
    // On confirmed connection send connection listener message
    if (midiDriver.hasDevicesDefined())
    {
        waitingForTestResponse = true;

        if (determinedVersion >= LumatoneFirmwareVersion::VERSION_1_0_9)
        {
            pingLumatone(0xf);
        }
        else
        {
            sendGetSerialIdentityRequest();
        }
    }
}

// Send parametrization of one key to the device
void LumatoneController::sendKeyParam(int boardIndex, int keyIndex, TerpstraKey keyData)
{
    sendKeyConfig(boardIndex, keyIndex, keyData.noteNumber, keyData.channelNumber, keyData.keyType /*, TODO fader cc polarity */);
    sendKeyColourConfig(boardIndex, keyIndex, keyData.colour);
}

// Send configuration of a certain look up table
void LumatoneController::sendTableConfig(TerpstraVelocityCurveConfig::VelocityCurveType velocityCurveType, const uint8* table)
{
    switch (velocityCurveType)
    {
    case TerpstraVelocityCurveConfig::VelocityCurveType::fader:
        setFaderConfig(table);
        break;

    case TerpstraVelocityCurveConfig::VelocityCurveType::afterTouch:
        setAftertouchConfig(table);
        break;

    case TerpstraVelocityCurveConfig::VelocityCurveType::lumaTouch:
        setLumatouchConfig(table);
        break;

    default:
        sendVelocityConfig(table);
    }
}

//=============================================================================
// Mid-level firmware functions

// Send note, channel, cc, and fader polarity data
void LumatoneController::sendKeyConfig(int boardIndex, int keyIndex, int noteOrCCNum, int channel, LumatoneKeyType keyType, bool faderUpIsNull)
{
    midiDriver.sendKeyFunctionParameters(boardIndex, keyIndex, noteOrCCNum, channel, keyType, faderUpIsNull);
}

void LumatoneController::sendKeyColourConfig(int boardIndex, int keyIndex, Colour colour)
{
    if (determinedVersion >= LumatoneFirmwareVersion::VERSION_1_0_11)
        midiDriver.sendKeyLightParameters(boardIndex, keyIndex, colour.getRed(), colour.getGreen(), colour.getBlue());
    else
        midiDriver.sendKeyLightParameters_Version_1_0_0(boardIndex, keyIndex, colour.getRed() / 2, colour.getGreen() / 2, colour.getBlue() / 2);
}


// Send expression pedal sensivity
void LumatoneController::sendExpressionPedalSensivity(unsigned char value)
{
    midiDriver.sendExpressionPedalSensivity(value);
}

// Send parametrization of foot controller
void LumatoneController::sendInvertFootController(bool value)
{
    midiDriver.sendInvertFootController(value);
}

// Colour for macro button in active state
void LumatoneController::sendMacroButtonActiveColour(String colourAsString)
{
    auto c = Colour::fromString(colourAsString);
    if (determinedVersion >= LumatoneFirmwareVersion::VERSION_1_0_11)
        midiDriver.sendMacroButtonActiveColour(c.getRed(), c.getGreen(), c.getBlue());
    else
        midiDriver.sendMacroButtonActiveColour_Version_1_0_0(c.getRed(), c.getGreen(), c.getBlue());
}

// Colour for macro button in inactive state
void LumatoneController::sendMacroButtonInactiveColour(String colourAsString)
{
    auto c = Colour::fromString(colourAsString);
    if (determinedVersion >= LumatoneFirmwareVersion::VERSION_1_0_11)
        midiDriver.sendMacroButtonInactiveColour(c.getRed(), c.getGreen(), c.getBlue());
    else
        midiDriver.sendMacroButtonInactiveColour_Version_1_0_0(c.getRed(), c.getGreen(), c.getBlue());
}

// Send parametrization of light on keystrokes
void LumatoneController::sendLightOnKeyStrokes(bool value)
{
    midiDriver.sendLightOnKeyStrokes(value);
}

// Send a value for a velocity lookup table
void LumatoneController::sendVelocityConfig(const uint8 velocityTable[])
{
    midiDriver.sendVelocityConfig(velocityTable);
}

// Save velocity config to EEPROM
void LumatoneController::saveVelocityConfig()
{
    midiDriver.saveVelocityConfig();
}

void LumatoneController::resetVelocityConfig()
{
    midiDriver.resetVelocityConfig();
}

void LumatoneController::setFaderConfig(const uint8 faderTable[])
{
    midiDriver.sendFaderConfig(faderTable);
}

void LumatoneController::resetFaderConfig()
{
    midiDriver.resetFaderConfig();
}

void LumatoneController::setAftertouchEnabled(bool value)
{
    midiDriver.sendAfterTouchActivation(value);
}

void LumatoneController::startCalibrateAftertouch()
{
    midiDriver.sendCalibrateAfterTouch();
}

void LumatoneController::setAftertouchConfig(const uint8 aftertouchTable[])
{
    midiDriver.sendAftertouchConfig(aftertouchTable);
}

void LumatoneController::resetAftertouchConfig()
{
    midiDriver.resetAftertouchConfig();
}

void LumatoneController::setVelocityIntervalConfig(const int velocityIntervalTable[])
{
    midiDriver.sendVelocityIntervalConfig(velocityIntervalTable);
}

void LumatoneController::getRedLEDConfig(int boardIndex)
{
    midiDriver.sendRedLEDConfigRequest(boardIndex);
}

void LumatoneController::getGreenLEDConfig(int boardIndex)
{
    midiDriver.sendGreenLEDConfigRequest(boardIndex);
}

void LumatoneController::getBlueLEDConfig(int boardIndex)
{
    midiDriver.sendBlueLEDConfigRequest(boardIndex);
}

void LumatoneController::getChannelConfig(int boardIndex)
{
    midiDriver.sendChannelConfigRequest(boardIndex);
}

void LumatoneController::getNoteConfig(int boardIndex)
{
    midiDriver.sendNoteConfigRequest(boardIndex);
}

void LumatoneController::getKeyTypeConfig(int boardIndex)
{
    midiDriver.sendKeyTypeConfigRequest(boardIndex);
}

void LumatoneController::sendVelocityConfigRequest()
{
    midiDriver.sendVelocityConfigRequest();
}

void LumatoneController::sendFaderConfigRequest()
{
    midiDriver.sendFaderConfigRequest();
}

void LumatoneController::sendAftertouchConfigRequest()
{
    midiDriver.sendAftertouchConfigRequest();
}

void LumatoneController::sendVelocityIntervalConfigRequest()
{
    midiDriver.sendVelocityIntervalConfigRequest();
}

// This command is used to read back the serial identification number of the keyboard.
void LumatoneController::sendGetSerialIdentityRequest()
{
    midiDriver.sendGetSerialIdentityRequest();
}

void LumatoneController::startCalibrateKeys()
{
    midiDriver.sendCalibrateKeys();
}

void LumatoneController::setCalibratePitchModWheel(bool startCalibration)
{
    midiDriver.sendCalibratePitchModWheel(startCalibration);
}

void LumatoneController::setLumatouchConfig(const uint8 lumatouchTable[])
{
    midiDriver.setLumatouchConfig(lumatouchTable);
}

void LumatoneController::resetLumatouchConfig()
{
    midiDriver.resetLumatouchConfig();
}

void LumatoneController::getLumatouchConfig()
{
    midiDriver.sendLumatouchConfigRequest();
}

// This command is used to read back the current Lumatone firmware revision.
void LumatoneController::sendGetFirmwareRevisionRequest()
{
    midiDriver.sendGetFirmwareRevisionRequest();
}

// Send a value for the Lumatone to echo back; used for auto device connection and monitoring
int LumatoneController::pingLumatone(uint8 pingId)
{
    return midiDriver.ping(pingId);
}

//=============================================================================
// Communication and broadcasting

void LumatoneController::midiMessageReceived(const MidiMessage& midiMessage)
{
    responseQueue.addEvent(midiMessage, sampleNum++);
    if (!bufferReadRequested)
    {
        bufferReadRequested = true;
        startTimer(bufferReadTimeoutMs);
    }
}

void LumatoneController::midiMessageSent(const MidiMessage& midiMessage)
{

}

void LumatoneController::midiSendQueueSize(int queueSize)
{

}

void LumatoneController::generalLogMessage(String textMessage, HajuErrorVisualizer::ErrorLevel errorLevel)
{

}

FirmwareSupport::Error LumatoneController::handleOctaveConfigResponse(
    const MidiMessage& midiMessage, 
    std::function < FirmwareSupport::Error(const MidiMessage&, int&, uint8, int*)> unpackFunction,
    std::function <void(int,void*)> callbackFunctionIfNoError)
{
    int boardId = -1;
    int channelData[56];
    
    auto errorCode = unpackFunction(midiMessage, boardId, octaveSize, channelData);
    if (errorCode == FirmwareSupport::Error::noError)
    {
        callbackFunctionIfNoError(boardId, channelData);
    }

    return errorCode;
}

FirmwareSupport::Error LumatoneController::handleTableConfigResponse(
    const MidiMessage& midiMessage,
    std::function<FirmwareSupport::Error(const MidiMessage&, int*)> unpackFunction,
    std::function <void(void*)> callbackFunctionIfNoError)
{
    int veloctiyData[128];
    auto errorCode = unpackFunction(midiMessage, veloctiyData);
    if (errorCode == FirmwareSupport::Error::noError)
        callbackFunctionIfNoError(veloctiyData);
    
    return errorCode;
}

FirmwareSupport::Error LumatoneController::handleLEDConfigResponse(const MidiMessage& midiMessage)
{
    int boardId = -1;
    int colourData[56];

    int cmd = firmwareSupport.getCommandNumber(midiMessage);
    FirmwareSupport::Error errorCode;

    // Use correct unpacking function
    if (determinedVersion < LumatoneFirmwareVersion::VERSION_1_0_11)
    {
        errorCode = midiDriver.unpackGetLEDConfigResponse_Version_1_0_0(midiMessage, boardId, octaveSize, colourData);
    }
    else
    {
        errorCode = midiDriver.unpackGetLEDConfigResponse(midiMessage, boardId, colourData);
    }

    if (errorCode == FirmwareSupport::Error::noError)
    {
        int colorCode = cmd - GET_RED_LED_CONFIG;
        firmwareListeners.call(&FirmwareListener::octaveColourConfigReceived, boardId, colorCode, colourData);
    }
    
    return errorCode;
}

FirmwareSupport::Error LumatoneController::handleChannelConfigResponse(const MidiMessage& midiMessage)
{
    auto unpack = [&](const MidiMessage& msg, int& boardId, uint8 numKeys, int* data) {
        auto errorCode = midiDriver.unpackGetChannelConfigResponse(msg, boardId, numKeys, data);
        if (errorCode == FirmwareSupport::Error::noError)
            for (int i = 0; i < numKeys; i++)
                data[i]++; // MIDI Channels are 1-based
        return errorCode;
    };
    auto callback = [&](int boardId, void* data) { firmwareListeners.call(&FirmwareListener::octaveChannelConfigReceived, boardId, (int*)data); };
    return handleOctaveConfigResponse(midiMessage, unpack, callback);
}

FirmwareSupport::Error LumatoneController::handleNoteConfigResponse(const MidiMessage& midiMessage)
{
    auto unpack = [&](const MidiMessage& msg, int& boardId, uint8 numKeys, int* data) {
        return midiDriver.unpackGetNoteConfigResponse(msg, boardId, numKeys, data);
    };
    auto callback = [&](int boardId, void* data) { firmwareListeners.call(&FirmwareListener::octaveNoteConfigReceived, boardId, (int*)data); };
    return handleOctaveConfigResponse(midiMessage, unpack, callback);
}

FirmwareSupport::Error LumatoneController::handleKeyTypeConfigResponse(const MidiMessage& midiMessage)
{
    auto unpack = [&](const MidiMessage& msg, int& boardId, uint8 numKeys, int* data) {
        return midiDriver.unpackGetTypeConfigResponse(msg, boardId, numKeys, data);
    };
    auto callback = [&](int boardId, void* data) { firmwareListeners.call(&FirmwareListener::keyTypeConfigReceived, boardId, (int*)data); };
    return handleOctaveConfigResponse(midiMessage, unpack, callback);
}

FirmwareSupport::Error LumatoneController::handleVelocityConfigResponse(const MidiMessage& midiMessage)
{
    auto unpack = [&](const MidiMessage& msg, int* data) {
        return midiDriver.unpackGetVelocityConfigResponse(msg, data);
    };
    auto callback = [&](void* data) { firmwareListeners.call(&FirmwareListener::velocityConfigReceived, (int*)data); };
    return handleTableConfigResponse(midiMessage, unpack, callback);
}

FirmwareSupport::Error LumatoneController::handleAftertouchConfigResponse(const MidiMessage& midiMessage)
{
    auto unpack = [&](const MidiMessage& msg, int* data) {
        return midiDriver.unpackGetAftertouchConfigResponse(msg, data);
    };
    auto callback = [&](void* data) { firmwareListeners.call(&FirmwareListener::aftertouchConfigReceived, (int*)data); };
    return handleTableConfigResponse(midiMessage, unpack, callback);
}

FirmwareSupport::Error LumatoneController::handleVelocityIntervalConfigResponse(const MidiMessage& midiMessage)
{
    auto unpack = [&](const MidiMessage& msg, int* data) {
        auto errorCode = midiDriver.unpackGetVelocityConfigResponse(msg, data);
        if (errorCode == FirmwareSupport::Error::noError)
        {
            // Reverse velocity table - maybe should move this to recepient end
            int* dataCopy[VELOCITYINTERVALTABLESIZE];
            memmove(dataCopy, data, sizeof(int) * VELOCITYINTERVALTABLESIZE);
            for (int i = 0; i < VELOCITYINTERVALTABLESIZE; i++)
            {
                data[i] = *dataCopy[VELOCITYINTERVALTABLESIZE - 1 - i];
            }
        }
        return errorCode;
    };
    auto callback = [&](void* data) { firmwareListeners.call(&FirmwareListener::velocityIntervalConfigReceived, (int*)data); };
    return handleTableConfigResponse(midiMessage, unpack, callback);
}

FirmwareSupport::Error LumatoneController::handleFaderConfigResponse(const MidiMessage& midiMessage)
{
    auto unpack = [&](const MidiMessage& msg, int* data) {
        return midiDriver.unpackGetFaderConfigResponse(msg, data);
    };
    auto callback = [&](void* data) { firmwareListeners.call(&FirmwareListener::faderConfigReceived, (int*)data); };
    return handleTableConfigResponse(midiMessage, unpack, callback);
}

FirmwareSupport::Error LumatoneController::handleSerialIdentityResponse(const MidiMessage& midiMessage)
{
    int serialBytes[6];
    auto errorCode = midiDriver.unpackGetSerialIdentityResponse(midiMessage, serialBytes);
    if (errorCode == FirmwareSupport::Error::noError)
    {
        connectedSerialNumber = serialIdentityToString(serialBytes);
        DBG("Device serial is: " + connectedSerialNumber);

        firmwareListeners.call(&FirmwareListener::serialIdentityReceived, serialBytes);
        
        if (waitingForTestResponse)
            emitConnectionEstablishedMessage();
        
        else if (!midiDriver.hasDevicesDefined())
            statusListeners.call(&StatusListener::connectionChanged, true);
    }
    return errorCode;
}

FirmwareSupport::Error LumatoneController::handleFirmwareRevisionResponse(const MidiMessage& midiMessage)
{
    int major, minor, revision;
    auto errorCode = midiDriver.unpackGetFirmwareRevisionResponse(midiMessage, major, minor, revision);
    if (errorCode == FirmwareSupport::Error::noError)
    {
        // Listener call handled here
        setFirmwareVersion(FirmwareVersion(major, minor, revision));
    }

    return errorCode;
}

FirmwareSupport::Error LumatoneController::handleLumatouchConfigResponse(const MidiMessage& midiMessage)
{
    auto unpack = [&](const MidiMessage& msg, int* data) {
        return midiDriver.unpackGetLumatouchConfigResponse(msg, data);
    };
    auto callback = [&](void* data) { firmwareListeners.call(&FirmwareListener::lumatouchConfigReceived, (int*)data); };
    return handleTableConfigResponse(midiMessage, unpack, callback);
}

FirmwareSupport::Error LumatoneController::handlePingResponse(const MidiMessage& midiMessage)
{
    auto errorCode = FirmwareSupport::Error::noError;
    if (midiMessage.getSysExData()[MSG_STATUS] == PING_ECHO)
        errorCode = FirmwareSupport::Error::messageIsNotResponseToCommand;
    else
        firmwareListeners.call(&FirmwareListener::pingResponseReceived, midiMessage.getSysExData()[PAYLOAD_INIT + 1]);

    return errorCode;
}

void LumatoneController::handleMidiDriverError(FirmwareSupport::Error errorToHandle, int commandReceived)
{
    jassertfalse;
    DBG("ERROR: " + firmwareSupport.errorToString(errorToHandle));

    if (errorToHandle == FirmwareSupport::Error::messageIsNotResponseToCommand)
    {
        if (commandReceived == LUMA_PING)
        {
            // Add to block list?
        }
    }

    // Generic handling
    if (commandReceived < CHANGE_KEY_NOTE)
    {

    }
    
    // Command specific
    else
    {

    }
}

void LumatoneController::timerCallback()
{
    stopTimer();

    if (readSample >= sampleNum)
    {
        sampleNum = 0;
        readSample = 0;
        responseQueue.clear();
    }
    else
    {
        MidiBuffer readBuffer;
        readBuffer.addEvents(responseQueue, readSample, bufferReadSize, 0);
        int maxSample = readSample + bufferReadSize;
        for (auto event : readBuffer)
        {
            auto midiMessage = event.getMessage();
            //DBG("READING: " + midiMessage.getDescription());
            if (midiMessage.isSysEx())
            {
                auto errorCode = FirmwareSupport::Error::noError;
                auto sysExData = midiMessage.getSysExData();

                switch (sysExData[MSG_STATUS])
                {
                case TerpstraMIDIAnswerReturnCode::NACK:  // Not recognized
                    errorVisualizer.setErrorLevel(
                        HajuErrorVisualizer::ErrorLevel::error,
                        "Not Recognized");
                    errorCode = FirmwareSupport::Error::unknownCommand;
                    break;

                case TerpstraMIDIAnswerReturnCode::ACK:  // Acknowledged, OK
                    errorVisualizer.setErrorLevel(
                        HajuErrorVisualizer::ErrorLevel::noError,
                        "Ack");
                    break;

                case TerpstraMIDIAnswerReturnCode::BUSY: // Controller busy
                    errorVisualizer.setErrorLevel(
                        HajuErrorVisualizer::ErrorLevel::warning,
                        "Busy");
                    errorCode = FirmwareSupport::Error::deviceIsBusy;
                    break;

                case TerpstraMIDIAnswerReturnCode::ERROR:    // Error
                    errorVisualizer.setErrorLevel(
                        HajuErrorVisualizer::ErrorLevel::error,
                        "Error from device");
                    errorCode = FirmwareSupport::Error::externalError;
                    break;

                default:
                    errorVisualizer.setErrorLevel(
                        HajuErrorVisualizer::ErrorLevel::noError,
                        "");
                    break;
                }

                unsigned int cmd = sysExData[CMD_ID];

                if (errorCode == FirmwareSupport::Error::noError)
                {
                    switch (cmd)
                    {
                    case GET_RED_LED_CONFIG:
                        errorCode = handleLEDConfigResponse(midiMessage);
                        break;

                    case GET_GREEN_LED_CONFIG:
                        errorCode = handleLEDConfigResponse(midiMessage);
                        break;

                    case GET_BLUE_LED_CONFIG:
                        errorCode = handleLEDConfigResponse(midiMessage);
                        break;

                    case GET_CHANNEL_CONFIG:
                        errorCode = handleChannelConfigResponse(midiMessage);
                        break;

                    case GET_NOTE_CONFIG:
                        errorCode = handleNoteConfigResponse(midiMessage);
                        break;

                    case GET_KEYTYPE_CONFIG:
                        errorCode = handleKeyTypeConfigResponse(midiMessage);
                        break;

                        // TODO

                    case GET_VELOCITY_CONFIG:
                        errorCode = handleVelocityConfigResponse(midiMessage);
                        break;

                    case GET_SERIAL_IDENTITY:
                        errorCode = handleSerialIdentityResponse(midiMessage);
                        break;

                    case GET_LUMATOUCH_CONFIG:
                        errorCode = handleLumatouchConfigResponse(midiMessage);
                        break;

                    case GET_FIRMWARE_REVISION:
                        errorCode = handleFirmwareRevisionResponse(midiMessage);
                        break;

                    case LUMA_PING:
                        errorCode = handlePingResponse(midiMessage);
                        break;

                    default:
                        if (sysExData[MSG_STATUS] == 1)
                        {
                            if (event.numBytes > 8)
                                DBG("WARNING UNIMPLEMENTED RESPONSE HANDLING CMD " + String(cmd));
                        }
                        else
                        {
                            DBG("UNHANDLED EXCEPTION");
                        }
                        ;
                    }
                }

                if (errorCode != FirmwareSupport::Error::noError)
                {
                    handleMidiDriverError(errorCode, cmd);
                }

            }
            readSample++;
            jassert(readSample <= sampleNum);
        }

        if (maxSample < sampleNum)
            startTimer(bufferReadTimeoutMs);
    }

    bufferReadRequested = false;
}

void LumatoneController::testResponseReceived()
{
    waitingForTestResponse = false;
    if (!currentDevicePairEstablished)
    {
        emitConnectionEstablishedMessage();
    }
}

void LumatoneController::emitConnectionEstablishedMessage()
{
    statusListeners.call(&StatusListener::connectionEstablished, midiDriver.getLastMidiInputIndex(), midiDriver.getLastMidiOutputIndex());

    // Identity returned on 55-keys version
    if (connectedSerialNumber == "00 00 00 00 00 00")
        setFirmwareVersion(LumatoneFirmwareVersion::VERSION_55_KEYS);
    else
        midiDriver.sendGetFirmwareRevisionRequest();
}

String LumatoneController::serialIdentityToString(int* serialBytes)
{
    return String::toHexString(serialBytes, 6);
}

