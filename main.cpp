//Chat Client
#include "ClientNetwork.hpp"
#include "ServerNetwork.hpp"

int main(int argc, char * argv[]){

    // Stores a role of either a server or client user.
    std::string role;

    // Set the role
    std::cout << "Enter (s) for Server, Enter (c) for client: " << std::endl;
    std::cin >> role;

    // The std::cin function records the '\n' character,
    // so I instead just look at the first character of
    // the user input.
    // A 'trim' function may be more ideal.
    if(role[0] == 's') {
        ServerNetwork server_network(2525, false);
        server_network.Run();
        return 0;
    } else if(role[0] == 'c'){
        ClientNetwork client_network;
        client_network.Connect("localHost", 2525);
        client_network.Run();
        return 0;
    }
}