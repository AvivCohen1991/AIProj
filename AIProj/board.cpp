#include "board.hpp"
#include <iostream>
#include "utils.hpp"


Board::Board() : piecesLists(4) {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::pair<int, int> place = std::make_pair(i, 2 * j + (i + 1) % 2);
			setPiece(Piece::Id::BLACK_PAWN, place);
		}
	}

	for (int i = 5; i < BOARD_LENGTH; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::pair<int, int> place = std::make_pair(i, 2 * j + (i + 1) % 2);
			setPiece(Piece::Id::WHITE_PAWN, place);
		}
	}
}

Board::Board(const Board & board) : piecesLists(4) {
	for (int i = 0; i < BOARD_LENGTH; ++i) {
		for (int j = 0; j < BOARD_LENGTH; ++j) {
			setPiece(board.getPiece(i, j), i, j);
		}
	}
}

Board & Board::operator=(const Board & board) {
	if (&board == this) {
		return *this;
	}

	for (int i = 0; i < BOARD_LENGTH; ++i) {
		for (int j = 0; j < BOARD_LENGTH; ++j) {
			mat[i][j] = MatElement();
		}
	}

	for (auto& list : piecesLists) {
		list.places.clear();
	}

	for (int i = 0; i < BOARD_LENGTH; ++i) {
		for (int j = 0; j < BOARD_LENGTH; ++j) {
			setPiece(board.getPiece(i, j), i, j);
		}
	}

	return *this;
}

//need to be fast
void Board::makeTrain(const Train & train) {
	for (const Train::Jump& jump : train.getJumps()) {
		destroy(jump.first, jump.second);
	}

	std::pair<int, int> src = train.getSrc();
	std::pair<int, int> dst = train.getDst();

	Piece::Id srcId = mat[src.first][src.second].getId();

	destroy(src.first, src.second);

	if (dst.first == 0 && srcId == Piece::Id::WHITE_PAWN) {
		setPiece(Piece::Id::WHITE_KING, dst);
	}
	else if (dst.first == BOARD_LENGTH - 1 && srcId == Piece::Id::BLACK_PAWN) {
		setPiece(Piece::Id::BLACK_KING, dst);
	}
	else {
		setPiece(srcId, dst);
	}
}

void Board::makeReverseTrain(const Train & train) {
	std::pair<int, int> src = train.getSrc();
	std::pair<int, int> dst = train.getDst();

	Piece::Id dstId = mat[dst.first][dst.second].getId();

	destroy(dst.first, dst.second);

	if (train.getIsCrowned()) {
		if (dstId == Piece::Id::WHITE_KING) {
			setPiece(Piece::Id::WHITE_PAWN, src);
		}
		else if (dstId == Piece::Id::BLACK_KING) {
			setPiece(Piece::Id::BLACK_PAWN, src);
		}
		else {
			assert(false);
		}
	}
	else {
		setPiece(dstId, src);
	}

	for (const Train::Jump& jump : train.getJumps()) {
		setPiece(jump.id, jump.first, jump.second);
	}
}

//need to be fast
Board::Status Board::getStatusAndPosibleTrains(Piece::Color color, std::vector<Train>& res) const {
	if (color == Piece::Color::WHITE) {
		addPosibleJumps(Piece::Id::WHITE_KING, res);
		addPosibleJumps(Piece::Id::WHITE_PAWN, res);

		if (!res.size()) {
			addPosibleMove(Piece::Id::WHITE_KING, res);
			addPosibleMove(Piece::Id::WHITE_PAWN, res);
		}

		for (Train& train : res) {
			Piece::Id srcId = mat[train.getSrc().first][train.getSrc().second].getId();
			if (train.getDst().first == 0 && (srcId == Piece::Id::WHITE_PAWN || srcId == Piece::Id::BLACK_PAWN)) {
				train.setIsCrowned(true);
			}
		}

		if (!res.size()) {
			return Status::BLACK_WIN;
		}
	}

	else if (color == Piece::Color::BLACK) {
		addPosibleJumps(Piece::Id::BLACK_KING, res);
		addPosibleJumps(Piece::Id::BLACK_PAWN, res);

		if (!res.size()) {
			addPosibleMove(Piece::Id::BLACK_KING, res);
			addPosibleMove(Piece::Id::BLACK_PAWN, res);
		}

		for (Train& train : res) {
			Piece::Id dstId = mat[train.getSrc().first][train.getSrc().second].getId();
			if (train.getDst().first == Board::BOARD_LENGTH - 1 && (dstId == Piece::Id::WHITE_PAWN || dstId == Piece::Id::BLACK_PAWN)) {
				train.setIsCrowned(true);
			}
		}

		if (!res.size()) {
			return Status::WHITE_WIN;
		}
	}

	return Status::RUNNING;
}

