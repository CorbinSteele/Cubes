/*
  ==============================================================================

    ConnectionController.cpp
    Created: 31 Mar 2021 2:59:45pm
    Author:  corbin

  ==============================================================================
*/

#include "ConnectionController.h"

#define TIME juce::Time::getCurrentTime().toString(true, true, true, true)
ConnectionController::ConnectionController(CubeCalendarData& calendarData) : calendarData(calendarData)
{
    establishConnection();
}
ConnectionController::~ConnectionController()
{
    connections.clear();
}

bool ConnectionController::establishConnection()
{
    return false; // DISABLED IN PUBLIC VERSION
    if (connections.size() > 0) {
        std::cout << TIME << " - ALREADY CONNECTED!" << std::endl;
        return true;
    }
    IPConnection* conn = connections.add(new IPConnection(*this));
    if (!conn->connectToSocket(REMOTE, PORT, TIMEOUT)) {
        connections.removeLast(1);
        std::cout << TIME;
        perror(" - FAILED TO FIND SERVER");
        std::cout << "RETRYING AFTER DELAY..." << std::endl;
        juce::Timer::callAfterDelay(60000, [this](){this->establishConnection();});
        return false;
    }
    else {
        std::cout << TIME << " - FOUND AND CONNECTED TO SERVER!" << std::endl;
        return true;
    }
}

void ConnectionController::closeConnection()
{
    if (connections.size() > 0) {
        std::cout << TIME << " - MANUALLY CLOSING CONNECTION..." << std::endl;
        connections[0]->disconnect(4000, juce::InterprocessConnection::Notify::no);
        connections.clear();
        std::cout << "ALL CONNECTIONS CLOSED" << std::endl;
    }
}

void ConnectionController::connectionLost(IPConnection* connection)
{
    std::cout << TIME << " - CONNECTION LOST!" << std::endl;
    connections.removeObject(connection);
    std::cout << "RECONNECTING..." << std::endl;
    establishConnection();
}
void ConnectionController::messageReceived(IPConnection* connection, const juce::MemoryBlock& message)
{
    //const std::array<int, 3>& date = calendarData.getDate();
    calendarData.setData(message);
    if (onDataChange != nullptr) {
        for (const CubeData& cube : calendarData.cubes)
            if (cube.state == -1)
                return;
        onDataChange();
        /*
        const std::array<int, 3>& newDate = calendarData.getDate();
        for (size_t i = 0; i < 3; i++) {
            std::cout << date[i] << " : " << newDate[i] << "; ";
            if (date[i] != newDate[i]) {
                //onDataChange();
                //break;
            }
        }
        std::cout << "\n";
        */
    }
}
void ConnectionController::sendMessage(const juce::MemoryBlock& message)
{
    return; // DISABLED IN PUBLIC VERSION
    if (connections.size() > 0) {
        if (!connections[0]->sendMessage(message))
            std::cout << TIME << " - DEAD CONNECTION?!?" << std::endl;
    } else {
        std::cout << TIME << " - MISSING CONNECTION?" << std::endl;
        std::cout << "CREATING NEW CONNECTION..." << std::endl;
        establishConnection();
        /*
        if (establishConnection())
            sendMessage(message);
        */
    }
}

void ConnectionController::registerDataChangeCallback(std::function<void()> callback) {
    onDataChange = callback;
}
#undef TIME
