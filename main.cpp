//Chat Client
#include "ClientNetwork.hpp"
#include "ServerNetwork.hpp"

/**
 *  @file   main.cpp
 *  @brief  Entry point into the program.
 *  @author Mike and ????
 *  @date   yyyy-dd-mm
 ***********************************************/

// Compile: Use a CMakeLists.txt to generate a build file or you can try compiling with:
// g++ -std=c++17 ./src/*.cpp -I./include/ -o App -lsfml-graphics -lsfml-window -lsfml-system
//
// Note:	If your compiler does not support -std=c++17,
//		    then try -std=c++14 then -std=c++11.
//
// HOW TO RUN
//
// ./App

// Include our Third-Party SFML header
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
// Include standard library C++ libraries.
#include <iostream>
#include <string>
// Project header files
#include "App.hpp"
#include "Draw.hpp"

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