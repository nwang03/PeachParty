/*
Inheritance Hiearchy:

					Actor
				/			\							\
		MovableObject		Activatable Object		   Vortex
		/		\					|
	Player		Baddie			  Square

*/

#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <vector>

class StudentWorld;

class Actor : public GraphObject
{
public:
	Actor(StudentWorld* sw, int x, int y, int imageID, int dir, int depth, bool status);
	virtual void doSomething() = 0;
	bool isActive() const { return m_activeStatus; }
	void setActiveStatus(bool status) { m_activeStatus = status; }
	StudentWorld* getWorld() { return m_world; }
	virtual bool isASquare() const = 0;
	virtual void hitByVortex() = 0;
	virtual bool hasDirection() = 0;
	virtual bool canBeHitByVortex() const = 0;
	
private:
	StudentWorld* m_world;
	bool m_activeStatus;
};

class MovableObject : public Actor
{
public:
	MovableObject(StudentWorld* sw, int x, int y, int imageID, bool status);
	virtual void doSomething() = 0;
	bool isWaitingToRoll() const { return m_isWaitingtoRoll; }
	void setWaitingToRollState(bool state) { m_isWaitingtoRoll = state; }
	bool isWalking() const { return m_isWalking; }
	void setWalkingState(bool state) { m_isWalking = state; }
	int getTicksToMove() const { return m_ticksToMove; }
	void setTicksToMove(int num) { m_ticksToMove = num; }
	int getWalkDirection() const { return m_walkDirection; }
	void setWalkDirection(int dir) { m_walkDirection = dir; }
	bool hasSquareInFront(int dir);
	void teleportToRandomSquare();
	virtual bool isASquare() const { return false; }
	std::vector<int> getAvailableDirections();
	int chooseRandomDirection();
	bool checkFork();
	void cantMoveForward();
	int getDieRoll() const { return m_dieRoll; }
	void setDieRoll(int num) { m_dieRoll = num; }
	virtual void hitByVortex() { return; }
	virtual bool hasDirection() { return true; }

private:
	bool m_isWaitingtoRoll;
	bool m_isWalking;
	int m_walkDirection;
	int m_ticksToMove;
	int m_dieRoll = 0;
};

class Player : public MovableObject
{
public:
	Player(StudentWorld* sw, int x, int y, bool status, int playerNum, int imageID);
	virtual void doSomething();
	int getNumCoins() const { return m_numCoins; }
	int getNumStars() const { return m_numStars; }
	void setNumCoins(int num) { m_numCoins = num; }
	void setNumStars(int num) { m_numStars = num; }
	void swapStars(Player* otherPlayer);  // with other player
	void swapCoins(Player* otherPlayer);  // with other player
	void swapPos(Player* otherPlayer);
	int getPlayerNum() const { return m_playerNum; }
	bool directionIsValid() const { return m_directionValid; }
	void setDirectionValid(bool b) { m_directionValid = b; }
	virtual bool canBeHitByVortex() const { return false; }
	void giveVortex() { m_hasVortex = true; }
	bool hasVortex()const { return m_hasVortex; }

private:
	int m_playerNum;
	int m_numCoins;
	int m_numStars;
	bool m_directionValid = true;
	bool m_hasVortex = false;
};

class Baddie : public MovableObject
{
public:
	Baddie(StudentWorld* sw, int x, int y, bool status, int imageID);
	virtual bool canBeHitByVortex() const { return true; }
	void detectPlayer();
	void setSeenPlayer1(bool b) { m_seenPlayer1 = b; }
	void setSeenPlayer2(bool b) { m_seenPlayer2 = b; }
	void doSomething();
	virtual void reRollDice() = 0;
	void setDirectionAfterPause();
	virtual void pause() = 0;
	virtual void hitByVortex();  // tell an game object it has been hit by a vortex
	void setPauseCounter(int x) { m_pauseCounter = x; }

private:
	int m_pauseCounter;
	bool m_canAct = false;
	bool m_seenPlayer1 = false;
	bool m_seenPlayer2 = false;
	virtual void doActivity(Player* p) = 0;
};

class Bowser : public Baddie
{
public:
	Bowser(StudentWorld* sw, int x, int y, bool status, int imageID);
	virtual void reRollDice();
	virtual void pause();

private:
	virtual void doActivity(Player* p);
};

class Boo : public Baddie
{
public:
	Boo(StudentWorld* sw, int x, int y, bool status, int imageID);
	virtual void reRollDice();
	virtual void pause();

private:
	virtual void doActivity(Player* p);
};

class ActivatableObject : public Actor
{
public:
	ActivatableObject(StudentWorld* sw, int x, int y, int imageID, bool status, bool m_seenPlayer1, bool m_seenPlayer2);
	ActivatableObject(StudentWorld* sw, int x, int y, int imageID, bool status, int dir);
	void doSomething();
	void setSeenPlayer1(bool b) { m_seenPlayer1 = b; }
	void setSeenPlayer2(bool b) { m_seenPlayer2 = b; }
	virtual bool isASquare() const { return true; }
	virtual bool canBeHitByVortex() const { return false; }
	void hitByVortex() { return; }
	virtual bool hasDirection() { return false; }

private:
	bool m_seenPlayer1;
	bool m_seenPlayer2;
	virtual void doActivity(Player* p) = 0;
};

class CoinSquare : public ActivatableObject
{
public:
	CoinSquare(StudentWorld* sw, int x, int y, bool status, int numCoins, int imageID, bool seenPlayer1, bool seenPlayer2);

private:
	int m_numCoins;
	virtual void doActivity(Player* p);
};

class StarSquare : public ActivatableObject
{
public:
	StarSquare(StudentWorld* sw, int x, int y, int imageID);

private:
	virtual void doActivity(Player* p);
};

class DirectionalSquare : public ActivatableObject
{
public:
	DirectionalSquare(StudentWorld* sw, int x, int y, int imageID, int dir);
	virtual bool hasDirection() { return true; }
private:
	int m_forceDir;
	virtual void doActivity(Player* p);
};

class BankSquare : public ActivatableObject
{
public:
	BankSquare(StudentWorld* sw, int x, int y, int imageID);
private:
	virtual void doActivity(Player* p);
};

class EventSquare : public ActivatableObject
{
public:
	EventSquare(StudentWorld* sw, int x, int y, int imageID);
private:
	virtual void doActivity(Player* p);
};

class DroppingSquare : public ActivatableObject
{
public:
	DroppingSquare(StudentWorld* sw, int x, int y, int imageID);
private:
	virtual void doActivity(Player* p);
};

class Vortex : public Actor
{
public:
	Vortex(StudentWorld* sw, int x, int y, int imageID, bool status, int dir);
	virtual void doSomething();
	virtual bool isASquare() const { return false; }
	virtual void hitByVortex() { return; }
	virtual bool canBeHitByVortex() const { return false; }
	virtual bool hasDirection() { return true; }


private:
	int m_moveDir;
};

#endif //ACTOR_H