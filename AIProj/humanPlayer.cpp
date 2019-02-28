#include "humanPlayer.hpp"
#include <iostream>

int HumanPlayer::getTrainIndex(const GameState& gameState) {
	std::cout << gameState << std::endl;

	int num;
	std::cin >> num;
	return num;
}

void HumanPlayer::startGame(const GameState & gameState) {
}

void HumanPlayer::endGame(const GameState& gameState) {
	std::cout << gameState << std::endl;

	if (gameState.getStatus() == Board::Status::WHITE_WIN) {
		std::cout << "white win!!!" << std::endl;
	}
	else if (gameState.getStatus() == Board::Status::BLACK_WIN) {
		std::cout << "black win!!!" << std::endl;
	}
	else {
		std::cout << "tie!!!" << std::endl;
	}
}
