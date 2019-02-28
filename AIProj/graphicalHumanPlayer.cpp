
#include "graphicalHumanPlayer.hpp"
#include "utils.hpp"
#include <iostream>
#include <SFML\System\Sleep.hpp>


GraphicalHumanPlayer::GraphicalHumanPlayer() : piecesImages(4), guiSquares(Board::BOARD_LENGTH) {}

void GraphicalHumanPlayer::create(GraphicalHumanPlayer * graphicHumanPlayer) {
	if (!graphicHumanPlayer->squaresTexture.loadFromFile("resources/squareImage.jpg")) {
		throw std::runtime_error("HumanPlayer::HumanPlayer - the resource squareImage.jpg not found");
	}

	if (!graphicHumanPlayer->pawnTexture.loadFromFile("resources/pawnImage.jpg")) {
		throw std::runtime_error("HumanPlayer::HumanPlayer - the resource pawnTexture.jpg not found");
	}

	if (!graphicHumanPlayer->kingTexture.loadFromFile("resources/kingImage.jpg")) {
		throw std::runtime_error("HumanPlayer::HumanPlayer - the resource kingTexture.jpg not found");
	}

	graphicHumanPlayer->target.create(sf::VideoMode(78 * Board::BOARD_LENGTH, 78 * Board::BOARD_LENGTH), "Checkers", sf::Style::Titlebar | sf::Style::Close);

	for (int i = 0; i < Board::BOARD_LENGTH; ++i) {
		graphicHumanPlayer->guiSquares[i] = std::vector<sf::RectangleShape>(Board::BOARD_LENGTH);
	}

	float squareImageWidth = graphicHumanPlayer->target.getSize().x / (float)Board::BOARD_LENGTH;
	float squareImageHeight = graphicHumanPlayer->target.getSize().y / (float)Board::BOARD_LENGTH;

	for (int i = 0; i < Board::BOARD_LENGTH; ++i) {
		for (int j = 0; j < Board::BOARD_LENGTH; ++j) {
			graphicHumanPlayer->guiSquares[i][j].setPosition(sf::Vector2f(j * squareImageWidth, i * squareImageHeight));
			graphicHumanPlayer->guiSquares[i][j].setSize(sf::Vector2f(squareImageWidth, squareImageHeight));

			sf::IntRect textureRect;
			textureRect.left = (int)(j * graphicHumanPlayer->guiSquares[i][j].getSize().x);
			textureRect.top = (int)(i * graphicHumanPlayer->guiSquares[i][j].getSize().y);
			textureRect.width = (int)graphicHumanPlayer->guiSquares[i][j].getSize().x;
			textureRect.height = (int)graphicHumanPlayer->guiSquares[i][j].getSize().y;
			graphicHumanPlayer->guiSquares[i][j].setTextureRect(textureRect);
			graphicHumanPlayer->guiSquares[i][j].setTexture(&graphicHumanPlayer->squaresTexture);

			if ((i + j) % 2 == 0) {
				graphicHumanPlayer->guiSquares[i][j].setFillColor(graphicHumanPlayer->WHITE_SQUARE_COLOR);
			}
			else {
				graphicHumanPlayer->guiSquares[i][j].setFillColor(graphicHumanPlayer->BLACK_SQUARE_COLOR);
			}
		}
	}

	graphicHumanPlayer->piecesImages[(int)Piece::Id::WHITE_PAWN].setTexture(&graphicHumanPlayer->pawnTexture);
	graphicHumanPlayer->piecesImages[(int)Piece::Id::WHITE_PAWN].setRadius((7 / 20.f) * squareImageWidth);
	graphicHumanPlayer->piecesImages[(int)Piece::Id::WHITE_PAWN].setFillColor(graphicHumanPlayer->WHITE_PIECE_COLOR);

	graphicHumanPlayer->piecesImages[(int)Piece::Id::BLACK_PAWN].setTexture(&graphicHumanPlayer->pawnTexture);
	graphicHumanPlayer->piecesImages[(int)Piece::Id::BLACK_PAWN].setRadius((7 / 20.f) * squareImageWidth);
	graphicHumanPlayer->piecesImages[(int)Piece::Id::BLACK_PAWN].setFillColor(graphicHumanPlayer->BLACK_PIECE_COLOR);

	graphicHumanPlayer->piecesImages[(int)Piece::Id::WHITE_KING].setTexture(&graphicHumanPlayer->kingTexture);
	graphicHumanPlayer->piecesImages[(int)Piece::Id::WHITE_KING].setRadius((7 / 20.f) * squareImageWidth);
	graphicHumanPlayer->piecesImages[(int)Piece::Id::WHITE_KING].setFillColor(graphicHumanPlayer->WHITE_PIECE_COLOR);

	graphicHumanPlayer->piecesImages[(int)Piece::Id::BLACK_KING].setTexture(&graphicHumanPlayer->kingTexture);
	graphicHumanPlayer->piecesImages[(int)Piece::Id::BLACK_KING].setRadius((7 / 20.f) * squareImageWidth);
	graphicHumanPlayer->piecesImages[(int)Piece::Id::BLACK_KING].setFillColor(graphicHumanPlayer->BLACK_PIECE_COLOR);

	run(graphicHumanPlayer);
}

