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
			setStun(false);
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
			setStun(false);
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
			setStun(false);
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
			setStun(false);
		}
		else
			m_walkingDistance = 0;
		break;
	}
	default:
		break;
	}//end switch statement
}
void Organic::die() {
	StudentWorld *temp = getWorld();
	temp->addFood(getX(), getY(), 100);
}
bool Organic::eat(int capacity) {
	StudentWorld *temp = getWorld();
	Food* food = temp->getFood(getX(), getY());
	if (food != nullptr) {
		if (food->gethp() < capacity) {
			sethp(food->gethp());
			food->sethp(-capacity);
		}
		else {
			sethp(capacity);
			food->sethp(-capacity);
		}
		return true;
	}
	return false;
}
void Pheromone::doAction() {
	StudentWorld *temp = getWorld();
	sethp(-1); //lose health every turn
	if (isDead()) {
		return;
	}
}
void GrassHopper::GrabnGo() {
	StudentWorld *temp = getWorld();
	int move = 0;
	if(eat(200))
		move = randInt(0, 1); //50% chance of not moving if it has eaten
	if (move == 0) {
		if (getWalkingDistance() <= 0) //if the grasshopper has finished walking, then reorient itself
			reOrient();
		attemptMove(getDirection()); //try moving one unit in that direction
	}
}
void BabyGrasshopper::doSomething() {
	StudentWorld *temp = getWorld();
	if (gethp() >= 1600) {//create and add a new adult grasshopper object in the square of the baby, kill the baby, and drop food
		setDead(); //the baby will be deleted once the function returns
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
	vector<Insect*> insects = temp->getInsects(getX(), getY());
	if (insects.size() > 1) { //if there are other insects on the square
		Insect* victim = insects[randInt(0, insects.size() - 1)]; //randomly select a victim
		while (victim == this) { //make sure the grasshopper doesn't bite itself lol
			victim = insects[randInt(0, insects.size() - 1)];
		}
		victim->getBitten();
	}
}
void Trap::doAction() {
	StudentWorld *temp = getWorld();
	vector<Insect*> insects = temp->getInsects(getX(),getY());
	for (int i = 0; i < insects.size(); i++) {
		trigger(insects[i]);
	}
}
void AntHill::doSomething() {
	StudentWorld *temp = getWorld();
	Food* food = temp->getFood(getX(), getY());
	if(eat(10000))
		return;
	if (gethp() >= 2000) {
		//add an ant
		temp->updateScore(m_colony,1); //update the score
		temp->addAnt(getX(), getY(), m_colony, instructions);
		sethp(-1500);
		//increase the count of ants in the world
	}
}
void Ant::doSomething() {
	Compiler::Command cmd;
	StudentWorld *temp = getWorld();
	vector<Insect*> insects;
	Food* food;
	for (int i = 0; i < 10; i++) {
		if (instructions->getCommand(m_counter, cmd)) {
			switch (cmd.opcode) {
				case Compiler::Opcode::moveForward:
					attemptMove(getDirection()); //remember if it was blocked/remember it hasn't been bit.
					m_counter++;
					return;
					break;
				case Compiler::Opcode::eatFood:
					eat(100);
					m_counter++;
					return;
					break;
				case Compiler::Opcode::dropFood: //dump all the food
					temp->addFood(getX(), getY(), m_food);
					m_food = 0;
					m_counter++;
					return;
					break;
				case Compiler::Opcode::bite:
					insects = temp->getInsects(getX(), getY());
					if (insects.size() > 1) { //if there are other insects on the square
						Insect* victim = insects[randInt(0, insects.size() - 1)]; //randomly select a victim
						while (victim == this||getTeam()==victim->getTeam()) { //make sure the ant doesn't bite itself or friendlies
							victim = insects[randInt(0, insects.size() - 1)];
						}
						victim->getBitten();
					}
					m_counter++;
					return;
					break;
					case Compiler::pickupFood:
						food = temp->getFood(getX(), getY());
						if (food != nullptr) {
							if (food->gethp() < 400) {
								m_food += food->gethp();
								food->sethp(-400);
							}
							else {
								m_food += 400;
								food->sethp(-400);
							}
							if (m_food > 1800)
								m_food = 1800;
						}
						m_counter++;
						return;
						break;
					case Compiler::emitPheromone:
						temp->addPheromone(getX(), getY(), m_colony);
						m_counter++;
						return;
						break;
					case Compiler::faceRandomDirection:
						setDir(Direction(randInt(1, 4)));
						setDirection(getDirection());
						m_counter++;
						return;
						break;
					case Compiler::rotateClockwise:
						if (getDirection() == 4)
							setDir(Direction(1));
						else
							setDir(Direction(getDirection() + 1));
						setDirection(getDirection());
						m_counter++;
						return;
						break;
					case Compiler::rotateCounterClockwise:
						if (getDirection() == 1)
							setDir(Direction(4));
						else
							setDir(Direction(getDirection() - 1));
						setDirection(getDirection());
						m_counter++;
						return;
						break;
					case Compiler::Opcode::generateRandomNumber:
						m_randInt=randInt(0, (int)cmd.operand1[0] - 48); //store this random number
						if (cmd.operand1 == "0")
							m_randInt = 0;
						m_counter++;
						return;
						break;
					case Compiler::goto_command: //no incrementing counter, no return
						m_counter = (int)cmd.operand1[0] - 48;
						break;
					case Compiler::if_command: //no incrementing counter, no return
						processIf(cmd.operand1, cmd.operand2);
						break;

			}
		}
		else {
			setDead();
			return;
		}
			
	}
	

}
void Ant::processIf(string op1, string op2) {
	StudentWorld *temp = getWorld();
	vector<Insect*> insects;
	if (op1 == "last_random_number_was_zero") {
		if(m_randInt==0)
			m_counter = (int)op2[0] - 48;
		else
			m_counter++;
		return;
	}

	if (op1 == "I_am_carrying_food") {
		if(m_food>0)
			m_counter = (int)op2[0] - 48;
		else
			m_counter++;
		return;
	}
	if (op1 == "I_am_hungry") {
		if(gethp()<=25)
			m_counter= (int)op2[0] - 48;
		else
			m_counter++;
		return;
	}
	if (op1 == "I_am_standing_with_an_enemy") {
		insects = temp->getInsects(getX(), getY());
		for (int i = 0; i < insects.size(); i++) {
			Insect* victim = insects[i];
			if (victim->getTeam() != getTeam()) {
				m_counter = (int)op2[0] - 48;
				return;
			}
		}
		m_counter++;
		return;
	}
	if (op1 == "I_am_standing_on_food") {
		Food* food = temp->getFood(getX(), getY());
		if (food != nullptr) {
			m_counter= (int)op2[0] - 48;
		}
		else
			m_counter++;
		return;
	}
	if (op1 == "I_am_standing_on_my_anthill") {
		AntHill* temphill = temp->getAntHill(getX(), getY());
		if (temphill != nullptr&&temphill->getColony()==m_colony) {
			m_counter = (int)op2[0] - 48;
		}
		else
			m_counter++;
		return;
	}
	if (op1 == "I_smell_pheromone_in_front_of_me") {
		if(isPheromone())
			m_counter = (int)op2[0] - 48;
		else
			m_counter++;
		return;
	}
	if (op1 == "I_smell_danger_in_front_of_me") {
		if(getDanger())
			m_counter = (int)op2[0] - 48;
		else
			m_counter++;
		return;
	}
	if (op1 == "I_was_bit") {
		if (m_bitten) {
			m_counter = (int)op2[0] - 48;
		}
		else
			m_counter++;
		return;
	}
	if (op1 == "I_was_blocked_from_moving") {
		if(m_blocked)
			m_counter = (int)op2[0] - 48;
		else
			m_counter++;
		return;
	}
	m_counter++;
}
void Ant::attemptMove(Direction dir) {
	StudentWorld *temp = getWorld();
	switch (dir) {
	case up: {
		if (temp->isValidTarget(getX(), getY() - 1)) { //or not collide with rock
			moveTo(getX(), getY() - 1);
			setStun(false);
			m_bitten = false;
			m_blocked = false;
		}
		else
			m_blocked = true;
		break;
	}
	case down:
	{
		if (temp->isValidTarget(getX(), getY() + 1)) { //or not collide with rock
			moveTo(getX(), getY() + 1);
			setStun(false);
			m_bitten = false;
			m_blocked = false;
		}
		else
			m_blocked = true;
		break;
	}
	case left:
	{
		if (temp->isValidTarget(getX() - 1, getY())) { //or not collide with rock
			moveTo(getX() - 1, getY());
			setStun(false);
			m_bitten = false;
			m_blocked = false;
		}
		else
			m_blocked = true;
		break;
	}
	case right:
	{
		if (temp->isValidTarget(getX() + 1, getY())) { //or not collide with rock
			moveTo(getX() + 1, getY());
			setStun(false);
			m_bitten = false;
			m_blocked = false;
		}
		else
			m_blocked = true;
		break;
	}
	default:
		break;
	}//end switch statement
}
bool Ant::getDanger() {
	StudentWorld *temp = getWorld();
	switch (getDirection()) {
	case up: {
		if (temp->isValidTarget(getX(), getY() - 1)) { //or not collide with rock
			if (checkDanger(temp->getTraps(getX(), getY() - 1), temp->getInsects(getX(), getY() - 1)))
				return true;
			return false;
		}
		return false;
		break;
	}
	case down:
	{
		if (temp->isValidTarget(getX(), getY() + 1)) { //or not collide with rock
			if (checkDanger(temp->getTraps(getX(), getY() + 1), temp->getInsects(getX(), getY() + 1)))
				return true;
			return false;
		}
		return false;
		break;
	}
	case left:
	{
		if (temp->isValidTarget(getX() - 1, getY())) { //or not collide with rock
			if (checkDanger(temp->getTraps(getX() - 1, getY()), temp->getInsects(getX()-1, getY())))
				return true;
			return false;
		}
		return false;
		break;
	}
	case right:
	{
		if (temp->isValidTarget(getX() + 1, getY())) { //or not collide with rock
			if (checkDanger(temp->getTraps(getX() + 1, getY()), temp->getInsects(getX() + 1, getY())))
				return true;
			return false;
		}
		return false;
		break;
	}
	default:
		break;
	}//end switch statement

	return false;
}
bool Ant::isPheromone() {
	StudentWorld *temp = getWorld();
	switch (getDirection()) {
	case up: {
		if (temp->isValidTarget(getX(), getY() - 1)) { //or not collide with rock
			if (temp->getPheromone(getX(), getY() - 1, m_colony)!=nullptr)
				return true;
			return false;
		}
		return false;
		break;
	}
	case down:
	{
		if (temp->isValidTarget(getX(), getY() + 1)) { //or not collide with rock
			if (temp->getPheromone(getX(), getY() +1, m_colony) != nullptr)
				return true;
			return false;
		}
		return false;
		break;
	}
	case left:
	{
		if (temp->isValidTarget(getX() - 1, getY())) { //or not collide with rock
			if (temp->getPheromone(getX()-1, getY(), m_colony) != nullptr)
				return true;
			return false;
		}
		return false;
		break;
	}
	case right:
	{
		if (temp->isValidTarget(getX() + 1, getY())) { //or not collide with rock
			if (temp->getPheromone(getX()+1, getY(), m_colony) != nullptr)
				return true;
			return false;
		}
		return false;
		break;
	}
	default:
		break;
	}//end switch statement

	return false;
}
void Ant::die() {
	StudentWorld *temp = getWorld();
	Organic::die();
	//increment simulation
	temp->updateScore(m_colony, -1); //update the score
}
void AntHill:: die() {
	//let simulation know that this anthill is fucked
	//drop no food
}