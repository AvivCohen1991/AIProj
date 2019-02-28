#pragma once

#include <list>
#include <vector>
#include "piece.hpp"


class Train {
public:
	struct Jump {
		Jump() = default;
		Jump(int i, int j, Piece::Id id) : first(i), second(j), id(id) {}

		int first;
		int second;
		Piece::Id id;
	};
public:
	Train() = default;
	Train(const std::pair<int, int>& src, const std::pair<int, int>& dst, bool isCrowned = false) : src(src), dst(dst), isCrowned(isCrowned) {}
	void addToJumps(const Jump& jump) { jumps.push_back(jump); }
	void setDst(const std::pair<int, int>& ddst) { dst = ddst; }
	void setSrc(const std::pair<int, int>& ssrc) { src = ssrc; }
	void setJumps(const std::vector<Jump>& jjumps) { jumps = jjumps; }
	void setIsCrowned(bool crowned) { isCrowned = crowned; }
	const std::pair<int, int>& getSrc() const { return src; }
	const std::pair<int, int>& getDst() const { return dst; }
	const std::vector<Jump>& getJumps() const { return jumps; }
	bool getIsCrowned() const { return isCrowned; }
	static Train getOppositeTrain(const Train& train);
	friend std::ostream& operator<<(std::ostream& out, const Train& train);
private:
	std::pair<int, int> src;
	std::pair<int, int> dst;
	std::vector<Jump> jumps;
	bool isCrowned;
};

std::ostream& operator<<(std::ostream& out, const Train& train);
