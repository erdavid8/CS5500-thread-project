#include "ClientNetwork.hpp"
#include "Draw.hpp"
#include <assert.h>
#include <map>
#include <unordered_map>

// "App" functions
/*! \brief 	We should execute commands in a data structure
*		Perhaps we will have to modify the logic in our
*		loop!
*
*/
void ClientNetwork::ExecuteCommand(Command* c){
    // if executed command is same as m_undo's commands, we do not want to change the same pixel to the same color twice in a row
    if (!m_undo.empty() && *(m_undo.top()) == *c) {
        std::cout << "Duplicate detected! Command NOT executed" << std::endl;
        return;
    }

    // once we execute a command, there are no undos to redo, so we can remove every Command in m_redo.
    while(!m_redo.empty()) {
        m_redo.pop();
    }
    sf::Image& m_image = this->GetImage();
    c->execute(&m_image);
    m_undo.push(c);
}

/*! \brief	Removes Command from m_undo, undoes the Command,
*	  	and pushes it to top of m_redo. Does nothing if m_undo is empty.
*/
void ClientNetwork::UndoCommand() {
    if (m_undo.size() <= 0) {
        return;
    }

    sf::Image& m_image = this->GetImage();
    m_undo.top()->undo(&m_image);
    m_redo.push(m_undo.top());
    m_undo.pop();
}

/*! \brief	Removes Command from m_redo, redoes the Command,
*	 	and pushes it to top of m_undo. Does nothing if m_undo is empty.
*/
void ClientNetwork::RedoCommand() {
    if (m_redo.size() <= 0) {
        return;
    }

    sf::Image& m_image = this->GetImage();
    m_redo.top()->redo(&m_image);
    m_undo.push(m_redo.top());
    m_redo.pop();
}

void ClientNetwork::clearStacks(){
    while (!m_undo.empty()){
        m_undo.pop();
    }
    while (!m_redo.empty()){
        m_redo.pop();
    }
}

/*! \brief 	Return a reference to our m_image, so that
*		we do not have to publicly expose it.
*
*/
sf::Image& ClientNetwork::GetImage(){
    return *m_image;
}

/*! \brief 	Return a reference to our m_Texture so that
*		we do not have to publicly expose it.
*
*/
sf::Texture& ClientNetwork::GetTexture(){
    return *m_texture;
}

/*! \brief 	Return a reference to our m_window so that we
*		do not have to publicly expose it.
*
*/
sf::RenderWindow& ClientNetwork::GetWindow(){
    return *m_window;
}

sf::Color ClientNetwork::getCurrColor(){
    //std::cout << currColor.toInteger() << std::endl;
    return currColor;
}
void ClientNetwork::setCurrColor(sf::Color color){
    std::cout << "Updated the current paint color" << std::endl;
    currColor = color;
}
void ClientNetwork::setImage(sf::Image *image){
    delete m_image;
    m_image = image;
    std::cout << "New image set" << std::endl;
}

/*! \brief 	Destroy we manually call at end of our program.
*
*/
void ClientNetwork::Destroy(){
    delete m_image;
    delete m_sprite;
    delete m_texture;
}

/*! \brief 	Initializes the App and sets up the main
*		rendering window(i.e. our canvas.)
*/
void ClientNetwork::Init(){
    // Create our window
    m_window = new sf::RenderWindow(sf::VideoMode(600,400),"Mini-Paint alpha 0.0.2",sf::Style::Titlebar);
    m_window->setVerticalSyncEnabled(true);
    // Create an image which stores the pixels we will update
    m_image->create(600,400,sf::Color::White);
    assert(m_image != nullptr && "m_image != nullptr");
    // Create a texture which lives in the GPU and will render our image
    m_texture->loadFromImage(*m_image);
    assert(m_texture != nullptr && "m_texture != nullptr");
    // Create a sprite which is the entity that can be textured
    m_sprite->setTexture(*m_texture);
    assert(m_sprite != nullptr && "m_sprite != nullptr");
    // Set our initialization function to perform any user
    // initialization
    std::cout << "Starting the App" << std::endl;
}

