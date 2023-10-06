#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include "Board.h"
#include <vector>


//				ACTOR



Actor::Actor(StudentWorld* sw, int x, int y, int imageID, int dir, int depth, bool status)
	: GraphObject(imageID, x, y, dir, depth), m_world(sw), m_activeStatus(status)
{
}



//				MOVABLE OBJECT



MovableObject::MovableObject(StudentWorld* sw, int x, int y, int imageID, bool status)
	: Actor(sw, x, y, imageID, 0, 0, status), m_walkDirection(right), m_ticksToMove(0), m_isWaitingtoRoll(true), m_isWalking(false)
{
}

bool MovableObject::hasSquareInFront(int dir)
{
	int newX = 0, newY = 0;

	int distance = 0;

	if (dir == right || dir == left) {
		distance = SPRITE_WIDTH;
	}
	else if (dir == up || dir == down) {
		distance = SPRITE_HEIGHT;
	}

	if (dir == right || dir == up)
		getPositionInThisDirection(dir, distance, newX, newY);
	else if (dir == left || dir == down)
		getPositionInThisDirection(dir, 2, newX, newY);

	Board::GridEntry ge = getWorld()->getBoard()->getContentsOf(newX / SPRITE_WIDTH, newY / SPRITE_HEIGHT);

	if (ge == Board::empty || (getX() == 0 && dir == left) || (getY() == 0 && dir == down))
	{
		return false;
	}
	return true;
}

void MovableObject::teleportToRandomSquare()
{
	int x = 0, y = 0;
	getWorld()->getRandomSquare(x, y);
	moveTo(x, y);
}

std::vector<int> MovableObject::getAvailableDirections()
{
	std::vector<int> availableDirections;
	if (hasSquareInFront(up))
		availableDirections.push_back(up);
	if (hasSquareInFront(down))
		availableDirections.push_back(down);
	if (hasSquareInFront(right))
		availableDirections.push_back(right);
	if (hasSquareInFront(left))
		availableDirections.push_back(left);
	return availableDirections;
}

int MovableObject::chooseRandomDirection()
{
	std::vector<int> vect = getAvailableDirections();
	int index = randInt(0, vect.size() - 1);
	return vect.at(index);
}

bool MovableObject::checkFork()
{
	if (getX() % SPRITE_WIDTH == 0 && getY() % SPRITE_HEIGHT == 0) {
		if (getAvailableDirections().size() > 2) {
			return true;
		}
	}
	return false;
}

void MovableObject::cantMoveForward()
{
	if (getWalkDirection() == right || getWalkDirection() == left) {
		if (hasSquareInFront(up)) {
			setDirection(0);
			setWalkDirection(up);
		}
		else {
			setDirection(0);
			setWalkDirection(down);
		}
	}
	else {
		if (hasSquareInFront(right)) {
			setDirection(0);
			setWalkDirection(right);
		}
		else {
			setDirection(180);
			setWalkDirection(left);
		}
	}
}



//				PLAYER



Player::Player(StudentWorld* sw, int x, int y, bool status, int playerNum, int imageID)
	: MovableObject(sw, x, y, imageID, status), m_playerNum(playerNum), m_numCoins(0), m_numStars(0)
{
}

void Player::swapCoins(Player* otherPlayer)
{
	int temp = otherPlayer->getNumCoins();
	otherPlayer->setNumCoins(m_numCoins);
	m_numCoins = temp;
}

void Player::swapStars(Player* otherPlayer)
{
	int temp = otherPlayer->getNumStars();
	otherPlayer->setNumStars(m_numStars);
	m_numStars = temp;
}