void GraphicalHumanPlayer::run(GraphicalHumanPlayer* graphicHumanPlayer) {
	sf::Clock clock;

	sf::Time reserveTime = clock.restart();

	while (graphicHumanPlayer->target.isOpen() && !graphicHumanPlayer->isNeededToCloseThread) {
		reserveTime += clock.restart();

		graphicHumanPlayer->handleEvent();

		while (reserveTime >= graphicHumanPlayer->FPS) {
			reserveTime -= graphicHumanPlayer->FPS;
			graphicHumanPlayer->update(graphicHumanPlayer->FPS);
		}

		graphicHumanPlayer->draw();
	}

	graphicHumanPlayer->target.close();
}

int GraphicalHumanPlayer::getTrainIndex(const GameState & gameState) {
	int res;
	if (!gameState.isFirstTrain()) {
		opponentTrain = gameState.getPrevTrain();
		animationPath = Path(getLocationsForTrain(opponentTrain));
		state = State::OPPONENT_ANIMATION_TRAIN;
	}

	while (true) {
		choosenTrainIndexMutex.lock();
		res = choosenTrainIndex;
		choosenTrainIndexMutex.unlock();
		if (res != -1) {
			choosenTrainIndex = -1;
			break;
		}
	}

	return res;
}

void GraphicalHumanPlayer::endGame(const GameState & gameState) {
	if (state != State::WIN) {
		getTrainIndex(gameState);
	}

	graphicThread.join();
}

void GraphicalHumanPlayer::startGame(const GameState & gameState) {
	//Path animationPath;

	isNeededToCloseThread = false;

	this->gameState = GameState();
	state = State::OPPONENT_TURN;

	choosenTrainIndex = -1;
	graphicThread = std::thread(create, this);

	sf::sleep(sf::seconds(0.1f));

	if (color == Piece::Color::WHITE) {
		state = State::WAITING_FOR_SELECT_PIECE;
	}
}

void GraphicalHumanPlayer::handleEvent() {
	sf::Event event;

	while (target.pollEvent(event)) {

		switch (event.type) {
			case sf::Event::Closed: {
				target.close();
				break;
			}

			case sf::Event::MouseButtonPressed: {
				std::pair<int, int> src = getCoordinatesWithPerspective(getClickedBoardPoint(event.mouseButton.x, event.mouseButton.y));

				if (state == State::WAITING_FOR_SELECT_PIECE && isPossibleSrc(src)) {
					choosenTrain.setSrc(src);
					state = State::WAITING_FOR_DROP_PIECE;
				}
				break;
			}

			case sf::Event::MouseButtonReleased: {
				std::pair<int, int> dst = getCoordinatesWithPerspective(getClickedBoardPoint(event.mouseButton.x, event.mouseButton.y));

				if (state == State::WAITING_FOR_DROP_PIECE) {
					choosenTrain.setDst(dst);

					if (isPossibleDst(dst)) {
						int choosenTrainIndex = getChosenTrainIndex();
						Train choosenTrain = gameState.getPossibleTrains()[choosenTrainIndex];
						gameState.makeTrain(choosenTrain);

						logout("********************* human player ******************");
						logout(gameState);
						logout("***********************************************************");

						choosenTrainIndexMutex.lock();
						this->choosenTrainIndex = choosenTrainIndex;
						choosenTrainIndexMutex.unlock();

						if (gameState.getStatus() == Board::Status::RUNNING) {
							state = State::OPPONENT_TURN;
						}
						else {
							state = State::WIN;
							isNeededToCloseThread = true;
						}
					}
					else {
						state = State::WAITING_FOR_SELECT_PIECE;
					}
				}
				break;
			}
		}
	}
}

