/*
  ==============================================================================

    DeviceActivityMonitor.cpp
    Created: 11 Feb 2021 9:15:43pm
    Author:  Vincenzo

  ==============================================================================
*/

#include "DeviceActivityMonitor.h"
#include "Main.h"


DeviceActivityMonitor::DeviceActivityMonitor()
{
}

DeviceActivityMonitor::~DeviceActivityMonitor()
{
    closeInputDevices();
    closeOutputDevices();
}

void DeviceActivityMonitor::initializeDeviceDetection()
{
    TerpstraMidiDriver& midiDriver = TerpstraSysExApplication::getApp().getMidiDriver();
    monitorMessage = midiDriver.getSerialIdentityRequestMessage();
    TerpstraSysExApplication::getApp().log("The pinging MIDI message is:");
    TerpstraSysExApplication::getApp().log(monitorMessage.getDescription());

    if (deviceConnectionMode != DetectConnectionMode::lookingForDevice)
    {
        deviceConnectionMode = DetectConnectionMode::lookingForDevice;
    }
    
    // Refresh available devices
    midiDriver.refreshDeviceLists();

    closeInputDevices();
    closeOutputDevices();
    pingOutputIndex = -1;

    TerpstraSysExApplication::getApp().log("MIDI Output devices found:");
    for (auto outputDeviceInfo : MidiOutput::getAvailableDevices())
    {
        outputsToPing.add(MidiOutput::openDevice(outputDeviceInfo.identifier));
        TerpstraSysExApplication::getApp().log("\t" + outputDeviceInfo.name + " - " + outputDeviceInfo.identifier);
    }

    TerpstraSysExApplication::getApp().log("MIDI Input devices found:");
    for (auto inputDeviceInfo : MidiInput::getAvailableDevices())
    {
        auto input = inputsListening.add(MidiInput::openDevice(inputDeviceInfo.identifier, this));
        TerpstraSysExApplication::getApp().log("\t" + inputDeviceInfo.name + " - " + inputDeviceInfo.identifier);
        input->start();
    }

    if (outputsToPing.size() > 0 && inputsListening.size() > 0)
    {
        expectedResponseReceived = false;
        deviceDetectInProgress = true;
        pingNextOutput();
    }
    else
    {
        TerpstraSysExApplication::getApp().log("No MIDI devices detected.");
        deviceDetectInProgress = false;
        startTimer(pingRoutineTimeoutMs);
    }
}

void DeviceActivityMonitor::pingNextOutput()
{
    pingOutputIndex++;

    if (pingOutputIndex >= 0 && pingOutputIndex < outputsToPing.size() && outputsToPing[pingOutputIndex])
    {
        TerpstraSysExApplication::getApp().log("Pinging output " + outputsToPing[pingOutputIndex]->getName());

        outputsToPing[pingOutputIndex]->sendMessageNow(monitorMessage);
        startTimer(responseTimeoutMs);
    }

    // Tried all devices, set timeout for next attempt
    else
    {
        TerpstraSysExApplication::getApp().log("Detect device timeout.\n");
        deviceDetectInProgress = false;
        startTimer(pingRoutineTimeoutMs);
    }
}

void DeviceActivityMonitor::stopDeviceDetection()
{
    if (deviceConnectionMode == DetectConnectionMode::lookingForDevice)
    {
        deviceDetectInProgress = false;
        TerpstraSysExApplication::getApp().log("Stopping detection.");
        stopTimer();
    }
}

//void DeviceActivityMonitor::pingAvailableDevices()
//{
//    const MidiMessage pingMsg = TerpstraSysExApplication::getApp().getMidiDriver().getSerialIdentityRequestMessage();
//
//    for (auto output : outputsToPing)
//    {
//        output
//    }
//}

void DeviceActivityMonitor::intializeConnectionLossDetection()
{
    deviceConnectionMode = DetectConnectionMode::waitingForConnectionLoss;

    // TODO
}

void DeviceActivityMonitor::closeInputDevices()
{
    for (auto inputs : inputsListening)
    {
        inputs->stop();
    }

    inputsListening.clear();
}

