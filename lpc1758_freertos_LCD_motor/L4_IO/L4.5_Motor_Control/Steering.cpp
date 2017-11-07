/*
 * Steering.cpp
 *
 *  Created on: Oct 14, 2017
 *      Author: mehtaman
 */

#include "IO.hpp"
#include "Steering.hpp"



bool Steering::init()
{
    this->setDirection(CENTER);
    this->currentDirection=CENTER;
   // LE.on(4);
    return true;
}

int Steering::getDirection()
{
    return this->currentDirection;
}

void Steering::setDirection(int value)
{
    this->set(value);
    this->currentDirection= value;
}