void Player::swapPos(Player* otherPlayer)
{
	// make template swap function instead
	int tempX = otherPlayer->getX(), tempY = otherPlayer->getY();
	otherPlayer->moveTo(getX(), getY());
	moveTo(tempX, tempY);

	int tempInt = otherPlayer->getTicksToMove();
	otherPlayer->setTicksToMove(getTicksToMove());
	setTicksToMove(tempInt);

	bool tempBool = otherPlayer->isWaitingToRoll();
	otherPlayer->setWaitingToRollState(isWaitingToRoll());
	setWaitingToRollState(tempBool);

	tempBool = otherPlayer->isWalking();
	otherPlayer->setWalkingState(isWalking());
	setWalkingState(tempBool);

	tempInt = otherPlayer->getWalkDirection();
	otherPlayer->setWalkDirection(getWalkDirection());
	setWalkDirection(tempInt);

	tempInt = otherPlayer->getDirection();
	otherPlayer->setDirection(getDirection());
	setDirection(tempInt);
}
#include<iostream>
void Player::doSomething()
{
	bool invalidAction = false;
	if (isWaitingToRoll())
	{
		if (!m_directionValid)
		{
			int dir = chooseRandomDirection();
			setWalkDirection(dir);
			if (dir == left)
				setDirection(180);
			else {
				setDirection(0);
			}
			m_directionValid = true;
		}

		switch (getWorld()->getAction(m_playerNum))
		{
		case ACTION_ROLL:
			setDieRoll(randInt(1, 10));
			setTicksToMove(getDieRoll() * 8);
			setWalkingState(true);
			setWaitingToRollState(false);
			break;
		case ACTION_FIRE:
			if (m_hasVortex)
			{
				int newX = 0, newY = 0;
				switch (getWalkDirection())
				{
				case right:
					newX = getX() + SPRITE_WIDTH;
					newY = getY();
					break;
				case left:
					newX = getX() - SPRITE_WIDTH;
					newY = getY();
					break;
				case up:
					newX = getX();
					newY = getY() + SPRITE_HEIGHT;
					break;
				case down:
					newX = getX();
					newY = getY() - SPRITE_HEIGHT;
					break;
				}
				getWorld()->addActor(new Vortex(getWorld(), newX, newY, IID_VORTEX, true, getWalkDirection()));
				getWorld()->playSound(SOUND_PLAYER_FIRE);
			}
			m_hasVortex = false;
			break;
		default:
			return;
		}
	}
	else if (isWalking())
	{
		Actor* ptr = getWorld()->getSquareAtLocation(getX(), getY());
		if (ptr != nullptr && ptr->hasDirection())
		{
			Board::GridEntry ge = getWorld()->getBoard()->getContentsOf(getX() / SPRITE_WIDTH, getY() / SPRITE_HEIGHT);
			if (ge == Board::down_dir_square) {
				setDirection(0);
			}
			else if (ge == Board::up_dir_square) {
				setDirection(0);
			}
			else if (ge == Board::right_dir_square) {
				setDirection(0);
			}
			else if (ge == Board::left_dir_square) {
				setDirection(180);
			}
		}
		else if (checkFork()) {
			switch (getWorld()->getAction(m_playerNum))
			{
			case ACTION_LEFT:
				if (getWalkDirection() == right || !hasSquareInFront(left))
				{
					invalidAction = true;
					break;
				}
				if (hasSquareInFront(left) && getWalkDirection() != right)
				{
					invalidAction = false;
					setWalkDirection(left);
					setDirection(180);
				}
				break;
			case ACTION_RIGHT:
				if (getWalkDirection() == left || !hasSquareInFront(right))
				{
					invalidAction = true;
					break;
				}
				if (hasSquareInFront(right) && getWalkDirection() != left)
				{
					invalidAction = false;
					setWalkDirection(right);
					setDirection(0);
				}
				break;
			case ACTION_DOWN:
				if (getWalkDirection() == up || !hasSquareInFront(down))
				{
					invalidAction = true;
					break;
				}
				if (hasSquareInFront(down) && getWalkDirection() != up)
				{
					invalidAction = false;
					setWalkDirection(down);
					setDirection(0);
				}
				break;
			case ACTION_UP:
				if (getWalkDirection() == down || !hasSquareInFront(up))
				{
					invalidAction = true;
					break;
				}
				if (hasSquareInFront(up) && getWalkDirection() != down)
				{
					invalidAction = false;
					setWalkDirection(up);
					setDirection(0);
				}
				break;

			default:
				return;
			}
		}
		else if (!hasSquareInFront(getWalkDirection())) {
			cantMoveForward();
		}
		if (!invalidAction) { // if the user is at a fork but tries to move in an invalid way (backwards or no square exists), this boolean is needed so the code won't have the player continue moving
			moveAtAngle(getWalkDirection(), 2);
			setTicksToMove(getTicksToMove() - 1);
			setDieRoll(getTicksToMove() / 8);
			if (getTicksToMove() == 0) {
				setWalkingState(false);
				setWaitingToRollState(true);
			}
		}
	}
	
}



