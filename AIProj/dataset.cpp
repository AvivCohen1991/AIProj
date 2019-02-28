#include "dataset.hpp"

void Dataset::addExample(const GameState & state, float y) {
	examples.push_back(std::make_pair(getXFromGameState(state), y));
}

std::vector<dynet::real> Dataset::getXFromGameState(const GameState & gameState) {
	std::vector<float> x(X_SIZE, 0);

	unsigned int index = 0;

	const Board& board = gameState.getBoard();
	for (int i = 0; i < Board::BOARD_LENGTH; ++i) {
		for (int j = 1 - (i % 2); j < Board::BOARD_LENGTH; j += 2) {
			Piece::Id pieceId = board.getPiece(i, j);

			switch (pieceId) {
			case Piece::Id::WHITE_PAWN:
				x[index] = 1;
				break;
			case Piece::Id::WHITE_KING:
				x[index + X_SIZE / 2] = 1;
				break;
			case Piece::Id::BLACK_PAWN:
				x[index] = -1;
				break;
			case Piece::Id::BLACK_KING:
				x[index + X_SIZE / 2] = -1;
				break;
			case Piece::Id::EMPTY:
				x[index] = 0;
				break;
			}

			++index;
		}
	}
	return x;
}
