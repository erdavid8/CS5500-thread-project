/**
 *  @file   App.hpp
 *  @brief  App class interface
 *  @author Mike and ????
 *  @date   yyyy-dd-mm
 ***********************************************/
#ifndef APP_HPP
#define APP_HPP

// Include our Third-Party SFML header
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
// Include standard library C++ libraries.
#include <queue>
#include <stack>
#include <SFML/Network/TcpSocket.hpp>
#include "Command.hpp"
#include "ClientNetwork.hpp"
// Project header files
// #include ...

// Singleton for our Application called 'App'.
class App{
private:
// Member variables
    // Stack that stores the most recently done command.
    std::stack<Command*> m_undo;	// Stack that stores the most recently undone command.
    std::stack<Command*> m_redo;
    // Main image
    sf::Image* m_image;
    // Create a sprite that we overaly
    // on top of the texture.
    sf::Sprite* m_sprite;
    // Texture sent to the GPU for rendering
    sf::Texture* m_texture;
    // Our rendering window
    sf::RenderWindow* m_window;

public:

    App() {
        m_window = nullptr;
        m_image = new sf::Image;
        m_sprite = new sf::Sprite;
        m_texture = new sf::Texture;
    }

// Member Variables
    // Keeps track of the previous mouse and current mouse positions
    // 'pmouse' is where the mouse previously was.
    // 'mouse' records where the mouse currently is.
    // Note: Occassinally I break my rules of always having a getter/setter
    //	 function for each member variable if there's reason to.
    //   	 There is some overhead associated with calling these functions,
    // 	 and even with 'inline' there is no gaureentee that the functions
    //  	 do get inlined. Other commercial and open source APIs seem
    // 	 to do this on occassion--use your disgression and expression to
    // 	 make an informed choice if exposuing these variables is safe, fast,
    // 	 and going to be more maintainable in the long run. In the end--it's
    // 	 still risky to expose anything as public.
// Member functions
    void 	ExecuteCommand(Command* c);
    void    UndoCommand();
    void    RedoCommand();

    sf::Image& GetImage();
    sf::Texture& GetTexture();
    sf::RenderWindow& GetWindow();

    void Destroy();
    void Init();
    void Update(ClientNetwork* clientNetwork);
    void DrawFunc();
    void Loop(ClientNetwork* clientNetwork);

};


#endif