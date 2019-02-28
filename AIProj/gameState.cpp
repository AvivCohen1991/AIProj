#include "gameState.hpp"
#include <iostream>
#include <stdexcept>


void GameState::makeTrain(Train train) {
	board.makeTrain(train);
	turn = (turn == Piece::Color::WHITE ? Piece::Color::BLACK : Piece::Color::WHITE);
	computeStatusAndPossibleTrains();

	trainsUndoStack.push_back(train);
	trainsRedoStack.clear();

	lastTrainAndDirection.first = train;
	lastTrainAndDirection.second = TrainDirection::FORWARD;
}

bool GameState::undo() {
	if (trainsUndoStack.size() > 0) {
		Train train = trainsUndoStack.back();
		turn = (turn == Piece::Color::WHITE ? Piece::Color::BLACK : Piece::Color::WHITE);
		board.makeReverseTrain(train);

		computeStatusAndPossibleTrains();

		trainsUndoStack.pop_back();
		trainsRedoStack.push_back(train);

		lastTrainAndDirection.first = train;
		lastTrainAndDirection.second = TrainDirection::FORWARD;
		return true;
	}
	return false;
}

bool GameState::redo() {
	if (trainsRedoStack.size() > 0) {
		Train train = trainsRedoStack.back();
		trainsRedoStack.pop_back();
		board.makeTrain(train);

		turn = (turn == Piece::Color::WHITE ? Piece::Color::BLACK : Piece::Color::WHITE);
		computeStatusAndPossibleTrains();

		trainsUndoStack.push_back(train);

		lastTrainAndDirection.first = train;
		lastTrainAndDirection.second = TrainDirection::FORWARD;
		return true;
	}
	return false;
}

const Train & GameState::getPrevTrain() const {
	if (trainsUndoStack.empty()) {
		throw std::runtime_error("GameState::getPrevTrain() - no prev train");
	}
	return trainsUndoStack.back();
}

const Train & GameState::getNextTrain() const {
	if (trainsRedoStack.empty()) {
		throw std::runtime_error("GameState::getNextTrain - no next train");
	}
	return trainsRedoStack.back();
}

void GameState::computeStatusAndPossibleTrains() {
	currentPossibleTrains.clear();
	currentStatus = board.getStatusAndPosibleTrains(turn, currentPossibleTrains);
}

std::ostream & operator<<(std::ostream & out, const GameState & state) {
	if (state.turn == Piece::Color::WHITE) {
		out << "turn of white" << std::endl;
	}
	else {
		out << "turn of black" << std::endl;
	}

	out << state.board;

	switch (state.currentStatus) {
	case Board::Status::WHITE_WIN:
		out << "white win!!!" << std::endl;
		return out;
	case Board::Status::BLACK_WIN:
		out << "black win!!!" << std::endl;
		return out;
	case Board::Status::TIE:
		out << "tie!!!" << std::endl;
		return out;
	case Board::Status::RUNNING:
		out << "running..." << std::endl;
		break;
	}

	for (unsigned int i = 0; i < state.currentPossibleTrains.size(); ++i) {
		out << "(" << i << ") " << state.currentPossibleTrains[i];
	}

	return out;
}
