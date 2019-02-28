#pragma once

#include "player.hpp"
#include <SFML\Graphics\Color.hpp>
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Window\Event.hpp>
#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\CircleShape.hpp>
#include <SFML\Graphics\Texture.hpp>
#include <thread>
#include <mutex>


class GraphicalHumanPlayer : public Player {
public:
	GraphicalHumanPlayer();
	virtual int getTrainIndex(const GameState& gameState);
	virtual void endGame(const GameState& gameState);
	virtual void startGame(const GameState& gameState);
private:
	const sf::Color WHITE_SQUARE_COLOR = sf::Color::White;
	const sf::Color BLACK_SQUARE_COLOR = sf::Color(40, 40, 80, 255);
	const sf::Color SRC_SQUARE_COLOR = sf::Color(25, 162, 189, 180);
	const sf::Color DST_SQUARE_COLOR = SRC_SQUARE_COLOR;
	const sf::Color JUMP_SQUARE_COLOR = sf::Color(161, 128, 120, 180);
	const sf::Color WHITE_PIECE_COLOR = sf::Color::Green;
	const sf::Color BLACK_PIECE_COLOR = sf::Color::Red;
	const float ANIMATION_SPEED = 250.f;

	enum class State { OPPONENT_TURN, WAITING_FOR_SELECT_PIECE, WAITING_FOR_DROP_PIECE, OPPONENT_ANIMATION_TRAIN, WIN, LOSE };
	enum SquareImage { WHITE, BLACK, SRC, DST, JUMP, SQUARES_IMAGES_NUM };

	class Path {
	public:
		Path() : isDone(false) {}
		explicit Path(const std::vector<sf::Vector2f>& points);
		bool move(float distance);
		const sf::Vector2f& getCurrentLocation() const { return currentLocation; }
		bool getIsDone() const { return isDone; }
	private:
		std::vector<sf::Vector2f> points;
		sf::Vector2f currentLocation;
		int currentPointIndex;
		bool isDone;
	};

	static void run(GraphicalHumanPlayer* graphicHumanPlayer);
	static void create(GraphicalHumanPlayer* graphicHumanPlayer);
	void handleEvent();
	void update(sf::Time fps);
	void draw();
	void drawPieces();
	void drawBoard();
	void drawSrcOptionsSquares();
	void drawDstAndJumpsOptionsSquares();
	void drawPickedUpPiece();
	void drawAnimationPiece(const std::pair<int, int>& trainSrc);
	std::pair<int, int> getClickedBoardPoint(int xPos, int yPos) const;
	bool isPickedUpSrc(int i, int j) const;
	bool isOpponentTrainSrc(int i, int j) const;
	bool isPrevTrainDst(int i, int j) const;
	bool isNextTrainSrc(int i, int j) const;
	bool isPossibleSrc(const std::pair<int, int>& src) const;
	bool isPossibleDst(const std::pair<int, int>& dst) const;
	int getChosenTrainIndex() const;
	std::vector<sf::Vector2f> getLocationsForTrain(const Train& train) const;
	sf::Vector2f getPieceDeltaPosFromSquare() const;
	std::pair<int, int> getCoordinatesWithPerspective(int i, int j) const;
	std::pair<int, int> getCoordinatesWithPerspective(const std::pair<int, int>& coordinates) const;
	bool isMyPiece(Piece::Id id) const;

	const sf::Time FPS = sf::Time(sf::seconds(1.0f / 60.0f));
	sf::RenderWindow target;
	std::vector<std::vector<sf::RectangleShape>> guiSquares;
	Train choosenTrain; //still without jumps
	Train opponentTrain;
	int choosenTrainIndex;
	std::mutex choosenTrainIndexMutex;
	State state;
	GameState gameState;
	Path animationPath;
	std::vector<sf::CircleShape> piecesImages;
	sf::Texture squaresTexture;
	sf::Texture pawnTexture;
	sf::Texture kingTexture;
	std::thread graphicThread;
	bool isNeededToCloseThread;
};