#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Field.h"
#include "Actor.h"
#include "GameConstants.h"
#include <string>
#include <vector>
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
		m_tick = 0;
	}
	virtual int init()
	{
		//1. Initialize the data structures used to keep track of your simulation’s virtual world.
		//2. Load the current field details from the specified field data file.
		//3. Allocate and insert all anthill objects, pebble objects, baby grasshopper objects, water
		//pool objects, food objects, poison objects, etc.into the simulation world’s data structure,
		//ensuring that the locations of these objects is as specified in the loaded field data file.
		//load a field's layout using a Field.h
		Field f;
		string fieldFile = getFieldFilename();
		string error;
		if (f.loadField(fieldFile, error) != Field::LoadResult::load_success)
		{
			setError(fieldFile + " " + error);
			return false; // something bad happened!
		}
		for (int x = 0; x < VIEW_WIDTH; x++) {
			for (int y = 0; y < VIEW_HEIGHT; y++) {
				Field::FieldItem item = f.getContentsOf(x, y); // note it’s x,y and not y,x!!!
				switch (item) {
					case(Field::FieldItem::empty):
						break;
					case (Field::FieldItem::anthill0):
						break;
					case (Field::FieldItem::anthill1):
						break;
					case (Field::FieldItem::anthill2):
						break;
					case (Field::FieldItem::anthill3):
						break;
					case (Field::FieldItem::food):
						actor_map[y][x].push_back(new Food(x, y, this,6000));
						break;
					case (Field::FieldItem::grasshopper): {
						GraphObject::Direction tempdir = GraphObject::Direction(randInt(1, 4));
						actor_map[y][x].push_back(new BabyGrasshopper(x, y, tempdir, this));
						break;
					}
					case (Field::FieldItem::water):
						break;
					case (Field::FieldItem::rock):
						actor_map[y][x].push_back(new Pebble(x, y, this));
						break;
					case (Field::FieldItem::poison):
						break;
					default:
						break;
				}
			}
		}
		
		// otherwise the load was successful and you can access the
		// contents of the field 
	}
	void updateDataStructure(int i, int x, int y, int newX, int newY, Actor* obj) {
		actor_map[y][x][i] = actor_map[y][x][actor_map[y][x].size() - 1];
		actor_map[y][x].pop_back(); //remove from original
		actor_map[newY][newX].push_back(obj);
	}
	void deletefromStructure(int i, int x, int y) {
		Actor* temp = actor_map[y][x][i];
		actor_map[y][x][i] = actor_map[y][x][actor_map[y][x].size() - 1];
		actor_map[y][x].pop_back(); //remove from original
		delete temp;
	}
	void setDisplayText() {
		string s = "Ticks: "+to_string(m_tick);
		setGameStatText(s);
	}
	virtual int move()
	{
		m_tick++; // update the current tick # in the simulation
		setDisplayText();//update the display text

		for (int x = 0; x<VIEW_WIDTH;x++)
		{
			for (int y = 0; y<VIEW_HEIGHT; y++) {
				for (int i = 0; i < actor_map[y][x].size(); i++){ //iterate through the vector
					Actor *temp = (actor_map[y][x])[i];
					int oldX = temp->getX(), oldY = temp->getY();
					temp->doAction();
					if (temp->isDead())
						deletefromStructure(i, x, y);
					else
					{
						//update location in the data structure if it has changed
						int newX = temp->getX(), newY = temp->getY();
						if (newX != oldX || newY != oldY) {
							updateDataStructure(i, oldX, oldY, newX, newY, temp);
						}
					}
				}
			}
		 }


		 /*
		 // Remove newly-dead actors after each tick
		 removeDeadSimulationObjects(); // delete dead simulation objects
		 // Update the simulation Status Line
		 updateDisplayText(); // update the ticks/ant stats text at screen top
		 // If the simulation’s over (ticks == 2000) then see if we have a winner
		 if (theSimulationIsOver())
		 {
		 if (weHaveAWinningAnt())
		 {
		 setWinner(getWinningAntsName());
		 return GWSTATUS_PLAYER_WON;
		 }
		 else
		 return GWSTATUS_NO_WINNER;
		 }*/
		 // the simulation is not yet over, continue!
		 if(m_tick>=2000)
			 return GWSTATUS_NO_WINNER;
		 return GWSTATUS_CONTINUE_GAME;
	}

	virtual void cleanUp()
	{
		for (int x = 0; x < VIEW_WIDTH; x++) {
			for (int y = 0; y < VIEW_HEIGHT; y++) {
				for (int i = actor_map[y][x].size()-1; i >=0; i--) {
					delete actor_map[y][x][i];
					actor_map[y][x].pop_back();
				}
			}
		}
	}
	~StudentWorld() { //destructor destroy all the actors
		cleanUp();
	}
	//helper functions for actor.h
	bool isPebble(int x, int y) {
		if (actor_map[y][x].size() != 0 && (actor_map[y][x])[0]->getID() == IID_ROCK)
			return true;
		return false;
	}
	void addFood(int x, int y, int add) {
		for (int i = 0; i < actor_map[y][x].size(); i++) {
			if (actor_map[y][x][i]->getID() == IID_FOOD) {
				actor_map[y][x][i]->sethp(add);
				return;
			}
		}
		actor_map[y][x].push_back(new Food(x, y, this, add));
	}
	Actor* getFood(int x, int y) {
		for (int i = 0; i < actor_map[y][x].size(); i++) {
			if (actor_map[y][x][i]->getID() == IID_FOOD) {
				return actor_map[y][x][i];
			}
		}
		return nullptr;
	}
private:
	int m_tick;
	std::vector<Actor*> actor_map[64][64]; //2d array of lists which contain pointers to actors
};

#endif // STUDENTWORLD_H_
