#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

class GameObject;
class Actor;
class Player;
class Board;
class Square;

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetPath);
	virtual ~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	Board* getBoard() const { return m_bd; }
	Player* getPlayer1() const { return m_Peach; }
	Player* getPlayer2() const { return m_Yoshi; }
	int getBankCoins() const { return m_bankCoins; }
	void depositeBankCoins(int coins) { m_bankCoins += coins; }
	void resetBankCoins() { m_bankCoins = 0; }
	// Given a player object pointer, returns a pointer to the other player object. Used for swapping
	// actions.
	Player* getOtherPlayer(Player* p) const;
	void getRandomSquare(int& x, int& y) const;
	// Add a new actor to the StudentWorld (not for players, but all other game objects)
	void addActor(Actor* actor);
	// Get a pointer to the square at the specified location
	Actor* getSquareAtLocation(double x, double y) const;

	Actor* getImpactableObjectAtLocation(double x, double y) const;

private:
	std::vector<Actor*> m_actorVec;
	Player* m_Peach;
	Player* m_Yoshi;
	Board* m_bd;
	int m_bankCoins = 0;
};

#endif // STUDENTWORLD_H_