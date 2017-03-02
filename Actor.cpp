#include "Actor.h"
#include "StudentWorld.h"
#include <vector>
#include <cmath>
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
void GrassHopper::attemptMove(Direction dir) { //if the grasshopper was able to move, then walkingdistance decrements and grasshopper moves. otherwise walking distance set to zero.
	StudentWorld *temp = getWorld();
	switch (dir) {
	case up: {
		if (temp->isValidTarget(getX(),getY()-1)) { //or not collide with rock
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
		if (temp->isValidTarget(getX(), getY() + 1)) { //or not collide with rock
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
		if (temp->isValidTarget(getX()-1, getY())) { //or not collide with rock
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
		if (temp->isValidTarget(getX()+1, getY())) { //or not collide with rock
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
		m_stunned_turns = 2;
	}
	else
		m_stunned_turns--;
}
void GrassHopper::GrabnGo() {
	StudentWorld *temp = getWorld();
	Food* food = temp->getFood(getX(), getY());
	int move = 0;
	if (food != nullptr) {
		if (food->gethp() < 200) { //if the food is smaller than 200
			sethp(food->gethp());//grasshopper eats whatever food there is
			food->sethp(-200); //food dies
		}
		else {
			sethp(200); //grasshopper eats 200
			food->sethp(-200);
		}
		move = randInt(0, 1); //50% chance of not moving if it has eaten
	}
	if (move == 0) {
		if (getWalkingDistance() <= 0) //if the grasshopper has finished walking, then reorient itself
			reOrient();
		attemptMove(getDirection()); //try moving one unit in that direction
	}
}
void BabyGrasshopper::doSomething() {
	StudentWorld *temp = getWorld();
	if (gethp() >= 1600) {//create and add a new adult grasshopper object in the square of the baby, kill the baby, and drop food
		m_dead = true; //the baby will be deleted once the function returns
		temp->addFood(getX(), getY(), 100); //add food
		temp->addAdultGrasshopper(getX(), getY()); //birth the grasshopper
		return; 
	}
	//now try to eat food if there is any
	GrabnGo();
}
void AdultGrasshopper::doSomething() {
	StudentWorld *temp = getWorld();
	if (randInt(1, 3) == 1) { //one in 3 chance that the adult will bite
		bite();
		return;
	}
	if (randInt(1, 10) == 1) { //one in ten chance that it will jump
		int randR, randAngle, randY, randX;
		do{ //find an area within 10 squares without a pebble
			double PI = 3.14159265;
			randR = randInt(1, 10);
			randAngle = randInt(0, 360);
			randY = getY()+randR*sin(randAngle*PI / 180);
			randX = getX()+randR*cos(randAngle*PI / 180);
		} while (!temp->isValidTarget(randX, randY));
		moveTo(randX, randY);
		return;
	}
	//now try and eat, then try and move
	GrabnGo();
}
void AdultGrasshopper::bite() { //causes the AdultGrasshopper to bite a random other insect on the square
	StudentWorld *temp = getWorld();
	vector<Organic*> insects = temp->getInsects(getX(), getY());
	if (insects.size() > 1) { //if there are other insects on the square
		Organic* victim = insects[randInt(0, insects.size() - 1)]; //randomly select a victim
		while (victim == this) { //make sure the grasshopper doesn't bite itself lol
			victim = insects[randInt(0, insects.size() - 1)];
		}
		victim->getBitten();
	}
}
void Trap::doAction() {
	StudentWorld *temp = getWorld();
	vector<Organic*> insects = temp->getInsects(getX(),getY());
	for (int i = 0; i < insects.size(); i++) {
		trigger(insects[i]);
	}
}
void AntHill::doAction() {
	StudentWorld* temp = getWorld();
	sethp(-1);
	if (isDead())
		return;
	Food* food = temp->getFood(getX(), getY());
	if (food != nullptr) {
		if (food->gethp() < 10000) { //if the food is smaller than 10000
			sethp(food->gethp());//anthill eats whatever is there
			food->sethp(-10000); //food dies
		}
		else {
			sethp(10000); //hill eats 10000
			food->sethp(-10000);
		}
		return;
	}
	if (gethp() >= 2000) {
		//produce an ant
		sethp(-1500);
		//increment the ant counter
	}

}
void Ant::doAction() {
	StudentWorld *temp = getWorld();
	sethp(-1);
	if (isDead()) {
		temp->addFood(getX(), getY(), 100);
		return;
	}
	if (m_stunned_turns <= 0) {
		//interpCommand(m_counter);
		m_counter++;
		m_stunned_turns = 2;
	}
	else
		m_stunned_turns--;
}