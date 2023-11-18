/*
  ==============================================================================

    IPConnection.cpp
    Created: 31 Mar 2021 2:45:29pm
    Author:  corbin

  ==============================================================================
*/

#include "IPConnection.h"
#include "ConnectionController.h"

#if JUCE_WINDOWS
#include <winsock2.h>
#include <mstcpip.h>
#define SET_KEEPALIVE(handle) WSAIoctl((SOCKET) handle, SIO_KEEPALIVE_VALS, &klive, sizeof(klive), nullptr, 0, reinterpret_cast<const unsigned long*>(&keep_alive), nullptr, nullptr) == 0
#elif JUCE_LINUX
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#define SET_KEEPALIVE(handle) setsockopt ((int) handle, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof (keep_alive)) == 0 &&\
                              setsockopt ((int) handle, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle)) == 0 &&\
                              setsockopt ((int) handle, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval)) == 0 &&\
                              setsockopt ((int) handle, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count)) == 0
#else
#define SET_KEEPALIVE(handle) false
#endif

void IPConnection::setKeepAlive()
{
    int keep_alive = 1;
    const int idle = 60;
    const int interval = 5;
    const int count = 10;
    #if JUCE_WINDOWS
    tcp_keepalive klive;
    klive.onoff = keep_alive;
	klive.keepalivetime = idle * 1000;
	klive.keepaliveinterval = interval * 1000;
    #endif
    if (SET_KEEPALIVE(getSocket()->getRawSocketHandle())) {
        std::cout << "keep_alive enabled" << std::endl;
    }
    else {
        perror("keep_alive failed to enable");
    }
}
#undef SET_KEEPALIVE

IPConnection::IPConnection(ConnectionController& controller) : controller(controller)
{
}

IPConnection::~IPConnection()
{
    disconnect(4000, Notify::yes);
}

void IPConnection::connectionMade()
{
    setKeepAlive();
}
void IPConnection::connectionLost()
{
    controller.connectionLost(this);
}
void IPConnection::messageReceived(const juce::MemoryBlock& message)
{
    controller.messageReceived(this, message);
}