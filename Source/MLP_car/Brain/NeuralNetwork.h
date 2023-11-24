#pragma once
#ifndef NEURAL_NETWORK_H
#define NURAL_NETWORK_H
#include<vector>
#include"Layer.h"
#include<random>
#include<fstream>
#include<sstream>

struct PRINT {
	static void printVec1d(std::vector<double> vec);
};

class NeuralNetwork
{
private:
	std::vector<unsigned int> m_NetStructure;
	std::vector<Layer> m_NetLayers;

public:
	NeuralNetwork();
	NeuralNetwork(int val);
	NeuralNetwork(std::vector<unsigned int> structure); //Create a Network with random weights 
	NeuralNetwork(const std::string& filename);			//Load Model from saved File (The NetStructure should match)
	void saveModel(const std::string& filename);

	void backProp();					//supervised learning Method Efficient but Implementation is a big question mark
	void Mutate(float MutationRate , float MutationStddev);	//blindly follow the rule of neurons fire togather wire togather
	static NeuralNetwork CrossOver(NeuralNetwork &Parent1,NeuralNetwork &Parent2 , float MutationRate);

	std::vector<double> feedForward(std::vector<double> input);  // also called Predict,get_results;
	void displayDetails();
	inline std::vector<unsigned int> getNetStructure() { return this->m_NetStructure; }
	inline std::vector<Layer> getNetLayers() { return this->m_NetLayers; }
};


#endif // !NEURAL_NETWORK_H
