#include "StudentWorld.h"
#include "GameConstants.h"
#include "Board.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

/*
Implement a constructor for this class that initializes your data
members.
*/
StudentWorld::StudentWorld(string assetPath)
	: GameWorld(assetPath)
{
}
/*
Implement a destructor for this class that frees all dynamically
allocated objects, if any, that have not yet been freed at the time the
StudentWorld object is destroyed.
*/
StudentWorld::~StudentWorld()
{
	cleanUp();
}

Player* StudentWorld::getOtherPlayer(Player* p) const
{
	if (p->getPlayerNum() == 1)
	{
		return m_Yoshi;
	}
	else
		return m_Peach;
}
void StudentWorld::getRandomSquare(int& x, int& y) const
{
	int rand = randInt(0, m_actorVec.size());
	while (!m_actorVec.at(rand)->isASquare())
	{
		rand = randInt(0, m_actorVec.size() - 1);
	}
	x = m_actorVec.at(rand)->getX();
	y = m_actorVec.at(rand)->getY();
}

void StudentWorld::addActor(Actor* actor)
{
	m_actorVec.push_back(actor);
}

Actor* StudentWorld::getSquareAtLocation(double x, double y) const
{
	for (vector<Actor*>::size_type i = 0; i < m_actorVec.size(); i++)
	{
		if (m_actorVec.at(i)->isASquare())
		{
			if (m_actorVec.at(i)->getX() == x && m_actorVec.at(i)->getY() == y)
				return m_actorVec.at(i);
		}
	}
	return nullptr;
}

Actor* StudentWorld::getImpactableObjectAtLocation(double x, double y) const
{
	for (vector<Actor*>::size_type i = 0; i < m_actorVec.size(); i++)
	{
		if (m_actorVec.at(i)->canBeHitByVortex())
		{
			if ((m_actorVec.at(i)->getX() >= x && m_actorVec.at(i)->getX() <= x + SPRITE_WIDTH) && (m_actorVec.at(i)->getY() >= y && m_actorVec.at(i)->getY() <= y + SPRITE_HEIGHT))
				return m_actorVec.at(i);
		}
	}
	return nullptr;
}

