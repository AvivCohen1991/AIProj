#include "piece.hpp"
#include "utils.hpp"


Piece::Color Piece::getColor(Piece::Id id) {
	if (id == Piece::Id::EMPTY) {
		throw std::runtime_error("Piece::getType - id is empty");
	}
	return (id == Piece::Id::WHITE_KING || id == Piece::Id::WHITE_PAWN) ? Piece::Color::WHITE : Piece::Color::BLACK;
}

Piece::Type Piece::getOtherType(Piece::Type type) {
	return (Piece::Type)(((int)type + 1) % (int)Piece::Type::PIECE_TYPE_SIZE);
}

Piece::Color Piece::getOtherColor(Piece::Color type) {
	return (Piece::Color)(((int)type + 1) % (int)Piece::Color::PIECE_COLOR_SIZE);
}

Piece::Type Piece::getType(Piece::Id id) {
	if (id == Piece::Id::EMPTY) {
		throw std::runtime_error("Piece::getType - id is empty");
	}
	return (id == Piece::Id::WHITE_KING || id == Piece::Id::BLACK_KING) ? Piece::Type::KING : Piece::Type::PAWN;
}

std::ostream & operator<<(std::ostream & out, const Piece::Id & id) {
	switch (id) {
	case Piece::Id::BLACK_KING:
		out << 'B';
		break;
	case Piece::Id::WHITE_KING:
		out << 'W';
		break;
	case Piece::Id::BLACK_PAWN:
		out << 'b';
		break;
	case Piece::Id::WHITE_PAWN:
		out << 'w';
		break;
	case Piece::Id::EMPTY:
		out << '.';
		break;
	default:
		assert(false);
	}
	return out;
}