//				BADDIE




Baddie::Baddie(StudentWorld* sw, int x, int y, bool status, int imageID)
	: MovableObject(sw, x, y, imageID, status), m_pauseCounter(180)
{
}

void Baddie::detectPlayer()
{
	Player* player1 = getWorld()->getPlayer1();
	Player* player2 = getWorld()->getPlayer2();
	Player* playerToActOn{};
	bool player1Status = player1->isWaitingToRoll();
	bool player2Status = player2->isWaitingToRoll();
	int player1X, player1Y, player2X, player2Y;
	player1X = player1->getX();
	player1Y = player1->getY();
	player2X = player2->getX();
	player2Y = player2->getY();

	if (player1X == getX() && player1Y == getY())
	{
		if (player1Status && !m_seenPlayer1 && !m_seenPlayer2) {
			m_canAct = true;
			playerToActOn = player1;
		}
		m_seenPlayer1 = true;

	}

	if (player2X == getX() && player2Y == getY())
	{
		if (player2Status && !m_seenPlayer1 && !m_seenPlayer2) {
			m_canAct = true;
			playerToActOn = player2;
		}
		m_seenPlayer2 = true;
	}

	if (m_canAct)
	{
		doActivity(playerToActOn);
		m_canAct = false;
	}

	if (player1X != getX() || player1Y != getY())
		m_seenPlayer1 = false;

	if (player2X != getX() || player2Y != getY())
		m_seenPlayer2 = false;

}

void Baddie::setDirectionAfterPause()
{
	int dir = chooseRandomDirection();
	setWalkDirection(dir);
	if (dir == left)
		setDirection(180);
	else
		setDirection(0);
}

void Baddie::doSomething()
{
	if (isWaitingToRoll())
	{
		detectPlayer();
		m_pauseCounter--;
		if (m_pauseCounter == 0)
		{
			reRollDice();
			setTicksToMove(getDieRoll() * 8);
			setDirectionAfterPause();
			setWalkingState(true);
			setWaitingToRollState(false);
		}
	}
	else if (isWalking())
	{
		if (checkFork())
			setDirectionAfterPause();
		else if(!hasSquareInFront(getWalkDirection()))
		{
			cantMoveForward();
		}
		moveAtAngle(getWalkDirection(), 2);
		setTicksToMove(getTicksToMove() - 1);
		if (getTicksToMove() == 0) {
			pause();
		}
	}
}

void Baddie::hitByVortex()
{
	teleportToRandomSquare();
	setWalkDirection(right);
	setDirection(0);
	setWalkingState(false);
	setWaitingToRollState(true);
	setPauseCounter(180);
}



//					BOWSER



Bowser::Bowser(StudentWorld* sw, int x, int y, bool status, int imageID)
	: Baddie(sw, x, y, status, imageID)
{
}

void Bowser::reRollDice()
{
	setDieRoll(randInt(1, 10));
}

void Bowser::pause()
{
	setWalkingState(false);
	setWaitingToRollState(true);
	setPauseCounter(180);
	int x = randInt(1, 4);
	if (x == 1)
	{
		Actor* ptr = getWorld()->getSquareAtLocation(getX(), getY());
		if (ptr != nullptr)
		{
			ptr->setActiveStatus(false);
			getWorld()->addActor(new DroppingSquare(getWorld(), getX(), getY(), IID_DROPPING_SQUARE));
			getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
		}
	}
}

