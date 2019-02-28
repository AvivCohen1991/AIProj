#include "game.hpp"
#include "train.hpp"
#include <iostream>


Game::Game(Player& whitePlayer, Player& blackPlayer) : players(2) {
	players[Piece::Color::WHITE] = &whitePlayer;
	players[Piece::Color::BLACK] = &blackPlayer;
	players[Piece::Color::WHITE]->setColor(Piece::Color::WHITE);
	players[Piece::Color::BLACK]->setColor(Piece::Color::BLACK);
}

void Game::run() {
	for (Player *player : players) {
		player->startGame(state);
	}

	while (state.getStatus() == Board::Status::RUNNING) {
		int trainIndex = players[state.getTurn()]->getTrainIndex(state);
		assert(trainIndex >= 0);
		assert(trainIndex < (int)state.getPossibleTrains().size());
		state.makeTrain(state.getPossibleTrains()[trainIndex]);
	}

	for (Player *player : players) {
		player->endGame(state);
	}
}

