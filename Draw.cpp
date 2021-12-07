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

/*! \brief 	Constructor for Draw, stores the X and Y coordinates of
*		the pixel that was drawn over.
*/
Draw::Draw(int newX, int newY) {
    this->mNewX = newX;
    this->mNewY = newY;
}

Draw::Draw(int newX, int newY, sf::Color prev_color,sf::Color curr_color) {
    this->mNewX = newX;
    this->mNewY = newY;
    this->prev_color = prev_color;
    this->curr_color = curr_color;
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
bool Draw::execute(sf::Image* m_image) {
    m_image->setPixel(mNewX, mNewY, curr_color);
    return true;
}

/*! \brief	Undoing an execute consists of redrawing the pixel with white.
*
*/
bool Draw::undo(sf::Image* m_image){
    m_image->setPixel(mNewX, mNewY, prev_color);
    return true;
}

/*! \brief	Redo a draw action. Redo-ing a draw action is
* 	  	essentially the same as just executing the action.
*
*/
bool Draw::redo(sf::Image* m_image) {
    this->execute(m_image);
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