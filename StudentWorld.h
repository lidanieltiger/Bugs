#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Field.h"
#include "Actor.h"
#include "GameConstants.h"
#include "Compiler.h"
#include <string>
#include <vector>
#include<sstream>
#include<iomanip>
using namespace std;
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
	 : GameWorld(assetDir)
	{
		m_tick = 0;
		m_col0score = m_col1score = m_col2score = m_col3score = 0;
	}
	virtual int init()
	{
		Compiler *compilerForEntrant0, *compilerForEntrant1,
			*compilerForEntrant2, *compilerForEntrant3;
		AntHill *ah0, *ah1, *ah2, *ah3;
		std::vector<std::string> fileNames = getFilenamesOfAntPrograms();

		compilerForEntrant0 = new Compiler;
		compilerForEntrant1 = new Compiler;
		compilerForEntrant2 = new Compiler;
		compilerForEntrant3 = new Compiler;

		std::string error;
		if (!compilerForEntrant0->compile(fileNames[0], error))
		{
			setError(fileNames[0] + " " + error);
			return GWSTATUS_LEVEL_ERROR;
		}
		if (!compilerForEntrant1->compile(fileNames[1], error))
		{
			setError(fileNames[1] + " " + error);
			return GWSTATUS_LEVEL_ERROR;
		}
		if (!compilerForEntrant2->compile(fileNames[2], error))
		{
			setError(fileNames[2] + " " + error);
			return GWSTATUS_LEVEL_ERROR;
		}
		if (!compilerForEntrant3->compile(fileNames[3], error))
		{
			setError(fileNames[3] + " " + error);
			return GWSTATUS_LEVEL_ERROR;
		}
		name0disp = fileNames[0];
		name1disp = fileNames[1];
		name2disp = fileNames[2];
		name3disp = fileNames[3];

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
						ah1 = new AntHill(x, y, this, compilerForEntrant1, 1);
						actor_map[y][x].push_back(ah1);
						break;
					case (Field::FieldItem::anthill2):
						ah2 = new AntHill(x, y, this, compilerForEntrant2, 2);
						actor_map[y][x].push_back(ah2);
						break;
					case (Field::FieldItem::anthill3):
						ah3 = new AntHill(x, y, this, compilerForEntrant3, 3);
						actor_map[y][x].push_back(ah3);
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
		std::ostringstream oss1; 
		oss1 << "Ticks: " << setw(5) << m_tick<<" - ";

		std::ostringstream oss;  // oss is a name of our choosing.
		oss.fill('0');
		oss << name0 << ": " << setw(2) << m_col0score;
		oss << "  " << name1 << ": " << setw(2) << m_col1score;
		oss << "  " << name2 << ": " << setw(2) << m_col2score;
		oss << "  " << name3 << ": " << setw(2) << m_col3score;
		string leaderboard = oss1.str()+oss.str();
		setGameStatText(leaderboard);
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

		 
		 setDisplayText();//update the display text

		 // If the simulation’s over (ticks == 2000) then see if we have a winner
		 if (m_tick>=2000)
		 {
			 if (weHaveAWinningAnt())
			 {
				 setWinner(getWinningAntsName());
				 return GWSTATUS_PLAYER_WON;
			 }else
				return GWSTATUS_NO_WINNER;
		 }else
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
	void updateScore(int colony, int score) {
		switch (colony) {
			case 0:
				m_col0score += score;
				break;
			case 1:
				m_col1score += score;
				break;
			case 2:
				m_col2score += score;
				break;
			case 3:
				m_col3score += score;
				break;
			default:
				break;
		}
		findMaxScore();
	}
	void findMaxScore() {
		int max = 0;
		if (m_col0score > max)
			max = m_col0score;
		if (m_col1score > max)
			max = m_col1score;
		if (m_col2score > max)
			max = m_col2score;
		if (m_col3score > max)
			max = m_col3score;

		if (m_col0score == max)
			name0 = name0disp + "*";
		else
			name0 = name0disp;
		if (m_col1score == max)
			name1 = name1disp + "*";
		else
			name1 = name1disp;
		if (m_col2score == max)
			name2 = name2disp + "*";
		else
			name2 = name2disp;
		if (m_col3score == max)
			name3 = name3disp + "*";
		else
			name3 = name3disp;
	}
	bool weHaveAWinningAnt() {
		if (m_col0score != 0 || m_col1score != 0 || m_col2score != 0 || m_col3score != 0)
			return true;
		return false;
	}
	string getWinningAntsName() {
		if (name0[name0.length() - 1] == '*')
			return name0disp;
		if (name1[name1.length() - 1] == '*')
			return name1disp;
		if (name2[name2.length() - 1] == '*')
			return name2disp;
		if (name3[name3.length() - 1] == '*')
			return name3disp;
		return "";
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
	void addPheromone(int x, int y, int colony) {
		for (int i = 0; i < actor_map[y][x].size(); i++) {
			Pheromone* pher = dynamic_cast<Pheromone*>(actor_map[y][x][i]);
			if (pher != nullptr&&pher->getColony()==colony) {
				pher->sethp(256);
				if (pher->gethp() > 768) { //account for overflow
					int subtract = pher->gethp() - 768;
					pher->sethp(-subtract);
				}
				return;
			}
		}
		actor_map[y][x].push_back(new Pheromone(x, y, this, colony));
	}
	Food* getFood(int x, int y) {
		for (int i = 0; i < actor_map[y][x].size(); i++) {
			Food* food = dynamic_cast<Food*>(actor_map[y][x][i]);
			if(food!=nullptr)
				return food;
		}
		return nullptr;
	}
	Pheromone* getPheromone(int x, int y, int colony) {
		for (int i = 0; i < actor_map[y][x].size(); i++) {
			Pheromone* pher = dynamic_cast<Pheromone*>(actor_map[y][x][i]);
			if (pher != nullptr&&pher->getColony()==colony)
				return pher;
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
	int m_col0score, m_col1score, m_col2score, m_col3score;
	string name0,name1,name2,name3;
	string name0disp, name1disp, name2disp, name3disp;

	std::vector<Actor*> actor_map[64][64]; //2d array of lists which contain pointers to actors
};

#endif // STUDENTWORLD_H_
