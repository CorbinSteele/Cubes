/*
  ==============================================================================

    ConnectionController.h
    Created: 31 Mar 2021 2:59:45pm
    Author:  corbin

  ==============================================================================
*/

#pragma once
#include "CubeCalendarData.h"
#include "IPConnection.h"

class ConnectionController
{
public:
    ConnectionController(CubeCalendarData& calendarData);
    ~ConnectionController();

    bool establishConnection();
    void closeConnection();

    void connectionLost(IPConnection* connection);
    void messageReceived(IPConnection* connection, const juce::MemoryBlock& message);
    void sendMessage(const juce::MemoryBlock& message);

    void registerDataChangeCallback(std::function<void()> callback);

private:
    const juce::String REMOTE = "**.**.**.***"; // DISABLED IN PUBLIC VERSION
    static constexpr int PORT = 9502;
    static constexpr int TIMEOUT = 3000;

    CubeCalendarData& calendarData;
    juce::OwnedArray<IPConnection> connections;
    std::function<void()> onDataChange;
};
