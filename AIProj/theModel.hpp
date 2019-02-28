#pragma once

#include <string>
#include <vector>
#include <dynet\expr.h>
#include <dynet\training.h>
#include <memory>
#include "gameState.hpp"


class TheModel {
public:
	static const std::string MODEL_FILE_NAME;
	const float LEARNING_RATE = 0.3f;
	const int EPOCHES_NUMBER_PER_TRAIN = 30;
	const unsigned int HIDEN_LAYER_SIZE = 2;
	const unsigned int NN_INPUT_SIZE = 64;
	const unsigned int BATCH_SIZE = 1;

	static TheModel& getInstance();
	float compute(const GameState & gameState);
	void addExample(const GameState& state, float y);
	void train();
	void save(const std::string& path);
	void load(const std::string& path);
private:
	TheModel();
	std::vector<dynet::real> getNNInputFromGameState(const GameState& gameState);

	static std::unique_ptr<TheModel> theModelPtr;
	std::vector<std::pair<std::vector<dynet::real>, dynet::real>> dataset;
	dynet::ComputationGraph computationGraph;
	std::vector<dynet::real> xValue;
	std::vector<dynet::real> yValue;
	dynet::ParameterCollection modelParameters;
	std::unique_ptr<dynet::SimpleSGDTrainer> trainerPtr;
	dynet::Expression output;
	dynet::Expression loss;

};