void Bowser::doActivity(Player* p)
{
	int x = randInt(1, 2);
	if (x == 1)
	{
		p->setNumCoins(0);
		p->setNumStars(0);
		getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
	}
}



//					BOO



Boo::Boo(StudentWorld* sw, int x, int y, bool status, int imageID)
	: Baddie(sw, x, y, status, imageID)
{
}

void Boo::reRollDice()
{
	setDieRoll(randInt(1, 3));
}
#include <iostream>

void Boo::pause()
{
	setWalkingState(false);
	setWaitingToRollState(true);
	setPauseCounter(180);
}

void Boo::doActivity(Player* p)
{
	int x = randInt(1, 2);
	if (x == 1)
		p->swapCoins(getWorld()->getOtherPlayer(p));
	else
		p->swapStars(getWorld()->getOtherPlayer(p));

	getWorld()->playSound(SOUND_BOO_ACTIVATE);
}



//				ACTIVATABLE OBJECT



ActivatableObject::ActivatableObject(StudentWorld* sw, int x, int y, int imageID, bool status = true, bool seenPlayer1 = false, bool seenPlayer2 = false)
	: Actor(sw, x, y, imageID, 0, 1, status), m_seenPlayer1(seenPlayer1), m_seenPlayer2(seenPlayer2)
{

}
ActivatableObject::ActivatableObject(StudentWorld* sw, int x, int y, int imageID, bool status, int dir)
	:Actor(sw, x, y, imageID, dir, 1, true)
{

}

void ActivatableObject::doSomething()
{
	if (!isActive())
		return;

	Player* player1 = getWorld()->getPlayer1();
	Player* player2 = getWorld()->getPlayer2();
	int player1X, player1Y, player2X, player2Y;
	player1X = player1->getX();
	player1Y = player1->getY();
	player2X = player2->getX();
	player2Y = player2->getY();
	if (!m_seenPlayer1)
	{
		if (player1X == getX() && player1Y == getY())
		{
			m_seenPlayer1 = true;
			doActivity(player1);
		}
	}

	if (!m_seenPlayer2)
	{
		if (player2X == getX() && player2Y == getY())
		{
			m_seenPlayer2 = true;
			doActivity(player2);
		}
	}
	// update x and y values in case of event square swapping
	player1X = player1->getX();
	player1Y = player1->getY();
	player2X = player2->getX();
	player2Y = player2->getY();

	if (player1X != getX() || player1Y != getY())
		m_seenPlayer1 = false;

	if (player2X != getX() || player2Y != getY())
		m_seenPlayer2 = false;

}



//				COIN SQUARE



CoinSquare::CoinSquare(StudentWorld* sw, int x, int y, bool status, int numCoins, int imageID, bool seenPlayer1, bool seenPlayer2)
	: ActivatableObject(sw, x, y, imageID, status, seenPlayer1, seenPlayer2), m_numCoins(numCoins)
{}

void CoinSquare::doActivity(Player* p)
{
	bool playerStatus = p->isWaitingToRoll();

	if (playerStatus)
	{
		p->setNumCoins(p->getNumCoins() + m_numCoins);
		if (m_numCoins > 0)
			getWorld()->playSound(SOUND_GIVE_COIN);
		else
			getWorld()->playSound(SOUND_TAKE_COIN);
		if (p->getNumCoins() < 0)
			p->setNumCoins(0);
	}

}



//				STAR SQUARE



StarSquare::StarSquare(StudentWorld* sw, int x, int y, int imageID)
	: ActivatableObject(sw, x, y, imageID, true, false, false)
{}

void StarSquare::doActivity(Player* p)
{
	int pCoins = p->getNumCoins();
	if (pCoins < 20)
		return;
	p->setNumCoins(pCoins - 20);
	p->setNumStars(p->getNumStars() + 1);
	getWorld()->playSound(SOUND_GIVE_STAR);
}



