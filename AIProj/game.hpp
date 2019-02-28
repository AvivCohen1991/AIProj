#ifndef GAME_H
#define GAME_H

#include <vector>
#include "player.hpp"
#include "board.hpp"
#include "utils.hpp"
#include "gameState.hpp"

class Game {
public:
	//functions
	Game() = default;
	Game(Player&  whitePlayer, Player& blackPlayer);
	void run();
private:
	//attributes
	std::vector<Player*> players;
	GameState state;
};

#endif // !GAME_H
