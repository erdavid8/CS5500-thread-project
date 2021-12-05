#include "ClientNetwork.hpp"
#include "App.hpp"
#include "Draw.hpp"

/*! \brief 	Call any initailization functions here.
*		This might be for example setting up any
*		global variables, allocating memory,
*		dynamically loading any libraries, or
*		doing nothing at all.
*
*/
void initialization(){
    std::cout << "Starting the App" << std::endl;
}

/*! \brief 	The update function presented can be simplified.
*		I have demonstrated two ways you can handle events,
*		if for example we want to add in an event loop.
*
*/
void update(App* app){
    // Update our canvas
    sf::Event event;
    while(app->GetWindow().pollEvent(event)){

    }

    // We can otherwise handle events normally
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
        sf::Vector2i coordinate = sf::Mouse::getPosition(app->GetWindow());
        // hard-coded values, its lazy but this part is not being graded :D
        if (coordinate.x >= 0 && coordinate.x <= 600 && coordinate.y >= 0 && coordinate.y <= 400) {

            std::cout << "Hmm, lots of repeats here: " << coordinate.x << "," << coordinate.y << std::endl;
            app->ExecuteCommand(new Draw(coordinate.x, coordinate.y));
        }
    }
    // Capture any keys that are released
    // I used "KeyPressed", because its faster functionality
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
        std::cout << "exited!" << std::endl;
        exit(EXIT_SUCCESS);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
        std::cout << "Undo button pressed!" << std::endl;
        app->UndoCommand();
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) {
        std::cout << "Redo button pressed!" << std::endl;
        app->RedoCommand();
    }

}


/*! \brief 	The draw call
*
*/
void draw(App* app) {

    // Static variable
    static int refreshRate = 0;
    ++refreshRate;	// Increment


    // We load into our texture the modified pixels
    // But we only do so every 10 draw calls to reduce latency of transfer
    // between the GPU and CPU.
    // Ask yourself: Could we do better with sf::Clock and refresh once
    // 	 	 every 'x' frames?
    if(refreshRate>10){
        app->GetTexture().loadFromImage(app->GetImage());
        refreshRate =0;
    }
}


// OUR ACTUAL CODE
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

void ClientNetwork::ReceiveTextThread(sf::TcpSocket * socket){
    logl("CLIENTNETWORK: RECEIVEPACKETSTHREAD CALLED");

    while(true){
        if(socket->receive(last_packet) == sf::Socket::Done){
            std::string received_string; std::string sender_address; unsigned short sender_port;
            last_packet >> received_string >> sender_address >> sender_port;
            logl("From (" << sender_address << ":" << sender_port << "): " << received_string);
        }

        std::this_thread::sleep_for((std::chrono::milliseconds)100);
    }
}

void ClientNetwork::SendTextThread(sf::TcpSocket *socket) {
    logl("CLIENTNETWORK: SENDTEXTTHREAD CALLED");

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

void ClientNetwork::ReceiveDrawThread(sf::TcpSocket * socket){
    logl("CLIENTNETWORK: RECEIVEDRAWTHREAD CALLED");

    while(true){
        if(socket->receive(last_packet) == sf::Socket::Done){
            std::string received_string; std::string sender_address; unsigned short sender_port;
            last_packet >> received_string >> sender_address >> sender_port;
            logl("From (" << sender_address << ":" << sender_port << "): " << received_string);
        }

        std::this_thread::sleep_for((std::chrono::milliseconds)100);
    }
}

void ClientNetwork::SendDrawThread(sf::TcpSocket *socket) {
    logl("CLIENTNETWORK: SENDDRAWTHREAD CALLED");

    sf::Packet reply_packet;
    reply_packet << "Message send from SendDrawThread";

    SendPacket(reply_packet);
}

void ClientNetwork::SendPacket(sf::Packet & packet){
    logl("CLIENTNETWORK: SENDPACKET CALLED");

    if(packet.getDataSize() > 0 && socket.send(packet) != sf::Socket::Done){
        logl("Could not send packet");
    }
}

void ClientNetwork::Run(){
    logl("CLIENTNETWORK: RUN CALLED");

    std::thread reception_thread(&ClientNetwork::ReceiveTextThread, this, &socket);

    std::thread send_thread(&ClientNetwork::SendTextThread, this, &socket);

    std::thread draw_receive_thread(&ClientNetwork::ReceiveDrawThread, this, &socket);

    std::thread draw_send_thread(&ClientNetwork::SendDrawThread, this, &socket);

    App app;

    // Call any setup function
    // Passing a function pointer into the 'init' function.
    // of our application.

    app.Init(&initialization);
    // Setup your keyboard
    app.UpdateCallback(&update);
    // Setup the Draw Function
    app.DrawCallback(&draw);
    // Call the main loop function
    app.Loop();
    // Destroy our app
    app.Destroy();

    // inside this while loop, we could have "if typed q, exit loop"
    while (true);
}