/*
1. Initialize the data structures used to keep track of your game’s world.
2. Allocate and insert Peach and Yoshi objects into the game world. Every time a
game begins, Peach and Yoshi start out initialized in their starting location as
specified by the current board data file.
3. Allocate and insert all of the other objects (e.g., squares, baddies, etc.) into the
game world as described below.
4. Start the countdown timer for the 99-second game
*/
int StudentWorld::init()
{
	m_bd = new Board;
	string board_file = assetPath() + "board0" + std::to_string(getBoardNumber()) + ".txt";
	Board::LoadResult result = m_bd->loadBoard(board_file);
	if (result == Board::load_fail_file_not_found)
		return GWSTATUS_BOARD_ERROR;
	else if (result == Board::load_fail_bad_format)
		return GWSTATUS_BOARD_ERROR;
	else if (result == Board::load_success) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			for (int y = 0; y < BOARD_HEIGHT; y++) {
				Board::GridEntry ge = m_bd->getContentsOf(x, y);
				switch (ge) {
				case Board::player:
					m_Peach = new Player(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, 1, IID_PEACH);
					m_Yoshi = new Player(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, 2, IID_YOSHI);
					m_actorVec.push_back(new CoinSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, 3, IID_BLUE_COIN_SQUARE, true, true));
					break;
				case Board::blue_coin_square:
					m_actorVec.push_back(new CoinSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, 3, IID_BLUE_COIN_SQUARE, false, false));
					break;
				case Board::red_coin_square:
					m_actorVec.push_back(new CoinSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, -3, IID_RED_COIN_SQUARE, false, false));
					break;
				case Board::star_square:
					m_actorVec.push_back(new StarSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, IID_STAR_SQUARE));
					break;
				case Board::up_dir_square:
					m_actorVec.push_back(new DirectionalSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, IID_DIR_SQUARE, 90));
					break;
				case Board::down_dir_square:
					m_actorVec.push_back(new DirectionalSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, IID_DIR_SQUARE, 270));
					break;
				case Board::right_dir_square:
					m_actorVec.push_back(new DirectionalSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, IID_DIR_SQUARE, 0));
					break;
				case Board::left_dir_square:
					m_actorVec.push_back(new DirectionalSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, IID_DIR_SQUARE, 180));
					break;
				case Board::bank_square:
					m_actorVec.push_back(new BankSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, IID_BANK_SQUARE));
					break;
				case Board::event_square:
					m_actorVec.push_back(new EventSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, IID_EVENT_SQUARE));
					break;
				case Board::bowser:
					m_actorVec.push_back(new Bowser(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, IID_BOWSER));
					m_actorVec.push_back(new CoinSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, 3, IID_BLUE_COIN_SQUARE, false, false));
					break;
				case Board::boo:
					m_actorVec.push_back(new Boo(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, IID_BOO));
					m_actorVec.push_back(new CoinSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, 3, IID_BLUE_COIN_SQUARE, false, false));
					break;
				case Board::empty:
					break;
				default:
					m_actorVec.push_back(new CoinSquare(this, SPRITE_WIDTH * x, SPRITE_HEIGHT * y, true, 3, IID_BLUE_COIN_SQUARE, false, false));
					break;
				}
			}
		}
	}
	startCountdownTimer(99);
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	m_Peach->doSomething();
	m_Yoshi->doSomething();

	for (vector<Actor*>::iterator p = m_actorVec.begin(); p != m_actorVec.end(); p++)
	{
		if ((*p)->isActive()) {
			(*p)->doSomething();
		}
	}
	for (vector<Actor*>::iterator p = m_actorVec.begin(); p != m_actorVec.end();)
	{
		if (!(*p)->isActive()) {
			delete* p;
			*p = nullptr;
			p = m_actorVec.erase(p);
		}
		if (p != m_actorVec.end())
			p++;
	}

	ostringstream oss;
	oss << "P1 Roll: ";
	oss << setw(2) << m_Peach->getDieRoll();
	oss << " Stars:";
	oss << setw(2) << m_Peach->getNumStars();
	oss << " $$:";
	oss << setw(3) << m_Peach->getNumCoins();
	if (m_Peach->hasVortex())
		oss << " VOR";
	oss << " | Time:";
	oss << setw(3);
	oss << timeRemaining();
	oss << " | Bank:";
	oss << setw(3);
	oss << getBankCoins();
	oss << " | P2 Roll:";
	oss << setw(2) << m_Yoshi->getDieRoll();
	oss << " Stars:";
	oss << setw(2) << m_Yoshi->getNumStars();
	oss << " $$:";
	oss << setw(3) << m_Yoshi->getNumCoins();
	if (m_Yoshi->hasVortex())
		oss << " VOR";
	setGameStatText(oss.str());
	
	if (timeRemaining() <= 0) {
		playSound(SOUND_GAME_FINISHED);
		bool peachWon = false;
		if (m_Peach->getNumStars() > m_Yoshi->getNumStars())
		{
			peachWon = true;
			setFinalScore(m_Peach->getNumStars(), m_Peach->getNumCoins());
		}
		else if(m_Peach->getNumStars() < m_Yoshi->getNumStars())
		{
			peachWon = false;
			setFinalScore(m_Yoshi->getNumStars(), m_Yoshi->getNumCoins());
		}
		else {
			if (m_Peach->getNumCoins() > m_Yoshi->getNumCoins())
			{
				peachWon = true;
				setFinalScore(m_Peach->getNumStars(), m_Peach->getNumCoins());
			}
			else if (m_Peach->getNumCoins() < m_Yoshi->getNumCoins())
			{
				peachWon = false;
				setFinalScore(m_Yoshi->getNumStars(), m_Yoshi->getNumCoins());
			}
			else
			{
				int x = randInt(1, 2);
				if (x == 1)
				{
					peachWon = true;
					setFinalScore(m_Peach->getNumStars(), m_Peach->getNumCoins());
				}
				else {
					peachWon = false;
					setFinalScore(m_Yoshi->getNumStars(), m_Yoshi->getNumCoins());
				}
			}
		}
		if (peachWon)
			return GWSTATUS_PEACH_WON;
		else
			return GWSTATUS_YOSHI_WON;
	}

	// the game isn't over yet so continue playing
	return GWSTATUS_CONTINUE_GAME;
}
/*
Implement a cleanUp() method that frees any dynamically allocated
data that was allocated during calls to the init() method or the move()
method (i.e., it should delete all your allocated Coin Squares and
Peach). Note: Your StudentWorld class must have both a destructor
and the cleanUp() method even though they likely do the same thing
(in which case the destructor could just call cleanUp())
*/

void StudentWorld::cleanUp()
{
	for (vector<Actor*>::size_type i = 0; i < m_actorVec.size(); i++)
	{
		delete m_actorVec.at(i);
		m_actorVec.at(i) = nullptr;
	}
	delete m_Peach;
	m_Peach = nullptr;
	delete m_Yoshi;
	m_Yoshi = nullptr;
	delete m_bd;
	m_bd = nullptr;
}