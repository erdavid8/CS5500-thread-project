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

// All static members of a Singleton need to initialized to some value.
// This is so that when the program is compiled, memory is reserved
// for each of these static values.
// Potentially handy data structures for building an undo system.
// TODO: Decide, modify, add or remove any data structures you need.
std::stack<Command*> App::m_undo;
std::stack<Command*> App::m_redo;
// Function pointers
void (*App::m_initFunc)(void) = nullptr;
void (*App::m_updateFunc)(void) = nullptr;
void (*App::m_drawFunc)(void) = nullptr;
// Canvas variables
sf::RenderWindow* App::m_window = nullptr;

sf::Image* App::m_image = new sf::Image;
sf::Sprite* App::m_sprite = new sf::Sprite;
sf::Texture* App::m_texture = new sf::Texture;

/*! \brief	Static Method to create instance of Singleton class.
*
*/
App& App::GetInstance() {
    static App* instance = new App();
    if (instance != nullptr) {

    }
    return *instance;
}

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
    c->execute();
    m_undo.push(c);
}

/*! \brief	Removes Command from m_undo, undoes the Command,
*	  	and pushes it to top of m_redo. Does nothing if m_undo is empty.
*/
void App::UndoCommand() {
    if (m_undo.size() <= 0) {
        return;
    }

    m_undo.top()->undo();
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
    m_redo.top()->redo();
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
void App::Init(void (*initFunction)(void)){
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
    m_initFunc = initFunction;
}

/*! \brief 	Set a callback function which will be called
		each iteration of the main loop before drawing.
*
*/
void App::UpdateCallback(void (*updateFunction)(void)){
    m_updateFunc = updateFunction;
}

/*! \brief 	Set a callback function which will be called
		each iteration of the main loop after update.
*
*/
void App::DrawCallback(void (*drawFunction)(void)){
    m_drawFunc = drawFunction;
}

/*! \brief 	The main loop function which handles initialization
		and will be executed until the main window is closed.
		Within the loop function the update and draw callback
		functions will be called.
*
*/
void App::Loop(){
    // Call the init function
    m_initFunc();

    // Start the main rendering loop
    while(m_window->isOpen()){
        // Clear the window
        m_window->clear();
        // Updates specified by the user
        m_updateFunc();
        // Additional drawing specified by user
        m_drawFunc();
        // Update the texture
        // Note: This can be done in the 'draw call'
        // Draw to the canvas
        m_window->draw(*m_sprite);
        // Display the canvas
        m_window->display();
    }
}