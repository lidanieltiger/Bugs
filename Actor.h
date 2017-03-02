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
			m_id = imageID;
			m_dead = false;
		}
		virtual void doAction() = 0;
		//functions to be overriden in organic
		virtual void sethp(int add) {}
		virtual int gethp() { return 0; }
		virtual void stun() {}
		virtual void poison() {}

		StudentWorld* getWorld() {
			return m_world;
		}
		bool isDead() {
			return m_dead;
		}
		int getID() {
			return m_id;
		}
	protected:
		bool m_dead;
	private:
		StudentWorld *m_world;
		int m_id;
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
class Trap :public Actor
{
	public:
		Trap(int startX, int startY, StudentWorld *src, int imageID)
			:Actor(imageID, startX, startY, Direction(2), 2, src)
		{}
		void doAction();//run through the victims
		virtual void trigger(Actor* victim) = 0;
};
class Water :public Trap {
	public:
		Water(int startX, int startY, StudentWorld *src)
			:Trap(startX, startY, src, IID_WATER_POOL)
		{}
		void trigger(Actor* victim) {
			victim->stun();
		}
};
class Poison :public Trap {
public:
	Poison(int startX, int startY, StudentWorld *src)
		:Trap(startX, startY, src, IID_POISON)
	{}
	void trigger(Actor* victim) {
		victim->poison();
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
		}
		virtual void doAction() = 0;
		virtual void stun() {}
		virtual void poison() {}

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
		bool m_stunned;
};
class Food : public Organic {
	public:
		Food(int startX, int startY, StudentWorld *src, int foodval)
			:Organic(startX, startY, src, IID_FOOD, Direction(2), 2, foodval) {
		}
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
		virtual void doSomething() = 0;
		virtual void stun() = 0;
		virtual void poison() = 0;

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

};
class BabyGrasshopper : public GrassHopper
{
	public:
		BabyGrasshopper(int startX, int startY, Direction startDirection, StudentWorld *src)
			:GrassHopper(IID_BABY_GRASSHOPPER, startX, startY, startDirection, 500, src)
		{
		}
		virtual void doSomething();
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
