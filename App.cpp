/**
 *  @file   App.cpp
 *  @brief  Main class for program
 *  @author Mike and ????
 *  @date   yyyy-dd-mm
 ***********************************************/

// Include our Third-Party SFML header
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
// Include standard library C++ libraries.
#include <cassert>
#include <iostream>
// Project header files
#include "App.hpp"
#include "Draw.hpp"

/*! \brief 	We should execute commands in a data structure
*		Perhaps we will have to modify the logic in our
*		loop!
*
*/
void App::ExecuteCommand(Command* c){
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
void App::UndoCommand() {
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
void App::RedoCommand() {
    if (m_redo.size() <= 0) {
        return;
    }

    sf::Image& m_image = this->GetImage();
    m_redo.top()->redo(&m_image);
    m_undo.push(m_redo.top());
    m_redo.pop();
}


/*! \brief 	Return a reference to our m_image, so that
*		we do not have to publicly expose it.
*
*/
sf::Image& App::GetImage(){
    return *m_image;
}

/*! \brief 	Return a reference to our m_Texture so that
*		we do not have to publicly expose it.
*
*/
sf::Texture& App::GetTexture(){
    return *m_texture;
}

/*! \brief 	Return a reference to our m_window so that we
*		do not have to publicly expose it.
*
*/
sf::RenderWindow& App::GetWindow(){
    return *m_window;
}

/*! \brief 	Destroy we manually call at end of our program.
*
*/
void App::Destroy(){
    delete m_image;
    delete m_sprite;
    delete m_texture;
}

/*! \brief 	Initializes the App and sets up the main
*		rendering window(i.e. our canvas.)
*/
void App::Init(){
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
void App::Update(ClientNetwork* clientNetwork){
    sf::Event event;
    while(this->GetWindow().pollEvent(event)){

    }

    // We can otherwise handle events normally
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
        sf::Vector2i coordinate = sf::Mouse::getPosition(this->GetWindow());
        // hard-coded values, its lazy but this part is not being graded :D
        if (coordinate.x >= 0 && coordinate.x <= 600 && coordinate.y >= 0 && coordinate.y <= 400) {

            std::cout << "Hmm, lots of repeats here: " << coordinate.x << "," << coordinate.y << std::endl;

            // send this packet back to the server
            clientNetwork->SendDrawThread(coordinate.x, coordinate.y);
//            std::string typeOfData = "d";
//            sf::Packet reply_packet;
//            reply_packet << typeOfData << coordinate.x << coordinate.y;
//            clientNetwork->SendPacket(reply_packet);

            // execute locally here; it might be better to instead execute if server sends something back?
            this->ExecuteCommand(new Draw(coordinate.x, coordinate.y));
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
void App::DrawFunc(){
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
void App::Loop(ClientNetwork* clientNetwork){
    // Start the main rendering loop
    while(m_window->isOpen()){
        // Clear the window
        m_window->clear();
        // Updates specified by the user
        this->Update(clientNetwork);
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