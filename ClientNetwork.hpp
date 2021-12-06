#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <SFML/Network.hpp>

#define logl(x) std::cout << x << std::endl
#define log(x) std::cout << x

class ClientNetwork{
    sf::TcpSocket socket;
    sf::Packet last_packet;

    bool isConnected = false;
public:

    ClientNetwork();
    void Connect(const char *, unsigned short);
    void ReceiveTextOrDrawThread(sf::TcpSocket *);
    void SendTextThread();
    void ReceiveDrawThread(sf::TcpSocket *);
    void SendDrawThread(int x, int y);
    void SendPacket(sf::Packet &);
    void Run();
};