/*! \brief 	Set a callback function which will be called
		each iteration of the main loop before drawing.
*
*/
void ClientNetwork::Update(){
    sf::Event event;
    while(this->GetWindow().pollEvent(event)){

    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)){
        this->setCurrColor(sf::Color::Black);
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)){
        this->setCurrColor(sf::Color::White);
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)){
        this->setCurrColor(sf::Color::Red);
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)){
        this->setCurrColor(sf::Color::Green);
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)){
        this->setCurrColor(sf::Color::Blue);
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num6)){
        this->setCurrColor(sf::Color::Yellow);
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num7)){
        this->setCurrColor(sf::Color::Magenta);
    }
    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num8)){
        this->setCurrColor(sf::Color::Cyan);
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
        this->clearStacks();
        sf::Image* n_image = new sf::Image();
        n_image->create(600,400,this->getCurrColor());
        this->setImage(n_image);
    }

    // We can otherwise handle events normally
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
        sf::Vector2i coordinate = sf::Mouse::getPosition(this->GetWindow());
        // hard-coded values, its lazy but this part is not being graded :D
        if (coordinate.x >= 0 && coordinate.x <= 600 && coordinate.y >= 0 && coordinate.y <= 400) {

            std::cout << "Hmm, lots of repeats here: " << coordinate.x << "," << coordinate.y << std::endl;

            // send this packet back to the server
            this->SendDrawThread(coordinate.x, coordinate.y, this->GetImage().getPixel(coordinate.x, coordinate.y),this->getCurrColor());
//            std::string typeOfData = "d";
//            sf::Packet reply_packet;
//            reply_packet << typeOfData << coordinate.x << coordinate.y;
//            clientNetwork->SendPacket(reply_packet);

            // execute locally here; it might be better to instead execute if server sends something back?
//            this->ExecuteCommand(new Draw(coordinate.x, coordinate.y));
//            app->ExecuteCommand(new Draw(coordinate.x, coordinate.y,app->GetImage().getPixel(coordinate.x, coordinate.y)
//                    ,app->getCurrColor()));
        }
    }
    // Capture any keys that are released
    // I used "KeyPressed", because its faster functionality
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
        std::cout << "exited!" << std::endl;
        exit(EXIT_SUCCESS);
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
        std::cout << "Undo button pressed!" << std::endl;
        this->UndoCommand();
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)) {
        std::cout << "Redo button pressed!" << std::endl;
        this->RedoCommand();
    }
}

/*! \brief 	Set a callback function which will be called
		each iteration of the main loop after update.
*
*/
void ClientNetwork::DrawFunc(){
    // Static variable
    static int refreshRate = 0;
    ++refreshRate;	// Increment


    // We load into our texture the modified pixels
    // But we only do so every 10 draw calls to reduce latency of transfer
    // between the GPU and CPU.
    // Ask yourself: Could we do better with sf::Clock and refresh once
    // 	 	 every 'x' frames?
    if(refreshRate>10){
        this->GetTexture().loadFromImage(this->GetImage());
        refreshRate =0;
    }
}

/*! \brief 	The main loop function which handles initialization
		and will be executed until the main window is closed.
		Within the loop function the update and draw callback
		functions will be called.
*
*/
void ClientNetwork::Loop(){
    // Start the main rendering loop
    while(m_window->isOpen()){
        // Clear the window
        m_window->clear();
        // Updates specified by the user
        this->Update();
        // Additional drawing specified by user
        this->DrawFunc();
        // Update the texture
        // Note: This can be done in the 'draw call'
        // Draw to the canvas
        m_window->draw(*m_sprite);
        // Display the canvas
        m_window->display();
    }
}

