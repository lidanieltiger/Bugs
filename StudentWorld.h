#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Field.h"
#include "Actor.h"
#include "GameConstants.h"
#include "Compiler.h"
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
		Compiler *compilerForEntrant0, *compilerForEntrant1,
			*compilerForEntrant2, *compilerForEntrant3;
		AntHill *ah0, *ah1, *ah2, *ah3;
		std::vector<std::string> fileNames = getFilenamesOfAntPrograms();

		compilerForEntrant0 = new Compiler;
		std::string error;

		if (!compilerForEntrant0->compile(fileNames[0], error))
		{
			setError(fileNames[0] + " " + error);
			return GWSTATUS_LEVEL_ERROR;
		}

		Field f;
		string fieldFile = getFieldFilename();
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
					{
						ah0 = new AntHill(x,y,this,compilerForEntrant0,0); 
						actor_map[y][x].push_back(ah0);
					}
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
						actor_map[y][x].push_back(new Water(x, y, this));
						break;
					case (Field::FieldItem::rock):
						actor_map[y][x].push_back(new Pebble(x, y, this));
						break;
					case (Field::FieldItem::poison):
						actor_map[y][x].push_back(new Poison(x, y, this));
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
					Organic* insect = dynamic_cast<Organic*>(temp);
					if (insect!=nullptr&&insect->isDead()) //only insects can be dead/deleted
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
	bool isValidTarget(int x, int y) { //check to see if an actor can move onto this space x,y
		//first check if it's in bounds
		if (x<1 || x>VIEW_WIDTH - 1)
			return false;
		if (y<1 || y>VIEW_HEIGHT - 1)
			return false;
		if (actor_map[y][x].size() != 0 && (actor_map[y][x])[0]->getID() == IID_ROCK) //this is okay because pebbles will always occupy the first location and nothing can collide
			return false;
		return true;
	}
	void addAdultGrasshopper(int x, int y) {
		actor_map[y][x].push_back(new AdultGrasshopper(x, y, this));
	}
	void addAnt(int x, int y, int colony, Compiler* c) {
		actor_map[y][x].push_back(new Ant(x, y, this, colony, c));
	}
	void addFood(int x, int y, int add) {
		for (int i = 0; i < actor_map[y][x].size(); i++) {
			Food* food = dynamic_cast<Food*>(actor_map[y][x][i]);
			if (food != nullptr){
				food->sethp(add);
				return;
			}
		}
		actor_map[y][x].push_back(new Food(x, y, this, add));
	}
	Food* getFood(int x, int y) {
		for (int i = 0; i < actor_map[y][x].size(); i++) {
			Food* food = dynamic_cast<Food*>(actor_map[y][x][i]);
			if(food!=nullptr)
				return food;
		}
		return nullptr;
	}
	AntHill* getAntHill(int x, int y) {
		for (int i = 0; i < actor_map[y][x].size(); i++) {
			AntHill* hill = dynamic_cast<AntHill*>(actor_map[y][x][i]);
			if (hill != nullptr)
				return hill;
		}
		return nullptr;
	}
	vector<Insect*> getInsects(int x, int y) {
		vector<Insect*> insects;
		for (int i = 0; i < actor_map[y][x].size(); i++) {
			Insect* insect = dynamic_cast<Insect*>(actor_map[y][x][i]);
			if(insect!=nullptr)
				insects.push_back(insect);
		}
		return insects;
	}
	vector<Trap*> getTraps(int x, int y) {
		vector<Trap*> traps;
		for (int i = 0; i < actor_map[y][x].size(); i++) {
			Trap* trap = dynamic_cast<Trap*>(actor_map[y][x][i]);
			if (trap != nullptr)
				traps.push_back(trap);
		}
		return traps;
	}
private:
	int m_tick;
	std::vector<Actor*> actor_map[64][64]; //2d array of lists which contain pointers to actors
};

#endif // STUDENTWORLD_H_
