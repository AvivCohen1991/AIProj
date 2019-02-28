#pragma once

#include <ostream>

class Piece {
public:
	enum class Id { WHITE_PAWN, WHITE_KING, BLACK_PAWN, BLACK_KING, EMPTY, PIECE_ID_SIZE };
	enum Color { WHITE, BLACK, PIECE_COLOR_SIZE };
	enum class Type { KING, PAWN, PIECE_TYPE_SIZE };

	static Type getType(Id id);
	static Color getColor(Id id);
	static Type getOtherType(Type type);
	static Color getOtherColor(Color type);
};

std::ostream& operator<<(std::ostream& out, const Piece::Id& id);