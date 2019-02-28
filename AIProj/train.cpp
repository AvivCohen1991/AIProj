#include "train.hpp"
#include <iostream>


std::ostream & operator<<(std::ostream & out, const Train & train) {
	out << "from (" << train.src.first << "," << train.src.second << ") to: (" << train.dst.first << "," << train.dst.second << ") crowned: " << train.getIsCrowned() << " eat: ";
	for (const Train::Jump& jump : train.jumps) {
		out << "(" << jump.first << "," << jump.second << "," << jump.id << ")";
	}
	out << std::endl;
	return out;
}

Train Train::getOppositeTrain(const Train & train) {
	Train oppositeTrain;
	oppositeTrain.setSrc(train.getDst());
	oppositeTrain.setDst(train.getSrc());
	std::vector<Train::Jump> jumps;
	for (auto jumpPtr = train.getJumps().rbegin(); jumpPtr != train.getJumps().rend(); ++jumpPtr) {
		jumps.push_back(*jumpPtr);
	}
	oppositeTrain.setJumps(jumps);
	return oppositeTrain;
}
