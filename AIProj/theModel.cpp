#include "theModel.hpp"
#include <dynet\param-init.h>
#include <dynet\io.h>
#include <filesystem>


const std::string TheModel::MODEL_FILE_NAME = "theBaseModel.txt";
std::unique_ptr<TheModel> TheModel::theModelPtr = nullptr;

TheModel::TheModel(): dataset(BATCH_SIZE), xValue(NN_INPUT_SIZE * BATCH_SIZE), yValue(BATCH_SIZE) {
	//init the params values
	std::vector<float> initW1Vec(HIDEN_LAYER_SIZE * NN_INPUT_SIZE, 1.0f / (float)HIDEN_LAYER_SIZE);
	for (unsigned int j = NN_INPUT_SIZE / 2; j < NN_INPUT_SIZE; ++j) {
		for (unsigned int i = 0; i < HIDEN_LAYER_SIZE; ++i) {
			initW1Vec[j * HIDEN_LAYER_SIZE + i] = 2.5f / (float)HIDEN_LAYER_SIZE;
		}
	}
	dynet::ParameterInitFromVector initW1(initW1Vec);
	dynet::ParameterInitFromVector initW2(std::vector<float>(HIDEN_LAYER_SIZE, 1.0f));
	dynet::ParameterInitFromVector initb1(std::vector<float>(HIDEN_LAYER_SIZE, 0.0f));
	dynet::ParameterInitFromVector initb2({ 0.0f });

	//create the params
	dynet::Parameter p_W1 = modelParameters.add_parameters({ HIDEN_LAYER_SIZE, NN_INPUT_SIZE }, initW1, "W1");
	dynet::Parameter p_W2 = modelParameters.add_parameters({ 1, HIDEN_LAYER_SIZE }, initW2, "W2");
	dynet::Parameter p_b1 = modelParameters.add_parameters({ HIDEN_LAYER_SIZE }, initb1, "b1");
	dynet::Parameter p_b2 = modelParameters.add_parameters({ 1 }, initb2, "b2");

	trainerPtr = std::unique_ptr<dynet::SimpleSGDTrainer>(new dynet::SimpleSGDTrainer(modelParameters, LEARNING_RATE));

	dynet::Expression x = dynet::input(computationGraph, { NN_INPUT_SIZE, BATCH_SIZE }, &xValue);
	dynet::Expression y = dynet::input(computationGraph, { 1, BATCH_SIZE }, &yValue);
	dynet::Expression W1 = dynet::parameter(computationGraph, p_W1);
	dynet::Expression W2 = dynet::parameter(computationGraph, p_W2);
	dynet::Expression b1 = dynet::parameter(computationGraph, p_b1);
	dynet::Expression b2 = dynet::parameter(computationGraph, p_b2);
	dynet::Expression a1 = dynet::tanh(W1 * x + b1);
	output = dynet::tanh(W2 * a1 + b2);
	loss = dynet::squared_distance(output, y) / 2;

	if (std::experimental::filesystem::exists(MODEL_FILE_NAME)) {
		load(MODEL_FILE_NAME);
	}
	else {
		save(MODEL_FILE_NAME);
	}
}

TheModel & TheModel::getInstance() {
	if (theModelPtr == nullptr) {
		theModelPtr = std::unique_ptr<TheModel>(new TheModel());
	}
	return *theModelPtr;
}

float TheModel::compute(const GameState & gameState)
{
	std::vector<float> x = getNNInputFromGameState(gameState);
	for (unsigned int j = 0; j < x.size(); ++j) {
		xValue[j] = x[j];
	}
	float res = dynet::as_vector(computationGraph.forward(output))[0];
	return res;
}

void TheModel::addExample(const GameState & state, float y) {
	dataset.push_back(std::make_pair(getNNInputFromGameState(state), y));
	if (dataset.size() == BATCH_SIZE) {
		train();
	}
}

void TheModel::train() {
	int xValueIndex = 0;
	int yValueIndex = 0;

	for (auto& pair : dataset) {
		for (dynet::real x : pair.first) {
			xValue[xValueIndex] = x;
			++xValueIndex;
		}
		yValue[yValueIndex] = pair.second;
		++yValueIndex;
	}

	for (int epoch = 0; epoch < EPOCHES_NUMBER_PER_TRAIN; ++epoch) {
		computationGraph.backward(loss);
		trainerPtr->update();
	}
}

void TheModel::save(const std::string & path) {
	dynet::TextFileSaver saver(path);
	saver.save(modelParameters);
}

void TheModel::load(const std::string & path) {
	dynet::TextFileLoader loader(path);
	loader.populate(modelParameters);
}


std::vector<dynet::real> TheModel::getNNInputFromGameState(const GameState& gameState) {
	std::vector<float> x(NN_INPUT_SIZE, 0);

	unsigned int index = 0;

	const Board& board = gameState.getBoard();
	for (int i = 0; i < Board::BOARD_LENGTH; ++i) {
		for (int j = 1 - (i % 2); j < Board::BOARD_LENGTH; j += 2) {
			Piece::Id pieceId = board.getPiece(i, j);

			switch (pieceId) {
			case Piece::Id::WHITE_PAWN:
				x[index] = 1;
				break;
			case Piece::Id::WHITE_KING:
				x[index + NN_INPUT_SIZE / 2] = 1;
				break;
			case Piece::Id::BLACK_PAWN:
				x[index] = -1;
				break;
			case Piece::Id::BLACK_KING:
				x[index + NN_INPUT_SIZE / 2] = -1;
				break;
			case Piece::Id::EMPTY:
				x[index] = 0;
				break;
			}

			++index;
		}
	}
	return x;
}

