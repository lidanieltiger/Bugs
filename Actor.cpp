#include "Actor.h"
#include "StudentWorld.h"
#include <vector>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
void GrassHopper::attemptMove(Direction dir) { //if the grasshopper was able to move, then walkingdistance decrements and grasshopper moves. otherwise walking distance set to zero.
	StudentWorld *temp = getWorld();
	switch (dir) {
	case up: {
		if (getY() - 1 >= 1 && !temp->isPebble(getX(), getY() - 1)) { //or not collide with rock
			moveTo(getX(), getY() - 1);
			m_walkingDistance--;
			m_stunned = false;
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
			m_stunned = false;
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
			m_stunned = false;
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
			m_stunned = false;
		}
		else
			m_walkingDistance = 0;
		break;
	}
	default:
		break;
	}//end switch statement
}

void GrassHopper::doAction() {
	StudentWorld *temp = getWorld();
	sethp(-1); //hurt the grasshopper every turn
	if (isDead()) {
		temp->addFood(getX(), getY(), 100);
		return;
		//add 100 food to the simulation, its state has already been set to dead
	}
	if (m_stunned_turns <= 0) {
		doSomething();
	}
	else
		m_stunned_turns--;
}
void BabyGrasshopper::doSomething() {
	StudentWorld *temp = getWorld();
	if (gethp() >= 1600) {//create and add a new adult grasshopper object in the square of the baby, kill the baby, and drop food
		m_dead = true; //kill the baby
		temp->addAdultGrasshopper(getX(), getY()); //birth the grasshopper
		return; //TODO: make the grasshopper evolve
	}
	//now try to eat food if there is any
	int move = 0;
	Actor* food = temp->getFood(getX(), getY());
	if (food != nullptr) {
		if (food->gethp() < 200) { //if the food is smaller than 200
			sethp(food->gethp());//grasshopper eats whatever food there is
			food->sethp(-200); //food dies
		}
		else {
			sethp(200); //grasshopper eats 200
			food->sethp(-200);
		}
		move = randInt(0, 1);
	}
	//try to move
	if (move == 0) { //50% chance it doesn't  move 
		if (getWalkingDistance() <= 0) //if the grasshopper has finished walking, then reorient itself
			reOrient();
		attemptMove(getDirection()); //try moving one unit in that direction

		m_stunned_turns = 2; //stun the baby grasshopper again
	}
}
void AdultGrasshopper::doSomething() {

}
void Trap::doAction() {
	StudentWorld *temp = getWorld();
	vector<Actor*> insects = temp->getInsects(getX(),getY());
	for (int i = 0; i < insects.size(); i++) {
		trigger(insects[i]);
	}
}