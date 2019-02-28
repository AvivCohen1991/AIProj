#pragma once

#include "player.hpp"


class CompPlayer : public Player {
public:
	virtual int getTrainIndex(const GameState& gameState);
	virtual void endGame(const GameState& gameState);
	virtual void startGame(const GameState& gameState);
private:
	const int MINIMAX_DEPTH = 10;

	int selectTrainIndexByMinimax(const GameState& gameState, int depth);
	int selectTrainIndexByMinimax(GameState& gameState, int depth, int bestMinimaxPatentValue);
	int heuristicFunction(const GameState& gameState);
};