void GraphicalHumanPlayer::update(sf::Time fps) {
	switch (state) {
	case State::OPPONENT_ANIMATION_TRAIN:
		if (animationPath.getIsDone()) {
			gameState.makeTrain(opponentTrain);

			logout("********************* human player ******************");
			logout(gameState);
			logout("***********************************************************");

			if (gameState.getStatus() == Board::Status::RUNNING) {
				state = State::WAITING_FOR_SELECT_PIECE;
			}
			else {
				state = State::LOSE;
				choosenTrainIndexMutex.lock();
				this->choosenTrainIndex = 0;
				choosenTrainIndexMutex.unlock();
				isNeededToCloseThread = true;
			}
		}
		else {
			animationPath.move(fps.asSeconds() * ANIMATION_SPEED);
		}
		break;
	}
}

void GraphicalHumanPlayer::draw() {
	drawBoard();

	if (state == State::WAITING_FOR_SELECT_PIECE) {
		drawSrcOptionsSquares();
	}
	else if (state == State::WAITING_FOR_DROP_PIECE) {
		drawDstAndJumpsOptionsSquares();
	}

	drawPieces();

	if (state == State::WAITING_FOR_DROP_PIECE) {
		drawPickedUpPiece();
	}
	else if (state == State::OPPONENT_ANIMATION_TRAIN) {
		drawAnimationPiece(opponentTrain.getSrc());
	}

	target.display();
}

void GraphicalHumanPlayer::drawBoard() {
	for (int i = 0; i < Board::BOARD_LENGTH; ++i) {
		for (int j = 0; j < Board::BOARD_LENGTH; ++j) {
			target.draw(guiSquares[i][j]);
		}
	}
}

void GraphicalHumanPlayer::drawSrcOptionsSquares() {
	for (const Train& possibleTrain : gameState.getPossibleTrains()) {
		std::pair<int, int> perspectiveSrcCoordinates = getCoordinatesWithPerspective(possibleTrain.getSrc().first, possibleTrain.getSrc().second);
		guiSquares[perspectiveSrcCoordinates.first][perspectiveSrcCoordinates.second].setFillColor(SRC_SQUARE_COLOR);
		target.draw(guiSquares[perspectiveSrcCoordinates.first][perspectiveSrcCoordinates.second]);
		guiSquares[perspectiveSrcCoordinates.first][perspectiveSrcCoordinates.second].setFillColor(BLACK_SQUARE_COLOR);
	}
}

void GraphicalHumanPlayer::drawDstAndJumpsOptionsSquares() {
	for (const Train& possibleTrain : gameState.getPossibleTrains()) {
		if (possibleTrain.getSrc() == choosenTrain.getSrc()) {
			std::pair<int, int> perspectiveDstCoordinates = getCoordinatesWithPerspective(possibleTrain.getDst().first, possibleTrain.getDst().second);
			guiSquares[perspectiveDstCoordinates.first][perspectiveDstCoordinates.second].setFillColor(DST_SQUARE_COLOR);
			target.draw(guiSquares[perspectiveDstCoordinates.first][perspectiveDstCoordinates.second]);
			guiSquares[perspectiveDstCoordinates.first][perspectiveDstCoordinates.second].setFillColor(BLACK_SQUARE_COLOR);

			std::vector<Train::Jump> jumps = possibleTrain.getJumps();
			for (Train::Jump& jump : jumps) {
				std::pair<int, int> perspectiveJumpCoordinates = getCoordinatesWithPerspective(jump.first, jump.second);
				guiSquares[perspectiveJumpCoordinates.first][perspectiveJumpCoordinates.second].setFillColor(JUMP_SQUARE_COLOR);
				target.draw(guiSquares[perspectiveJumpCoordinates.first][perspectiveJumpCoordinates.second]);
				guiSquares[perspectiveJumpCoordinates.first][perspectiveJumpCoordinates.second].setFillColor(BLACK_SQUARE_COLOR);
			}
		}
	}
}

