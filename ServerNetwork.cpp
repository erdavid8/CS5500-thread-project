#include "ServerNetwork.hpp"

ServerNetwork::ServerNetwork(unsigned short port, bool rawMode = false) : listen_port(port){
    logl("SERVERNETWORK: SERVERNETWORK CONSTRUCTOR CALLED");

    logl("Chat Server Started");

     this->rawMode = rawMode;
     if(rawMode) logl("Warning, raw mode enabled");

     if(listener.listen(listen_port) != sf::Socket::Done){
          logl("Could not listen");
     }
}

void ServerNetwork::ConnectClients(std::vector<sf::TcpSocket *> * client_array){
    logl("SERVERNETWORK: CONNECTCLIENTS CALLED");

     while(true){
          sf::TcpSocket * new_client = new sf::TcpSocket();
          if(listener.accept(*new_client) == sf::Socket::Done){
               new_client->setBlocking(false);
               client_array->push_back(new_client);
               logl("Added client " << new_client->getRemoteAddress() << ":" << new_client->getRemotePort() << " on slot " << client_array->size());
          }else{
               logl("Server listener error, restart the server");
               delete(new_client);
               break;
          }
     }
}

void ServerNetwork::DisconnectClient(sf::TcpSocket * socket_pointer, size_t position){
    logl("SERVERNETWORK: DISCONNECTCLIENT CALLED");

     logl("Client " << socket_pointer->getRemoteAddress() << ":" << socket_pointer->getRemotePort() << " disconnected, removing");
     socket_pointer->disconnect();
     delete(socket_pointer);
     client_array.erase(client_array.begin() + position);
}

void ServerNetwork::BroadcastPacket(sf::Packet & packet, sf::IpAddress exclude_address, unsigned short port){
    logl("SERVERNETWORK: BROADCASTPACKET CALLED");

    for(size_t iterator = 0; iterator < client_array.size(); iterator++){
          sf::TcpSocket * client = client_array[iterator];
//          if(client->getRemoteAddress() != exclude_address || client->getRemotePort() != port){
               if(client->send(packet) != sf::Socket::Done){
                    logl("Could not send packet on broadcast");
               }
//          }
     }
}

void ServerNetwork::BroadcastRawData(const char * data, sf::IpAddress exclude_address, unsigned short port){
    logl("SERVERNETWORK: BROADCASTRAWDATA CALLED");


    for(size_t iterator = 0; iterator < client_array.size(); iterator++){
          sf::TcpSocket * client = client_array[iterator];
          if(client->getRemoteAddress() != exclude_address || client->getRemotePort() != port){
               if(client->send(data, sizeof(data)) != sf::Socket::Done){
                    logl("Could not send packet on broadcast");
               }
          }
     }
}

void ServerNetwork::ReceiveRawData(sf::TcpSocket * client, size_t iterator){
    logl("SERVERNETWORK: RECEIVERAWDATA CALLED");

    char received_data[MAX_RAW_DATA]; size_t received_bytes;
     memset(received_data, 0, sizeof(received_data));
     if(client->receive(received_data, sizeof(received_data), received_bytes) == sf::Socket::Disconnected){
          DisconnectClient(client, iterator);
     }else if(received_bytes > 0){
               BroadcastRawData(received_data, client->getRemoteAddress(), client->getRemotePort());
               logl(client->getRemoteAddress().toString() << ":" << client->getRemotePort() << " '" << received_data << "'");
     }
}

void ServerNetwork::ReceivePacket(sf::TcpSocket * client, size_t iterator){
    // Don't want to log here because this method gets called a lot
    // logl("SERVERNETWORK: RECEIVEPACKET CALLED");

    sf::Packet packet;
     if(client->receive(packet) == sf::Socket::Disconnected){
          DisconnectClient(client, iterator);
     }else{
          if(packet.getDataSize() > 0) {
              std::string typeOfData;
              packet >> typeOfData;

              if (typeOfData == "t") {
                  std::string received_message;
                  packet >> received_message;
                  packet.clear();

                  packet << typeOfData << received_message << client->getRemoteAddress().toString()
                         << client->getRemotePort();

                  BroadcastPacket(packet, client->getRemoteAddress(), client->getRemotePort());
                  logl(client->getRemoteAddress().toString() << ":" << client->getRemotePort() << " 'typeOfData: "
                                                             << "text" << " " << received_message << "'");
              } else if (typeOfData == "d") {
                  int x; int y; int prevColorInt; int currColorInt;
                  packet >> x >> y >> prevColorInt >> currColorInt;
                  packet.clear();

                  packet << typeOfData << x << y << prevColorInt << currColorInt << client->getRemoteAddress().toString()
                                                 << client->getRemotePort();

                  BroadcastPacket(packet, client->getRemoteAddress(), client->getRemotePort());
                  logl(client->getRemoteAddress().toString() << ":" << client->getRemotePort() << " 'typeOfData: "
                                                             << typeOfData << " x=" << x << " y=" << y);
              } else {
                  logl("UNEXPECTED ERROR!");
              }
          }
     }
}

void ServerNetwork::ManagePackets(){
    logl("SERVERNETWORK: MANAGEPACKETS CALLED");


    while(true){
          for(size_t iterator = 0; iterator < client_array.size(); iterator++){
               if(rawMode == true) ReceiveRawData(client_array[iterator], iterator);
               else ReceivePacket(client_array[iterator], iterator);
          }
          
          std::this_thread::sleep_for((std::chrono::milliseconds)100);
     }
}

void ServerNetwork::Run(){
    logl("SERVERNETWORK: RUN CALLED");

    std::thread connection_thread(&ServerNetwork::ConnectClients, this, &client_array);

     ManagePackets();
}