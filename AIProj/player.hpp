#ifndef PLAYER_H
#define PLAYER_H

#include "train.hpp"
#include <list>
#include "utils.hpp"
#include "gameState.hpp"

class Player {
public:
	virtual int getTrainIndex(const GameState& gameState) = 0;
	virtual void endGame(const GameState& gameState) = 0;
	virtual void startGame(const GameState& gameState) = 0;
	Piece::Color getColor() const { return color; }
	void setColor(Piece::Color color) { this->color = color; }
protected:
	Piece::Color color;
};

#endif // !PLAYER_H
