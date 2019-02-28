#pragma once

#include "train.hpp"
#include <list>
#include <vector>
#include "piece.hpp"


class Board {
public:
	static const int BOARD_LENGTH = 8;
	enum class Status { RUNNING, WHITE_WIN, BLACK_WIN, TIE, STATUS_SIZE };
private:
	struct PiecesList {
		typedef std::list<std::pair<int, int>>::iterator iterator;

		explicit PiecesList(Piece::Id id = Piece::Id::EMPTY) {}
		iterator add(const std::pair<int, int>& place);
		void remove(iterator itr) { places.erase(itr); }

		std::list<std::pair<int, int>> places;
	};

	struct MatElement {
		MatElement() : id(Piece::Id::EMPTY) {}
		void setPlace(const std::pair<int, int>& place);
		bool isEmpty() const { return id == Piece::Id::EMPTY; }
		Piece::Id getId() const { return id; }

		Piece::Id id;
		PiecesList::iterator pieceItr;
	};
public:
	Board();
	Board(const Board& board);
	Board& operator=(const Board& board);
	void makeTrain(const Train& train);
	void makeReverseTrain(const Train& train);
	Status getStatusAndPosibleTrains(Piece::Color color, std::vector<Train>& res) const;
	Piece::Id getPiece(int i, int j) const;
	Piece::Id getPiece(const std::pair<int, int>& place) const;
	bool isEmpty(const std::pair<int, int>& place) const { return mat[place.first][place.second].isEmpty(); }
	bool operator==(const Board& board) const;
	friend std::ostream& operator<<(std::ostream& out, const Board& board);
	const std::list<std::pair<int, int>>& getPiecesPlaces(Piece::Id id) const { return piecesLists[(int)id].places; }
private:
	std::vector<PiecesList> piecesLists;
	MatElement mat[BOARD_LENGTH][BOARD_LENGTH];
private:
	void setPiece(Piece::Id id, int i, int j);
	void setPiece(Piece::Id id, const std::pair<int, int>& place);
	void destroy(int i, int j); /*delete the piece from the system*/
	void hide(int i, int j); /*hide mat element from the mat*/

							 /*functions that return all optional trains*/
	void addPosibleJumps(Piece::Id id, std::vector<Train>& res) const;
	void addPosibleMove(Piece::Id id, std::vector<Train>& res) const;

	void Board::getJumps(Piece::Id id, std::vector<Train>& res, const std::pair<int, int>& place) const;
	void Board::getJumpsRec(std::vector<Train>& res, std::vector<Train::Jump>& jumps, Piece::Color color, const std::pair<int, int>& srcPlace, const std::pair<int, int>& currPlace, bool isPawn) const;
	void Board::callingGetJumpsRecWithoutSideEffect(std::vector<Train>& resTrains, std::vector<Train::Jump>& resJumps, Piece::Color color, const std::pair<int, int>& srcPlace, const std::pair<int, int>& currPlace, bool isPawn, const Train::Jump& jump) const;
	bool Board::addJumpAndTargetPlace(int directionX, int directionY, Piece::Color color, const std::pair<int, int>& srcPlace, bool isPawn, std::pair<int, int>& resTargetPlace, Train::Jump& resJump) const;

	void Board::addPossibleMoves(Piece::Id id, std::vector<Train>& res, const std::pair<int, int>& srcPlace) const;
};

std::ostream& operator<<(std::ostream& out, const Board& board);