#pragma once

#include <vector>
#include <dynet\expr.h>
#include "gameState.hpp"


class Dataset {
public:
	const int X_SIZE = Board::BOARD_LENGTH * Board::BOARD_LENGTH;
	const int Y_SIZE = 1;

	typedef std::pair<std::vector<dynet::real>, dynet::real> Example;

	void addExample(const GameState& state, float y);
private:
	std::vector<Example> examples;
	std::vector<dynet::real> getXFromGameState(const GameState& gameState);
};