void GraphicalHumanPlayer::drawPickedUpPiece() {
	std::pair<int, int> perspectiveCoordinates = getCoordinatesWithPerspective(choosenTrain.getSrc().first, choosenTrain.getSrc().second);

	const sf::Vector2f pieceDeltaPosFromSquare = getPieceDeltaPosFromSquare();

	Piece::Id pieceId = gameState.getBoard().getPiece(choosenTrain.getSrc());
	sf::CircleShape& guiPiece = piecesImages[(int)pieceId];

	sf::Vector2i mousePos = sf::Mouse::getPosition(target);

	guiPiece.setPosition(mousePos.x - guiSquares[0][0].getSize().x / 2.f + pieceDeltaPosFromSquare.x, mousePos.y - guiSquares[0][0].getSize().y / 2.f + pieceDeltaPosFromSquare.y);

	target.draw(guiPiece);
}

void GraphicalHumanPlayer::drawAnimationPiece(const std::pair<int, int>& trainSrc) {
	const sf::Vector2f pieceDeltaPosFromSquare = getPieceDeltaPosFromSquare();

	Piece::Id pieceId = gameState.getBoard().getPiece(trainSrc);
	sf::CircleShape guiPiece = piecesImages[(int)pieceId];

	guiPiece.setPosition(animationPath.getCurrentLocation());

	target.draw(guiPiece);
}

void GraphicalHumanPlayer::drawPieces() {
	const sf::Vector2f pieceDeltaPosFromSquare = getPieceDeltaPosFromSquare();

	for (int i = 0; i < Board::BOARD_LENGTH; ++i) {
		for (int j = 0; j < Board::BOARD_LENGTH; ++j) {
			std::pair<int, int> perspectiveCoordinates = getCoordinatesWithPerspective(i, j);

			if (!(state == State::WAITING_FOR_DROP_PIECE && isPickedUpSrc(perspectiveCoordinates.first, perspectiveCoordinates.second) ||
				(state == State::OPPONENT_ANIMATION_TRAIN && isOpponentTrainSrc(perspectiveCoordinates.first, perspectiveCoordinates.second)))) {

				Piece::Id pieceId = gameState.getBoard().getPiece(perspectiveCoordinates);

				if (pieceId == Piece::Id::EMPTY) {
					continue;
				}

				sf::CircleShape guiPiece = piecesImages[(int)pieceId];

				guiPiece.setPosition(guiSquares[i][j].getPosition() + pieceDeltaPosFromSquare);

				target.draw(guiPiece);
			}
		}
	}
}

std::pair<int, int> GraphicalHumanPlayer::getClickedBoardPoint(int xPos, int yPos) const {
	return std::make_pair(yPos / (int)guiSquares[0][0].getSize().y, xPos / (int)guiSquares[0][0].getSize().x);
}

bool GraphicalHumanPlayer::isPickedUpSrc(int i, int j) const {
	return choosenTrain.getSrc().first == i && choosenTrain.getSrc().second == j;
}

bool GraphicalHumanPlayer::isOpponentTrainSrc(int i, int j) const {
	return opponentTrain.getSrc() == std::make_pair(i, j);
}

bool GraphicalHumanPlayer::isPrevTrainDst(int i, int j) const {
	return gameState.getPrevTrain().getDst() == std::make_pair(i, j);
}

bool GraphicalHumanPlayer::isNextTrainSrc(int i, int j) const {
	return gameState.getNextTrain().getSrc() == std::make_pair(i, j);
}

bool GraphicalHumanPlayer::isPossibleSrc(const std::pair<int, int>& src) const {
	for (const Train& train : gameState.getPossibleTrains()) {
		if (src == train.getSrc()) {
			return true;
		}
	}

	return false;
}

