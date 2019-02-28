#pragma once

#include "player.hpp"

class HumanPlayer : public Player {
public:
	virtual int getTrainIndex(const GameState& gameState);
	virtual void endGame(const GameState& gameState);
	virtual void startGame(const GameState& gameState);
};