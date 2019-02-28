#pragma once

#include "player.hpp"


class CompLearnerPlayer : public Player {
public:
	virtual int getTrainIndex(const GameState& gameState);
	virtual void endGame(const GameState& gameState);
	virtual void startGame(const GameState& gameState);
private:
	const int MINIMAX_DEPTH = 10;

	int selectTrainIndexByMinimaxFromWhiteViewpoint(const GameState& gameState, int depth);
	float minimaxFromWhiteViewpoint(GameState& gameState, int depth, float bestMinimaxPatentValue);
	float heuristicFunctionFromWhiteViewpoint(const GameState& gameState);	
};