Piece::Id Board::getPiece(int i, int j) const {
	if (mat[i][j].isEmpty()) {
		return Piece::Id::EMPTY;
	}

	return mat[i][j].getId();
}

Piece::Id Board::getPiece(const std::pair<int, int>& pair) const {
	return getPiece(pair.first, pair.second);
}

bool Board::operator==(const Board & board) const {
	for (unsigned int i = 0; i < piecesLists.size(); ++i) {
		std::list<std::pair<int, int>> myPlaces = piecesLists[i].places;
		std::list<std::pair<int, int>> boardPlaces = board.piecesLists[i].places;
		myPlaces.sort();
		boardPlaces.sort();
		if (myPlaces != boardPlaces) {
			return false;
		}
	}

	for (int i = 0; i < BOARD_LENGTH; ++i) {
		for (int j = 0; j < BOARD_LENGTH; ++j) {
			if (mat[i][j].getId() != board.mat[i][j].getId() ||
				(mat[i][j].getId() != Piece::Id::EMPTY &&
					board.mat[i][j].getId() != Piece::Id::EMPTY &&
					*mat[i][j].pieceItr != *board.mat[i][j].pieceItr)) {
				return false;
			}
		}
	}

	return true;
}

//all this functions for getStatusAndPosibleTrains


/*
separate between pawn and king in the first train
res is empty if no possible trains
*/
void Board::getJumps(Piece::Id id, std::vector<Train>& resTrains, const std::pair<int, int>& srcPlace) const {
	Piece::Color color = Piece::getColor(id);
	bool isPawn = Piece::getType(id) == Piece::Type::PAWN;

	int minDirectionY = -1;
	int maxDirectionY = 1;

	if (isPawn) {
		if (color == Piece::Color::WHITE) {
			maxDirectionY = -1;
		}
		else {
			minDirectionY = 1;
		}
	}

	for (int directionY = minDirectionY; directionY <= maxDirectionY; directionY += 2) {
		for (int directionX = -1; directionX <= 1; directionX += 2) {
			std::pair<int, int> newCurrPlace;
			Train::Jump jump;
			if (addJumpAndTargetPlace(directionX, directionY, color, srcPlace, isPawn, newCurrPlace, jump)) {
				std::vector<Train::Jump> jumps;
				callingGetJumpsRecWithoutSideEffect(resTrains, jumps, color, srcPlace, newCurrPlace, isPawn, jump);
			}
		}
	}
}

void Board::getJumpsRec(std::vector<Train>& resTrains, std::vector<Train::Jump>& resJumps, Piece::Color color, const std::pair<int, int>& srcPlace, const std::pair<int, int>& currPlace, bool isPawn) const {
	bool isLeaf = true;
	for (int directionY = -1; directionY <= 1; directionY += 2) {
		for (int directionX = -1; directionX <= 1; directionX += 2) {
			std::pair<int, int> newCurrPlace;
			Train::Jump jump;
			if (addJumpAndTargetPlace(directionX, directionY, color, currPlace, isPawn, newCurrPlace, jump)) {
				isLeaf = false;
				callingGetJumpsRecWithoutSideEffect(resTrains, resJumps, color, srcPlace, newCurrPlace, isPawn, jump);
			}
		}
	}
	if (isLeaf) {
		Train train(srcPlace, currPlace);
		train.setJumps(resJumps);
		resTrains.push_back(train);
	}
}

void Board::callingGetJumpsRecWithoutSideEffect(std::vector<Train>& resTrains, std::vector<Train::Jump>& resJumps, Piece::Color color, const std::pair<int, int>& srcPlace, const std::pair<int, int>& currPlace, bool isPawn, const Train::Jump& jump) const {
	MatElement srcElement = mat[srcPlace.first][srcPlace.second];
	MatElement jumpElement = mat[jump.first][jump.second];

	const_cast<Board*>(this)->hide(srcPlace.first, srcPlace.second);
	const_cast<Board*>(this)->hide(jump.first, jump.second);

	resJumps.push_back(jump);

	getJumpsRec(resTrains, resJumps, color, srcPlace, currPlace, isPawn);

	resJumps.pop_back();

	const_cast<Board*>(this)->mat[jump.first][jump.second] = jumpElement;
	const_cast<Board*>(this)->mat[srcPlace.first][srcPlace.second] = srcElement;
}

