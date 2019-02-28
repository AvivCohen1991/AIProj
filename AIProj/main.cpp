#include "game.hpp"
#include "utils.hpp"
#include "humanPlayer.hpp"
#include "compPlayer.hpp"
#include "compLearnerPlayer.hpp"
#include "graphicalHumanPlayer.hpp"
#include <iostream>
#include <Windows.h>
#include <dynet\dynet.h>
#include <time.h>


int main(int argc, char **argv) try {
	dynet::initialize(argc, argv);

	HumanPlayer humanPlayer1;
	GraphicalHumanPlayer humanPlayer2;
	CompLearnerPlayer compLearnerPlayer;
	CompPlayer compPlayer;

	clock_t start = clock();

	//Game game(humanPlayer1, humanPlayer2);
	//game.run();

	Game game2(humanPlayer1, compPlayer);
	game2.run();

	clock_t end = clock();

	std::cout << double(end - start) / CLOCKS_PER_SEC << std::endl;

	return 0;
}
catch (std::exception& e) {
	std::cout << e.what() << std::endl;
}