#pragma once

#include "board.hpp"


class GameState {
public:
	typedef Piece::Color Turn;
	enum class TrainDirection { FORWARD, BACKWARD, NONE };
	static const int EMPTY_TRAIN_INDEX = -1;
public:
	GameState() : turn(Turn::WHITE), lastTrainAndDirection(Train(), TrainDirection::NONE) { computeStatusAndPossibleTrains(); }
	Turn getNextTurn(Turn turn) const { return (Turn)(((int)turn + 1) % (int)Turn::PIECE_COLOR_SIZE); }
	Turn getTurn() const { return turn; }
	const std::vector<Train>& getPossibleTrains() const { return currentPossibleTrains; }
	const Board::Status& getStatus() const { return currentStatus; }
	void makeTrain(Train train);
	const Board& getBoard() const { return board; }
	bool undo();
	bool redo();
	const Train& getPrevTrain() const;
	const Train& getNextTrain() const;
	//const std::pair<Train, TrainDirection>& getLastTrain() const { return lastTrainAndDirection; }
	bool isFirstTrain() const { return trainsUndoStack.empty(); }
	bool isThereNext() const { return !trainsRedoStack.empty(); }
	bool isTherePrev() const { return !trainsUndoStack.empty(); }
	friend std::ostream& operator<<(std::ostream& out, const GameState& state);
private:
	void computeStatusAndPossibleTrains();

	Board board;
	Turn turn;
	std::pair<Train, TrainDirection> lastTrainAndDirection;
	std::vector<Train> trainsUndoStack;
	std::vector<Train> trainsRedoStack;
	std::vector<Train> currentPossibleTrains;
	Board::Status currentStatus;
};

std::ostream& operator<<(std::ostream& out, const GameState& state);