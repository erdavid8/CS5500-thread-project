/**
 *  @file   Draw.cpp
 *  @brief  Draw implementation, all drawing actions are commands.
 *  @author Mike and ????
 *  @date   yyyy-dd-mm
 ***********************************************/

// Include our Third-Party SFML header
#include <SFML/Graphics/Color.hpp>
// Include standard library C++ libraries.
#include <iostream>
// Project header files
#include "Draw.hpp"
#include "App.hpp"

/*! \brief 	Constructor for Draw, stores the X and Y coordinates of
*		the pixel that was drawn over.
*/
Draw::Draw(int newX, int newY) {
    this->mNewX = newX;
    this->mNewY = newY;
}

/*! \brief      Getter method for X coordinate that was drawn over.
*
*/
int Draw::getNewX() {
    return mNewX;
}

/*! \brief      Getter method for Y coordinate that was drawn over.
*
*/
int Draw::getNewY() {
    return mNewY;
}

/*! \brief	Runs the draw execution, which consists of drawing
*	  	the Draw's x and y values onto the image.
*/
bool Draw::execute() {
    App::GetInstance().GetImage().setPixel(mNewX, mNewY, sf::Color::Red);
    return true;
}

/*! \brief	Undoing an execute consists of redrawing the pixel with white.
*
*/
bool Draw::undo(){
    App::GetInstance().GetImage().setPixel(mNewX, mNewY, sf::Color::White);
    return true;
}

/*! \brief	Redo a draw action. Redo-ing a draw action is
* 	  	essentially the same as just executing the action.
*
*/
bool Draw::redo() {
    this->execute();
    return true;
}

/*! \brief	The equality operator is necessary because
* 	  	we want to avoid adding the same command to
* 	  	the undo stack (performance optimization).
*/
bool Draw::operator==(Command& other) {

    Draw* otherPtr = dynamic_cast<Draw*>(&other);
    return otherPtr != nullptr && this->getNewX() == otherPtr->getNewX() && this->getNewY() == otherPtr->getNewY();
}