//void DeviceActivityMonitor::closeInputDevicesExcept(int inputDeviceIndexToRetain)
//{
//    if (inputDeviceIndexToRetain >= 0 && inputDeviceIndexToRetain < inputsListening.size() && inputsListening[inputDeviceIndexToRetain])
//    {
//        auto inputToRetain = inputsListening.getUnchecked(inputDeviceIndexToRetain);
//
//        for (auto input : inputsListening)
//        {
//            if (input != inputToRetain)
//            {
//                input->stop();
//                inputsListening.removeObject(input, true);
//            }
//        }
//    }
//}

void DeviceActivityMonitor::closeOutputDevices()
{
    // juce::MidiOutput stops background threads in its destructor
    outputsToPing.clear();
}

void DeviceActivityMonitor::testConnectionToDevices(int inputDeviceIndex, int outputDeviceIndex)
{
    manualDeviceInputWait = inputDeviceIndex;
    deviceConnectionMode = DetectConnectionMode::testingManualConnection;

    if (pingOutputIndex >= 0 && pingOutputIndex < outputsToPing.size() && outputsToPing[pingOutputIndex])
    {
        TerpstraSysExApplication::getApp().log("Pinging " + outputsToPing[pingOutputIndex]->getName());
        outputsToPing[outputDeviceIndex]->sendMessageNow(monitorMessage);
        startTimer(responseTimeoutMs);
    }
}

void DeviceActivityMonitor::handleIncomingMidiMessage(MidiInput* source, const MidiMessage& response)
{
    // Check if SysEx and expected manufacturer ID, board ID, and command ID
    bool isExpectedResponseType = TerpstraSysExApplication::getApp().getMidiDriver().messageIsResponseToMessage(response, monitorMessage);
    if (isExpectedResponseType)
    {
        auto data = response.getSysExData();
        
        if (isSearchingForLumatone())
        {
            MessageManagerLock mml;
            TerpstraSysExApplication::getApp().log("Received response from " + source->getName());
            TerpstraSysExApplication::getApp().log(response.getDescription());
        }

        // Is a duplicate of the request
        if (data[5] == TerpstraMidiDriver::TerpstraMIDIAnswerReturnCode::NACK)
        {
            //if (isSearchingForLumatone())
            //    TerpstraSysExApplication::getApp().log("Echo detected: ignoring virtual device");
            return;
        }

        // Is an acknowledged answer and success connection
        else
        {
            confirmedOutputIndex = pingOutputIndex;
            confirmedInputIndex = inputsListening.indexOf(source);
            expectedResponseReceived = true;
        }
    }
}

void DeviceActivityMonitor::checkDetectionStatus()
{
    // If turned off, stop detection immediately
    // For future use
    if (!detectDevicesIfDisconnected)
    {
        deviceDetectInProgress = false;
    }
    else
    {
        // Successful
        if (expectedResponseReceived)
        {
            deviceDetectInProgress = false;
            TerpstraSysExApplication::getApp().log("Connection to Lumatone confirmed.");
            sendChangeMessage();
            
            if (checkConnectionOnInactivity)
            {
                intializeConnectionLossDetection(); // TODO
            }
        }

        // Continue trying
        else if (deviceDetectInProgress)
        {
            pingNextOutput();
        }

        // Restart detection
        else
        {
            initializeDeviceDetection();
        }
    }

}

void DeviceActivityMonitor::timerCallback()
{
    stopTimer();

    if (deviceConnectionMode < DetectConnectionMode::waitingForConnectionLoss)
    {
        if (detectDevicesIfDisconnected)
        {
            if (deviceDetectInProgress)
                checkDetectionStatus();

            else
                initializeDeviceDetection();
        }
    }
    else if (deviceConnectionMode == DetectConnectionMode::waitingForConnectionLoss)
    {
        // TODO
        // Maybe look at the last message time stamp and only check if there hasn't been activity
        // Then stop if there is
    }
}

//=========================================================================
// TerpstraMidiDriver::Listener Implementation, only for connection loss detection

void DeviceActivityMonitor::midiMessageReceived(const MidiMessage& midiMessage)
{
    // TODO
}

void DeviceActivityMonitor::generalLogMessage(String textMessage, HajuErrorVisualizer::ErrorLevel errorLevel)
{
    // TODO
}