// "ClientNetwork" functions
ClientNetwork::ClientNetwork(){
    logl("CLIENTNETWORK: CLIENTNETWORK CONSTRUCTOR CALLED");
    m_window = nullptr;
    m_image = new sf::Image;
    m_sprite = new sf::Sprite;
    m_texture = new sf::Texture;
    currColor = sf::Color::Black;
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

void ClientNetwork::ReceiveTextOrDrawThread(sf::TcpSocket * socket){
    logl("CLIENTNETWORK: RECEIVEPACKETSTHREAD CALLED");


    // map of int to sf::Color
    std::map<int, sf::Color> sample_map {
        {1, sf::Color::Black},
        {2, sf::Color::White},
        {3, sf::Color::Red},
        {4, sf::Color::Green},
        {5, sf::Color::Blue},
        {6, sf::Color::Yellow},
        {7, sf::Color::Magenta},
        {8, sf::Color::Cyan}
    };

    while(true){
        if(socket->receive(last_packet) == sf::Socket::Done){
            std::string typeOfData;
            last_packet >> typeOfData;

            if (typeOfData == "t") {
                std::string received_string; std::string sender_address; unsigned short sender_port;
                last_packet >> received_string >> sender_address >> sender_port;
                logl("text" << " From (" << sender_address << ":" << sender_port << "): " << received_string);
            } else if (typeOfData == "d") {
                int x; int y; int prevColorInt; int currColorInt; std::string sender_address; unsigned short sender_port;
                last_packet >> x >> y >> prevColorInt >> currColorInt >> sender_address >> sender_port;
                sf::Color prevColor = sample_map[prevColorInt];
                sf::Color currColor = sample_map[currColorInt];
                logl(typeOfData << " From (" << sender_address << ":" << sender_port << "): x=" << x << " y=" << y);
                this->ExecuteCommand(new Draw(x, y, prevColor, currColor));
            }
        }

        std::this_thread::sleep_for((std::chrono::milliseconds)100);
    }
}

void ClientNetwork::SendTextThread() {
    logl("CLIENTNETWORK: SENDTEXTTHREAD CALLED");

    while(true){
        if(isConnected){

            std::string typeOfData = "t";

            std::string user_input;
            std::getline(std::cin, user_input);

            sf::Packet reply_packet;
            reply_packet << typeOfData << user_input;

            SendPacket(reply_packet);
        }
    }
}

void ClientNetwork::SendDrawThread(int x, int y, sf::Color prevColor, sf::Color currColor) {
    logl("CLIENTNETWORK: SENDDRAWTHREAD CALLED");

    int prevColorInt;
    int currColorInt;

    // THIS BAD CODE lol
    if (prevColor == sf::Color::Black) {
        prevColorInt = 1;
    } else if (prevColor == sf::Color::White) {
        prevColorInt = 2;
    } else if (prevColor == sf::Color::Red) {
        prevColorInt = 3;
    } else if (prevColor == sf::Color::Green) {
        prevColorInt = 4;
    } else if (prevColor == sf::Color::Blue) {
        prevColorInt = 5;
    } else if (prevColor == sf::Color::Yellow) {
        prevColorInt = 6;
    } else if (prevColor == sf::Color::Magenta) {
        prevColorInt = 7;
    } else {
        prevColorInt = 8;
    }

    if (currColor == sf::Color::Black) {
        currColorInt = 1;
    } else if (currColor == sf::Color::White) {
        currColorInt = 2;
    } else if (currColor == sf::Color::Red) {
        currColorInt = 3;
    } else if (currColor == sf::Color::Green) {
        currColorInt = 4;
    } else if (currColor == sf::Color::Blue) {
        currColorInt = 5;
    } else if (currColor == sf::Color::Yellow) {
        currColorInt = 6;
    } else if (currColor == sf::Color::Magenta) {
        currColorInt = 7;
    } else {
        currColorInt = 8;
    }

    std::string typeOfData = "d";

    sf::Packet reply_packet;
    reply_packet << typeOfData << x << y << prevColorInt << currColorInt;

    SendPacket(reply_packet);
}

// helper method
void ClientNetwork::SendPacket(sf::Packet & packet){
    logl("CLIENTNETWORK: SENDPACKET CALLED");

    if(packet.getDataSize() > 0 && socket.send(packet) != sf::Socket::Done){
        logl("Could not send packet");
    }
}

void ClientNetwork::Run(){
    logl("CLIENTNETWORK: RUN CALLED");

    std::thread reception_thread(&ClientNetwork::ReceiveTextOrDrawThread, this, &socket);

    std::thread text_send_thread(&ClientNetwork::SendTextThread, this); // dont think these need to be off the main thread

    std::thread draw_send_thread(&ClientNetwork::SendDrawThread, this, 69, 69, sf::Color::White, sf::Color::White); // dont think these need to be off the main thread

    // Call any setup function
    // Passing a function pointer into the 'init' function.
    // of our application.

    this->Init();
    // Setup your keyboard
    this->Update();
    // Setup the Draw Function
    this->DrawFunc();
    // Call the main loop function
    this->Loop();
    // Destroy our app
    this->Destroy();

    // inside this while loop, we could have "if typed q, exit loop"
    while (true);
}