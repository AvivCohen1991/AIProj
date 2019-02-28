#include "compPlayer.hpp"
#include <algorithm>
#include <vector>
#include <filesystem>



int CompPlayer::getTrainIndex(const GameState& gameState) {
	return selectTrainIndexByMinimax(gameState, MINIMAX_DEPTH);
}

void CompPlayer::startGame(const GameState & gameState)
{
}

void CompPlayer::endGame(const GameState & gameState)
{
	std::cout << gameState << std::endl;
}

int CompPlayer::selectTrainIndexByMinimax(const GameState& gameState, int depth) {
	//logout("-------------------------minimax start--------------------------------");

	int maxValue = std::numeric_limits<int>::min();
	int maxTrainIndex = 0;

	GameState localState = gameState;
	std::vector<Train> possibleTrains = localState.getPossibleTrains();

	for (unsigned int trainIndex = 0; trainIndex < possibleTrains.size(); ++trainIndex) {
		localState.makeTrain(possibleTrains[trainIndex]);

		int currentMinimaxValue = selectTrainIndexByMinimax(localState, depth - 1, maxValue);

		localState.undo();

		if (currentMinimaxValue > maxValue) {
			maxValue = currentMinimaxValue;
			maxTrainIndex = trainIndex;
		}
	}

	return maxTrainIndex;
}

int CompPlayer::selectTrainIndexByMinimax(GameState & gameState, int depth, int bestMinimaxPatentValue) {
	//logout("minimax state:");
	//logout(state);

	int maxInt = std::numeric_limits<int>::max();
	int minInt = std::numeric_limits<int>::min();

	Board::Status status = gameState.getStatus();

	if (getColor() == Piece::Color::WHITE && status == Board::Status::WHITE_WIN ||
		getColor() == Piece::Color::BLACK && status == Board::Status::BLACK_WIN) {
		//std::cout << "minimax value: " << maxInt << "\n" << std::endl;
		return maxInt;
	}

	if (getColor() == Piece::Color::WHITE && status == Board::Status::BLACK_WIN ||
		getColor() == Piece::Color::BLACK && status == Board::Status::WHITE_WIN) {
		//std::cout << "minimax value: " << minInt << "\n" << std::endl;
		return minInt;
	}

	if (status == Board::Status::TIE) {
		//std::cout << "minimax value: " << 0 << "\n" << std::endl;
		return 0;
	}

	if (depth == 0) {
		int heuristicValue = heuristicFunction(gameState);
		//std::cout << "minimax value: " << heuristicValue << "\n" << std::endl;
		return heuristicValue;
	}

	std::vector<Train> possibleTrains = gameState.getPossibleTrains();

	if (gameState.getTurn() == getColor()) {
		int maxValue = minInt;

		for (const Train& possibleTrain : possibleTrains) {
			gameState.makeTrain(possibleTrain);

			//std::cout << std::endl;

			int currentMinimaxValue = selectTrainIndexByMinimax(gameState, depth - 1, maxValue);

			gameState.undo();

			if (currentMinimaxValue >= bestMinimaxPatentValue) {
				return maxInt;
			}

			maxValue = std::max(maxValue, currentMinimaxValue);
		}

		//std::cout << "minimax value: " << maxValue << "\n" << std::endl;
		return maxValue;
	}

	else {
		int minValue = maxInt;

		for (const Train& possibleTrain : possibleTrains) {
			gameState.makeTrain(possibleTrain);

			//std::cout << std::endl;

			int currentMinimaxValue = selectTrainIndexByMinimax(gameState, depth - 1, minValue);

			gameState.undo();

			if (currentMinimaxValue <= bestMinimaxPatentValue) {
				return minInt;
			}

			minValue = std::min(minValue, currentMinimaxValue);
		}

		//std::cout << "minimax value: " << minValue << "\n" << std::endl;
		return minValue;
	}
}

int CompPlayer::heuristicFunction(const GameState & gameState) {
	const Board& board = gameState.getBoard();

	int res = 0;

	const std::list<std::pair<int, int>>& whitePawnsPlaces = board.getPiecesPlaces(Piece::Id::WHITE_PAWN);
	const std::list<std::pair<int, int>>& blackPawnsPlaces = board.getPiecesPlaces(Piece::Id::BLACK_PAWN);
	const std::list<std::pair<int, int>>& whiteKingsPlaces = board.getPiecesPlaces(Piece::Id::WHITE_KING);
	const std::list<std::pair<int, int>>& blackKingsPlaces = board.getPiecesPlaces(Piece::Id::BLACK_KING);

	int pawnValue = 2;
	int kingValue = 5;
	//int forwardValue = 1;

	//for (const std::pair<int, int>& pair : whitePawnsPlaces) {
	//	res += pawnValue;
	//}

	//for (const std::pair<int, int>& pair : whiteKingsPlaces) {
	//	res += kingValue;
	//}

	//for (const std::pair<int, int>& pair : blackPawnsPlaces) {
	//	res -= pawnValue;
	//}

	//for (const std::pair<int, int>& pair : blackKingsPlaces) {
	//	res -= kingValue;
	//}

	res = (int)((whitePawnsPlaces.size() - blackPawnsPlaces.size()) * pawnValue + (whiteKingsPlaces.size() - blackKingsPlaces.size()) * kingValue);

	if (getColor() == Piece::Color::WHITE) {
		return res;
	}
	else {
		return -res;
	}
}
