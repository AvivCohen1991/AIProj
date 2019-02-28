#ifndef KING_H
#define KING_H

#include "piece.hpp"
#include "utils.hpp"

class King : public Piece {
public:
	explicit King(Color color, const std::pair<int, int>& place) : Piece(color, place) {}
private:
};

#endif // !KING_H
