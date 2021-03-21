/*
  ==============================================================================

    DeviceActivityMonitor.h
    Created: 11 Feb 2021 9:15:43pm
    Author:  Vincenzo

    Finds a pair of devices connected to Lumatone, and monitors activity 
    to detect if the connection to the device is lost.

    Sends a change message when a change in the connection status is detected;

  ==============================================================================
*/

#pragma once

#include "TerpstraMidiDriver.h"

class DeviceActivityMonitor : public juce::Thread,
                              public juce::MidiInputCallback,
                              public TerpstraMidiDriver::Listener, 
                              public juce::ChangeBroadcaster
{
    
public:
    enum class DetectConnectionMode
    {
        noDeviceActivity = -1,
        noDeviceMonitoring,
        lookingForDevice,
        testingConnection,
        waitingForInactivity
    };
    
public:

    DeviceActivityMonitor();
    ~DeviceActivityMonitor() override;

    DetectConnectionMode getMode() const { return deviceConnectionMode; }
    bool isConnectionEstablished() const { return expectedResponseReceived && confirmedInputIndex >= 0 && confirmedOutputIndex >= 0; }
    
    bool willDetectDeviceIfDisconnected() const { return detectDevicesIfDisconnected; }
    void setDetectDeviceIfDisconnected(bool doDetection);
    
    bool willCheckForInactivity() const { return checkConnectionOnInactivity; }
    void setCheckForInactivity(bool monitorActivity);

    /// <summary>
    /// Prepares to ping devices by refreshing available devices, opening them, and starting pinging routine
    /// </summary>
    void initializeDeviceDetection();

    /// <summary>
    /// Increments the output index and sends a Get Serial Identity message to this next output to listen for a response 
    /// </summary>
    void pingNextOutput();

    ///// <summary>
    ///// Send a ping to all available outputs and prepare to listen to responses
    ///// </summary>
    //void pingAvailableDevices();

    void stopDeviceDetection();

    // Determines whether or not to try to continue device detection, tries next pair if so
    void checkDetectionStatus();

    // Begin monitoring for unresponsiveness from device
    void intializeConnectionLossDetection();

    int getConfirmedOutputIndex() const { return confirmedOutputIndex; }
    int getConfirmedInputIndex() const { return confirmedInputIndex; }

    void openAvailableOutputDevices();
    void openAvailableInputDevices();

    void closeInputDevices();
    void closeOutputDevices();

    /// <summary>
    /// Test device connectivity after a connection has previously been made.
    /// </summary>
    /// <returns>Returns false if devices are not valid, and true if it an attempt to connect was made</returns>
    bool initializeConnectionTest(DetectConnectionMode modeToUse = DetectConnectionMode::testingConnection);

    //=========================================================================
    // juce::Thread Implementation

    void run() override;

    //=========================================================================
    // juce::MidiInputCallback Implementation
  
    void handleIncomingMidiMessage(MidiInput*, const MidiMessage&) override;


protected:

    //=========================================================================
    // TerpstraMidiDriver::Listener Implementation, only for connection loss detection

    void midiMessageReceived(const MidiMessage& midiMessage) override;
    void midiMessageSent(const MidiMessage& midiMessage) override {}
    void midiSendQueueSize(int queueSize) override {}
    void generalLogMessage(String textMessage, HajuErrorVisualizer::ErrorLevel errorLevel) override {};


private:
    
    TerpstraMidiDriver*     midiDriver;

    DetectConnectionMode    deviceConnectionMode   = DetectConnectionMode::noDeviceMonitoring;
    std::atomic<bool>       deviceDetectInProgress;

    const int               responseTimeoutMs; // Pull from properties, currently 1000 by default
    const int               pingRoutineTimeoutMs = 1000;
    const int               inactivityTimeoutMs   = 5000;
    
    int                     pingOutputIndex = -1;

    OwnedArray<MidiOutput>  outputsToPing;
    OwnedArray<MidiInput>   inputsListening;

    int                     confirmedInputIndex = -1;
    int                     confirmedOutputIndex = -1;
    
    bool                    detectDevicesIfDisconnected = true;
    bool                    checkConnectionOnInactivity = true;

    MidiMessage             monitorMessage;
    std::atomic<bool>       waitingForTestResponse;
    std::atomic<bool>       expectedResponseReceived;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeviceActivityMonitor)
};
