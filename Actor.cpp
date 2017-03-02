#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
void GrassHopper::attemptMove(Direction dir) { //if the grasshopper was able to move, then walkingdistance decrements and grasshopper moves. otherwise walking distance set to zero.
	StudentWorld *temp = getWorld();
	switch (dir) {
	case up: {
		if (getY() - 1 >= 1 && !temp->isPebble(getX(), getY() - 1)) { //or not collide with rock
			moveTo(getX(), getY() - 1);
			m_walkingDistance--;
		}
		else
			m_walkingDistance = 0;
		break;
	}
	case down:
	{
		if (getY() + 1 < VIEW_HEIGHT - 1 && !temp->isPebble(getX(), getY() + 1)) { //or not collide with rock
			moveTo(getX(), getY() + 1);
			m_walkingDistance--;
		}
		else
			m_walkingDistance = 0;
		break;
	}
	case left:
	{
		if (getX() - 1 >= 1 && !temp->isPebble(getX() - 1, getY())) { //or not collide with rock
			moveTo(getX() - 1, getY());
			m_walkingDistance--;
		}
		else
			m_walkingDistance = 0;
		break;
	}
	case right:
	{
		if (getX() + 1 < VIEW_WIDTH - 1 && !temp->isPebble(getX() + 1, getY())) { //or not collide with rock
			moveTo(getX() + 1, getY());
			m_walkingDistance--;
		}
		else
			m_walkingDistance = 0;
		break;
	}
	default:
		break;
	}//end switch statement
}