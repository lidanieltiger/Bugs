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
		virtual bool canStun() { return false; } //for the water pool so it can't get repeatedly stunned

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
class Organic :public Actor
{
	public:
		Organic(int startX, int startY, StudentWorld *src, int imageID, Direction startDirection, int depth, int hp) //addition of hp to the actor class
			:Actor(imageID, startX, startY, startDirection, depth, src)
		{
			m_hp = hp;
		}
		virtual void doAction() = 0;
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
		GrassHopper(int imageID, int startX, int startY, Direction startDirection, int depth, int hp, StudentWorld *src)
			:Organic(startX, startY, src, imageID, startDirection, depth, hp)
		{
			m_walkingDistance = randInt(2, 10);
			m_direction = startDirection;
		}
		virtual void doAction() = 0;
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
};

class BabyGrasshopper : public GrassHopper
{
	public:
		BabyGrasshopper(int startX, int startY, Direction startDirection, StudentWorld *src)
			:GrassHopper(IID_BABY_GRASSHOPPER, startX, startY, startDirection, 1, 500, src)
		{
			m_stunned_turns = 0;
		}
		virtual void doAction();
	private:
		int m_stunned_turns;
};

#endif // ACTOR_H_
