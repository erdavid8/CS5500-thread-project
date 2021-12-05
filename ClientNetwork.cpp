#include "ClientNetwork.hpp"

ClientNetwork::ClientNetwork(){
    logl("CLIENTNETWORK: CLIENTNETWORK CONSTRUCTOR CALLED");

    logl("Chat Client Started");
}

void ClientNetwork::Connect(const char * address, unsigned short port){
    logl("CLIENTNETWORK: CONNECT CALLED");

    if(socket.connect(address, port) != sf::Socket::Done){
        logl("Could not connect to the server\n");
    }else{
        isConnected = true;
        logl("Connected to the server\n");
    }
}

void ClientNetwork::ReceivePackets(sf::TcpSocket * socket){
    logl("CLIENTNETWORK: RECEIVEPACKETS CALLED");

    while(true){
        if(socket->receive(last_packet) == sf::Socket::Done){
            std::string received_string; std::string sender_address; unsigned short sender_port;
            last_packet >> received_string >> sender_address >> sender_port;
            logl("From (" << sender_address << ":" << sender_port << "): " << received_string);
        }

        std::this_thread::sleep_for((std::chrono::milliseconds)100);
    }
}

void ClientNetwork::SendPacket(sf::Packet & packet){
    logl("CLIENTNETWORK: SENDPACKET CALLED");

    if(packet.getDataSize() > 0 && socket.send(packet) != sf::Socket::Done){
        logl("Could not send packet");
    }
}

void ClientNetwork::Run(){
    logl("CLIENTNETWORK: RUN CALLED");

    std::thread reception_thred(&ClientNetwork::ReceivePackets, this, &socket);

    while(true){
        if(isConnected){
            std::string user_input;
            std::getline(std::cin, user_input);

            sf::Packet reply_packet;
            reply_packet << user_input;

            SendPacket(reply_packet);
        }
    }
}