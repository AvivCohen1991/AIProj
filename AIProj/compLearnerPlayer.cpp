#include "compLearnerPlayer.hpp"
#include <algorithm>
#include <vector>
#include "theModel.hpp"


int CompLearnerPlayer::getTrainIndex(const GameState& gameState) {
	return selectTrainIndexByMinimaxFromWhiteViewpoint(gameState, MINIMAX_DEPTH);
}

void CompLearnerPlayer::startGame(const GameState & gameState)
{
}

void CompLearnerPlayer::endGame(const GameState & gameState) {
	std::cout << gameState << std::endl;
}

int CompLearnerPlayer::selectTrainIndexByMinimaxFromWhiteViewpoint(const GameState & gameState, int depth) {
	//logout("-------------------------minimax start--------------------------------");

	float bestValue = (getColor() == Piece::Color::WHITE) ? -1.0f : 1.0f;
	int bestTrainIndex = 0;

	GameState localState = gameState;
	std::vector<Train> possibleTrains = localState.getPossibleTrains();

	for (unsigned int trainIndex = 0; trainIndex < possibleTrains.size(); ++trainIndex) {
		localState.makeTrain(possibleTrains[trainIndex]);

		float currentMinimaxValue = minimaxFromWhiteViewpoint(localState, depth - 1, bestValue);

		localState.undo();

		if ((getColor() == Piece::Color::WHITE && currentMinimaxValue > bestValue) ||
			(getColor() == Piece::Color::BLACK && currentMinimaxValue < bestValue)) {
			bestValue = currentMinimaxValue;
			bestTrainIndex = trainIndex;
		}
	}

	TheModel::getInstance().addExample(gameState, bestValue);

	return bestTrainIndex;
}

float CompLearnerPlayer::minimaxFromWhiteViewpoint(GameState & gameState, int depth, float bestMinimaxPatentValue) {
	//logout("minimax state:");
	//logout(state);

	float maxFloat = 1.0f;
	float minFloat = -1.0f;

	Board::Status status = gameState.getStatus();

	if (status == Board::Status::WHITE_WIN) {
		//std::cout << "minimax value: " << maxInt << "\n" << std::endl;
		return maxFloat;
	}

	if (status == Board::Status::BLACK_WIN) {
		//std::cout << "minimax value: " << minInt << "\n" << std::endl;
		return minFloat;
	}

	if (status == Board::Status::TIE) {
		//std::cout << "minimax value: " << 0 << "\n" << std::endl;
		return 0;
	}

	if (depth == 0) {
		float heuristicValue = heuristicFunctionFromWhiteViewpoint(gameState);

		//std::cout << "minimax value: " << heuristicValue << "\n" << std::endl;
		return heuristicValue;
	}

	std::vector<Train> possibleTrains = gameState.getPossibleTrains();

	if (gameState.getTurn() == Piece::Color::WHITE) {
		float maxValue = minFloat;

		for (const Train& possibleTrain : possibleTrains) {
			gameState.makeTrain(possibleTrain);

			//std::cout << std::endl;

			float currentMinimaxValue = minimaxFromWhiteViewpoint(gameState, depth - 1, maxValue);

			gameState.undo();

			if (currentMinimaxValue >= bestMinimaxPatentValue) {
				return maxFloat;
			}

			maxValue = std::max(maxValue, currentMinimaxValue);
		}

		//std::cout << "minimax value: " << maxValue << "\n" << std::endl;
		return maxValue;
	}

	else {
		float minValue = maxFloat;

		for (const Train& possibleTrain : possibleTrains) {
			gameState.makeTrain(possibleTrain);

			//std::cout << std::endl;

			float currentMinimaxValue = minimaxFromWhiteViewpoint(gameState, depth - 1, minValue);

			gameState.undo();

			if (currentMinimaxValue <= bestMinimaxPatentValue) {
				return minFloat;
			}

			minValue = std::min(minValue, currentMinimaxValue);
		}

		//std::cout << "minimax value: " << minValue << "\n" << std::endl;
		return minValue;
	}
}

float CompLearnerPlayer::heuristicFunctionFromWhiteViewpoint(const GameState & gameState) {
	return TheModel::getInstance().compute(gameState);
}