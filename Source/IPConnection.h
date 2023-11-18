/*
  ==============================================================================

    IPConnection.h
    Created: 31 Mar 2021 2:45:29pm
    Author:  corbin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
class ConnectionController;

class IPConnection : public juce::InterprocessConnection
{
public:
    IPConnection(ConnectionController& controller);
    ~IPConnection();
    
    void connectionMade() override;
    void connectionLost() override;
    void messageReceived(const juce::MemoryBlock& message) override;
private:
    ConnectionController& controller;
    void setKeepAlive();
};