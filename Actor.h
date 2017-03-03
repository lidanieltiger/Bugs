#ifndef ACTOR_H_
#define ACTOR_H_
#include "Compiler.h"
#include "GraphObject.h"
class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
	public:
		Actor(int imageID, int startX, int startY, Direction startDirection, int depth, StudentWorld *src)
			:GraphObject(imageID, startX, startY, startDirection, depth)
		{
			m_world = src;
		}
		virtual void doAction() = 0;

		StudentWorld* getWorld() {
			return m_world;
		}
	private:
		StudentWorld *m_world;
};
class Pebble :public Actor
{
	public:
		Pebble(int startX, int startY, StudentWorld *src, int imageID = IID_ROCK, Direction startDirection = right, int depth = 1)
			:Actor(imageID, startX, startY, startDirection, depth, src)
		{}
		void doAction() {//do nothing
		}
};
class Organic :public Actor
{
	public:
		Organic(int startX, int startY, StudentWorld *src, int imageID, Direction startDirection, int depth, int hp) //addition of hp to the actor class
			:Actor(imageID, startX, startY, startDirection, depth, src)
		{
			m_hp = hp;
			m_dead = false;
		}
		virtual void doAction() {
			sethp(-1); //lose health every turn
			if (isDead()) {
				die();
				return;
				//add 100 food to the simulation, its state has already been set to dead
			}
		}
		virtual void die();
		virtual void doSomething() {}
		bool isDead() {
			return m_dead;
		}
		void sethp(int life) {
			m_hp += life;
			if (m_hp < 0) { 
				m_dead = true;
			}
		}
		int gethp() {
			return m_hp;
		}
	private:
		int m_hp;
	protected:
		bool m_dead;
};
class Insect :public Organic {
public:
	Insect(int imageID, int startX, int startY, Direction startDirection, int hp, StudentWorld *src)
		:Organic(startX, startY, src, imageID, startDirection, 1, hp) {
		m_stunned = false;
		m_stunned_turns = 0;
		m_direction = startDirection;
	}
	virtual int getTeam() { return 5; } //return something that's on no one's team by default
	virtual void stun() { //get stunned by water
		if (m_stunned) {
			return;
		}
		m_stunned = true;
		m_stunned_turns += 2;
	}
	virtual void poison() {
		sethp(-150); //get wrecked by poison
	}
	virtual void getBitten() {
		sethp(-50);
	};
protected:
	bool m_stunned; //ants can be stunned
	int m_stunned_turns;
	Direction m_direction;
};
class Trap :public Actor
{
public:
	Trap(int startX, int startY, StudentWorld *src, int imageID)
		:Actor(imageID, startX, startY, Direction(2), 2, src)
	{}
	void doAction();//run through the victims
	virtual void trigger(Insect* victim) = 0;
};
class Water :public Trap {
public:
	Water(int startX, int startY, StudentWorld *src)
		:Trap(startX, startY, src, IID_WATER_POOL)
	{}
	void trigger(Insect* victim) {
		victim->stun();
	}
};
class Poison :public Trap {
public:
	Poison(int startX, int startY, StudentWorld *src)
		:Trap(startX, startY, src, IID_POISON)
	{}
	void trigger(Insect* victim) {
		victim->poison();
	}
};
class Pheromone :public Organic {
	public:
		Pheromone(int startX, int startY, StudentWorld *src, int pheromonetype)
			:Organic(startX, startY, src, IID_PHEROMONE_TYPE0+pheromonetype, Direction(2), 2, 256) {
			m_colony = pheromonetype;
		}
		void doAction(); //override doaction so that it doesn't drop food on death
		int getColony() {
			return m_colony;
		}
	private:
		int m_colony;
};
class AntHill : public Organic {
	public:
		AntHill(int startX, int startY, StudentWorld *src, Compiler* c, int colonyID) :
			Organic(startX, startY, src, IID_ANT_HILL, Direction(2), 2, 8999) 
		{
			instructions = c;
			m_colony = colonyID;
		}
		int getColony() {
			return m_colony;
		}
		void doAction() {
			Organic::doAction();
			doSomething();
		}
		void doSomething();
		virtual void die(); //drops nothing when dead
	private: 
		Compiler* instructions;
		int m_colony;

};
class Ant : public Insect {
	public:
		Ant(int startX, int startY, StudentWorld *src, int antType, Compiler *c) :
			Insect(antType, startX, startY, Direction(randInt(1, 4)), 1500, src) {
			instructions = c;
			m_colony = antType;
			m_counter = 0;
			m_food = 0;
			m_randInt = 0;
			m_bitten = false;
			m_blocked = false;
		}
		void doAction() {
			Organic::doAction();
			if (m_stunned_turns > 0) {
				m_stunned_turns--;
			}
			else 
				doSomething();
		}
		void doSomething();
		void attemptMove(Direction dir);
		bool getDanger();
		bool isPheromone();
		bool checkDanger(std::vector<Trap*> traps, std::vector<Insect*> insects) {
			if (traps.size() > 0)
				return true;
			for (int i = 0; i < insects.size(); i++) {
				if (insects[i]->getTeam() != m_colony)
					return true;
			}
			return false;
		}
		void processIf(std::string op1, std::string op2);
		int getTeam() {
			return m_colony;
		}
		void getBitten() {
			sethp(-50);
			m_bitten = true;
		};
		virtual void die();
	private:
		int m_counter;
		int m_colony;
		Compiler* instructions;
		int m_food;
		int m_randInt;
		bool m_bitten;
		bool m_blocked;
};
class Food : public Organic {
	public:
		Food(int startX, int startY, StudentWorld *src, int foodval)
			:Organic(startX, startY, src, IID_FOOD, Direction(2), 2, foodval) {
		}
		void doAction() {} //food doesn't do anything
};
class GrassHopper : public Insect
{
	public:
		GrassHopper(int imageID, int startX, int startY, Direction startDirection, int hp, StudentWorld *src)
			:Insect(imageID, startX, startY, startDirection, hp, src)
		{
			m_walkingDistance = randInt(2, 10);
		}
		void GrabnGo(); //eats and tries to move
		virtual void doSomething() = 0;
		virtual void stun() { Insect::stun(); }
		virtual void poison() { Insect::poison(); }
		virtual void getBitten() { Insect::getBitten(); }
		virtual void doAction() {
			Organic::doAction();
			if (m_stunned_turns <= 0) {
				doSomething();
				m_stunned_turns = 2;
			}
			else
				m_stunned_turns--;
		}
		int getWalkingDistance() {
			return m_walkingDistance;
		}
		void attemptMove(Direction dir); //if the grasshopper was able to move, then walkingdistance decrements and grasshopper moves. otherwise walking distance set to zero.

		void reOrient() { //give the grasshopper new direction and walkingdistance
			m_direction = Direction(randInt(1, 4));
			setDirection(m_direction);
			m_walkingDistance = randInt(2, 10);
		}
	private:
		int m_walkingDistance;

};
class AdultGrasshopper : public GrassHopper
{
	public:
		AdultGrasshopper(int startX, int startY, StudentWorld *src)
			:GrassHopper(IID_ADULT_GRASSHOPPER, startX, startY, Direction(randInt(1,4)), 1600, src)
		{
			reOrient();
		}
		virtual void doSomething();
		void stun() {}//unfazed by water
		void poison() {}//unfazed by poison
		void bite(); //bite a random insect on the same square
		virtual void getBitten() {
			if(randInt(0,1)==0) //50% chance for retaliation
				bite();
			sethp(-50); //take damage
		}

};
class BabyGrasshopper : public GrassHopper
{
	public:
		BabyGrasshopper(int startX, int startY, Direction startDirection, StudentWorld *src)
			:GrassHopper(IID_BABY_GRASSHOPPER, startX, startY, startDirection, 500, src)
		{
		}
		virtual void doSomething();
};

#endif // ACTOR_H_