//				DIRECTIONAL SQUARE



DirectionalSquare::DirectionalSquare(StudentWorld* sw, int x, int y, int imageID, int dir)
	: ActivatableObject(sw, x, y, imageID, dir, true, false), m_forceDir(dir)
{
	setDirection(dir);
}

void DirectionalSquare::doActivity(Player* p)
{
	p->setWalkDirection(m_forceDir);
}



//				BANK SQUARE



BankSquare::BankSquare(StudentWorld* sw, int x, int y, int imageID)
	: ActivatableObject(sw, x, y, imageID, true, false, false)
{
}

void BankSquare::doActivity(Player* p)
{
	bool playerStatus = p->isWaitingToRoll();
	int playerCoins = p->getNumCoins();
	int bankCoins = p->getWorld()->getBankCoins();

	if (playerStatus)
	{
		p->setNumCoins(playerCoins + bankCoins);
		p->getWorld()->resetBankCoins();
		getWorld()->playSound(SOUND_WITHDRAW_BANK);
	}
	else
	{
		if (playerCoins < 5) {
			p->setNumCoins(0);
			p->getWorld()->depositeBankCoins(playerCoins);
		}
		else
		{
			p->setNumCoins(playerCoins - 5);
			p->getWorld()->depositeBankCoins(5);
		}

		getWorld()->playSound(SOUND_DEPOSIT_BANK);
	}
}



//				EVENT SQUARE



EventSquare::EventSquare(StudentWorld* sw, int x, int y, int imageID)
	: ActivatableObject(sw, x, y, imageID, true, false, false)
{
}

void EventSquare::doActivity(Player* p)
{
	bool playerStatus = p->isWaitingToRoll();
	if (playerStatus)
	{
		int option = randInt(1, 3);
		switch (option)
		{
		case 1:
			p->teleportToRandomSquare();
			p->setDirectionValid(false);
			getWorld()->playSound(SOUND_PLAYER_TELEPORT);
			break;
		case 2:
			p->swapPos(getWorld()->getOtherPlayer(p));
			getWorld()->playSound(SOUND_PLAYER_TELEPORT);
			if (getWorld()->getOtherPlayer(p)->getPlayerNum() == 1)
				setSeenPlayer1(true);
			else
				setSeenPlayer2(true);
			break;
		case 3:
			if (!p->hasVortex())
			{
				p->giveVortex();
				getWorld()->playSound(SOUND_GIVE_VORTEX);
			}
			break;
		}
	}
}



//				DROPPING SQUARE



DroppingSquare::DroppingSquare(StudentWorld* sw, int x, int y, int imageID)
	: ActivatableObject(sw, x, y, imageID, true, false, false)
{
}

void DroppingSquare::doActivity(Player* p)
{
	bool playerStatus = p->isWaitingToRoll();
	if (playerStatus)
	{
		int x = randInt(1, 2);
		if (x == 1)
		{
			int coins = p->getNumCoins();
			if (coins < 10)
				p->setNumCoins(0);
			else
				p->setNumCoins(p->getNumCoins() - 10);
		}
		else
		{
			int stars = p->getNumStars();
			if (stars >= 1)
				p->setNumStars(p->getNumStars() - 1);
		}
		getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
	}
}



//				VORTEX



Vortex::Vortex(StudentWorld* sw, int x, int y, int imageID, bool status, int dir)
	: Actor(sw, x, y, imageID, 0, 0, true), m_moveDir(dir)
{
}

void Vortex::doSomething()
{
	if (!isActive())
		return;
	moveAtAngle(m_moveDir, 2);
	if (getX() < 0 || getX() > VIEW_WIDTH || getY() < 0 || getY() > VIEW_HEIGHT)
		setActiveStatus(false);
	Actor* ptr = getWorld()->getImpactableObjectAtLocation(getX(), getY());
	if (ptr != nullptr)
	{
		ptr->hitByVortex();
		getWorld()->playSound(SOUND_HIT_BY_VORTEX);
		setActiveStatus(false);
	}
}