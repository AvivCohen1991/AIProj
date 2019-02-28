#ifndef PAWN_H
#define PAWN_H

#include "piece.hpp"
#include "utils.hpp"
#include <iostream>

class Pawn : public Piece {
public:
	explicit Pawn(Color color, const std::pair<int, int>& place) : Piece(color, place) {}
private:
};

#endif // !PAWN_H
