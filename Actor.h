#ifndef ACTOR_H_
#define ACTOR_H_

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
			m_stunned = false;
			m_dead = false;
		}
		virtual void doAction() = 0;
		virtual void stun() {}
		virtual void poison() {}
		virtual void getBitten() = 0;
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
		bool m_stunned; //ants can be stunned
		bool m_dead;
};
class Trap :public Actor
{
public:
	Trap(int startX, int startY, StudentWorld *src, int imageID)
		:Actor(imageID, startX, startY, Direction(2), 2, src)
	{}
	void doAction();//run through the victims
	virtual void trigger(Organic* victim) = 0;
};
class Water :public Trap {
public:
	Water(int startX, int startY, StudentWorld *src)
		:Trap(startX, startY, src, IID_WATER_POOL)
	{}
	void trigger(Organic* victim) {
		victim->stun();
	}
};
class Poison :public Trap {
public:
	Poison(int startX, int startY, StudentWorld *src)
		:Trap(startX, startY, src, IID_POISON)
	{}
	void trigger(Organic* victim) {
		victim->poison();
	}
};
class Food : public Organic {
	public:
		Food(int startX, int startY, StudentWorld *src, int foodval)
			:Organic(startX, startY, src, IID_FOOD, Direction(2), 2, foodval) {
		}
		virtual void getBitten() {} //food can't get bitten
		void doAction() {} //food doesn't do anything
};
class GrassHopper : public Organic
{
	public:
		GrassHopper(int imageID, int startX, int startY, Direction startDirection, int hp, StudentWorld *src)
			:Organic(startX, startY, src, imageID, startDirection, 1, hp)
		{
			m_walkingDistance = randInt(2, 10);
			m_direction = startDirection;
			m_stunned_turns = 0;
		}
		void doAction();
		void GrabnGo(); //eats and tries to move
		virtual void doSomething() = 0;
		virtual void stun() = 0;
		virtual void poison() = 0;
		virtual void getBitten() = 0;

		int getWalkingDistance() {
			return m_walkingDistance;
		}
		Direction getDirection() {
			return m_direction;
		}
		void attemptMove(Direction dir); //if the grasshopper was able to move, then walkingdistance decrements and grasshopper moves. otherwise walking distance set to zero.
		//if attemptMove returns true, then set stunned to false because it got off the water pool

		void reOrient() { //give the grasshopper new direction and walkingdistance
			m_direction = Direction(randInt(1, 4));
			setDirection(m_direction);
			m_walkingDistance = randInt(2, 10);
		}
	private:
		int m_walkingDistance;
		Direction m_direction;
	protected:
		int m_stunned_turns;

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
		virtual void getBitten() {
			sethp(-50); //take damage
		}
		void stun() { //get stunned by water
			if (m_stunned) {
				return;
			}
			m_stunned = true;
			m_stunned_turns += 2;
		}
		void poison() {
			sethp(-150); //get wrecked by poison
		}

};

#endif // ACTOR_H_
