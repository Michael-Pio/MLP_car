// Feel Free to copy my works ,Have a great day -Michael Pio 


#include "GenerationDetails.h"

FGenerationDetails::FGenerationDetails()
{
	Generation = 0;
	AverageFitness = -1.f;
	MaxFitness = -1.f;
	NN_Runner = NeuralNetwork(0);
	NN_Winner = NeuralNetwork(0);
}

FGenerationDetails::FGenerationDetails(int Generation, float AverageFitness, float MaxFitness, NeuralNetwork NN_Winner, NeuralNetwork NN_Runner)
{
	this->Generation = Generation;
	this->AverageFitness = AverageFitness;
	this->MaxFitness = MaxFitness;
	this->NN_Winner = NN_Winner;
	this->NN_Runner = NN_Runner;
}

FGenerationDetails::~FGenerationDetails()
{
}