bool GraphicalHumanPlayer::isPossibleDst(const std::pair<int, int>& dst) const {
	for (const Train& train : gameState.getPossibleTrains()) {
		if (dst == train.getDst() && this->choosenTrain.getSrc() == train.getSrc()) {
			return true;
		}
	}

	return false;
}

int GraphicalHumanPlayer::getChosenTrainIndex() const {
	const std::vector<Train>& possibleTrains = gameState.getPossibleTrains();

	for (unsigned int i = 0; i < possibleTrains.size(); ++i) {
		if (possibleTrains[i].getSrc() == choosenTrain.getSrc() && possibleTrains[i].getDst() == choosenTrain.getDst()) {
			return i;
		}
	}

	assert(false);
	return -1;
}

std::vector<sf::Vector2f> GraphicalHumanPlayer::getLocationsForTrain(const Train& train) const {
	std::vector<std::pair<int, int>> places;

	const std::vector<Train::Jump>& jumps = train.getJumps();
	std::pair<int, int> lastPlace;

	lastPlace = train.getSrc();

	places.push_back(train.getSrc());

	if (jumps.size() > 0) {
		for (const Train::Jump& jump : jumps) {
			sf::Vector2i direction(jump.first - lastPlace.first, jump.second - lastPlace.second);
			direction /= std::abs(direction.x);
			lastPlace = std::make_pair(jump.first + direction.x, jump.second + direction.y);
			places.push_back(lastPlace);
		}
	}
	else {
		places.push_back(train.getDst());
	}

	std::vector<sf::Vector2f> locations;
	for (const auto& place : places) {
		std::pair<int, int> perspectivecoordinates = getCoordinatesWithPerspective(std::make_pair(place.first, place.second));
		locations.push_back(guiSquares[perspectivecoordinates.first][perspectivecoordinates.second].getPosition() + getPieceDeltaPosFromSquare());
	}

	return locations;
}

sf::Vector2f GraphicalHumanPlayer::getPieceDeltaPosFromSquare() const {
	return sf::Vector2f((guiSquares[0][0].getSize() - sf::Vector2f(piecesImages[(int)Piece::Id::WHITE_PAWN].getRadius() * 2, piecesImages[(int)Piece::Id::WHITE_PAWN].getRadius() * 2)) / 2.f);
}

GraphicalHumanPlayer::Path::Path(const std::vector<sf::Vector2f>& points) : points(points),
currentPointIndex(0), isDone(false) {
	assert(points.size() > 0);
	currentLocation = points[0];
}

bool GraphicalHumanPlayer::Path::move(float distance) {
	if (currentPointIndex == points.size() - 1) {
		isDone = true;
		return false;
	}

	sf::Vector2f delta = points[currentPointIndex + 1] - points[currentPointIndex];
	delta = delta / norm(delta) * distance;
	currentLocation += delta;

	sf::Vector2f directionToNextPoint = points[currentPointIndex + 1] - currentLocation;
	if (directionToNextPoint.x * delta.x < 0) {
		++currentPointIndex;
		currentLocation = points[currentPointIndex];
	}

	return true;
}

std::pair<int, int> GraphicalHumanPlayer::getCoordinatesWithPerspective(int i, int j) const {
	if (color == Piece::Color::WHITE) {
		return std::make_pair(i, j);
	}
	else {
		return std::make_pair(Board::BOARD_LENGTH - i - 1, Board::BOARD_LENGTH - j - 1);
	}
}

std::pair<int, int> GraphicalHumanPlayer::getCoordinatesWithPerspective(const std::pair<int, int>& coordinates) const {
	return getCoordinatesWithPerspective(coordinates.first, coordinates.second);
}

bool GraphicalHumanPlayer::isMyPiece(Piece::Id id) const {
	return ((getColor() == Piece::Color::WHITE) && (id == Piece::Id::WHITE_KING || id == Piece::Id::WHITE_PAWN)) ||
		((getColor() == Piece::Color::BLACK) && (id == Piece::Id::BLACK_KING || id == Piece::Id::BLACK_PAWN));
}