/*return if is possible to jump*/
bool Board::addJumpAndTargetPlace(int directionX, int directionY, Piece::Color color, const std::pair<int, int>& srcPlace, bool isPawn, std::pair<int, int>& resTargetPlace, Train::Jump& resJump) const {
	int iterationsNumY = ((BOARD_LENGTH - 1) * (directionY + 1)) / 2 - directionY * srcPlace.first - 2;
	int iterationsNumX = ((BOARD_LENGTH - 1) * (directionX + 1)) / 2 - directionX * srcPlace.second - 2;
	int iterationsNum = (iterationsNumX < iterationsNumY ? iterationsNumX : iterationsNumY);
	if (iterationsNum < 0) { return false; }
	if (isPawn && iterationsNum > 0) { iterationsNum = 0; }

	int i;
	for (i = 1; i <= iterationsNum && mat[srcPlace.first + i * directionY][srcPlace.second + i * directionX].isEmpty(); ++i);

	std::pair<int, int> emptyPlace(srcPlace.first + (i + 1) * directionY, srcPlace.second + (i + 1) * directionX);
	std::pair<int, int> blocked(srcPlace.first + i * directionY, srcPlace.second + i * directionX);
	if (mat[emptyPlace.first][emptyPlace.second].isEmpty() && !mat[blocked.first][blocked.second].isEmpty() && Piece::getColor(mat[blocked.first][blocked.second].getId()) == Piece::getOtherColor(color)) {
		resTargetPlace.first = emptyPlace.first;
		resTargetPlace.second = emptyPlace.second;
		resJump.first = blocked.first;
		resJump.second = blocked.second;
		resJump.id = mat[blocked.first][blocked.second].getId();
		return true;
	}
	return false;
}

void Board::setPiece(Piece::Id id, int i, int j) {
	setPiece(id, std::make_pair(i, j));
}

void Board::setPiece(Piece::Id id, const std::pair<int, int>& place) {
	mat[place.first][place.second].id = id;
	if (id != Piece::Id::EMPTY) {
		mat[place.first][place.second].pieceItr = piecesLists[(int)id].add(place);
	}
}

void Board::destroy(int i, int j) {
	if (!mat[i][j].isEmpty()) {
		piecesLists[(int)mat[i][j].id].remove(mat[i][j].pieceItr);
	}
	mat[i][j].id = Piece::Id::EMPTY;
}

void Board::hide(int i, int j) {
	mat[i][j].id = Piece::Id::EMPTY;
}

inline void Board::addPosibleJumps(Piece::Id id, std::vector<Train>& res) const {
	assert(id != Piece::Id::EMPTY);

	for (auto& place : piecesLists[(int)id].places) {
		getJumps(id, res, place);
	}
}

void Board::addPosibleMove(Piece::Id id, std::vector<Train>& res) const {
	for (auto& place : piecesLists[(int)id].places) {
		addPossibleMoves(id, res, place);
	}
}

void Board::addPossibleMoves(Piece::Id id, std::vector<Train>& res, const std::pair<int, int>& srcPlace) const {
	int minDirectionY = -1;
	int maxDirectionY = 1;

	Piece::Color color = Piece::getColor(id);
	bool isPawn = Piece::getType(id) == Piece::Type::PAWN;

	if (isPawn) {
		if (color == Piece::Color::WHITE) {
			maxDirectionY = -1;
		}
		else {
			minDirectionY = 1;
		}
	}

	for (int directionY = minDirectionY; directionY <= maxDirectionY; directionY += 2) {
		for (int directionX = -1; directionX <= 1; directionX += 2) {
			int iterationsNumY = ((BOARD_LENGTH - 1) * (directionY + 1)) / 2 - directionY * srcPlace.first;
			int iterationsNumX = ((BOARD_LENGTH - 1) * (directionX + 1)) / 2 - directionX * srcPlace.second;
			int iterationsNum = (iterationsNumX < iterationsNumY ? iterationsNumX : iterationsNumY);
			if (isPawn && iterationsNum > 1) { iterationsNum = 1; }
			if (iterationsNum <= 0) { continue; }

			for (int i = 1; i <= iterationsNum && mat[srcPlace.first + i * directionY][srcPlace.second + i * directionX].isEmpty(); ++i) {
				res.push_back(Train(srcPlace, std::pair<int, int>(srcPlace.first + i * directionY, srcPlace.second + i * directionX)));
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////


inline void Board::MatElement::setPlace(const std::pair<int, int>& place) {
	pieceItr->first = place.first;
	pieceItr->second = place.second;
}

Board::PiecesList::iterator Board::PiecesList::add(const std::pair<int, int>& place) {
	places.push_back(place);
	return --places.end();
}

std::ostream & operator<<(std::ostream & out, const Board & board) {
	for (unsigned int i = 0; i < board.piecesLists.size(); ++i) {
		out << (Piece::Id(i)) << ": ";
		for (const auto& pair : board.piecesLists[i].places) {
			out << "(" << pair.first << "," << pair.second << ")";
		}
		out << std::endl;
	}

	for (int i = 0; i < Board::BOARD_LENGTH; ++i) {
		for (int j = 0; j < Board::BOARD_LENGTH; ++j) {
			out << board.mat[i][j].getId() << " ";
		}
		out << std::endl;
	}

	return out;
}
