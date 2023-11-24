#include "Layer.h"

double Layer::reLU(double x)
{
	return (x <= 0) ? 0 : x;
}

double Layer::sigmoid(double x)   //f(x) = 1 / (1 + e ^ -x)
{
	return 1/(1+exp(-x));
}

double Layer::Tanh(double x)
{
	return tanh(x);
}
//TO DO LATER
void Layer::activationFunction()
{
	for (unsigned int i = 0; i < m_LayerNodeCount; i++) {
		m_NodeArray[i] = Tanh(m_NodeArray[i]);
	}
}

Layer::Layer(unsigned int InputNodeCount, unsigned int LayerNodeCount,std::vector<node> weightArray, std::vector<double> biasArray)
{
	this->m_LayerNodeCount = LayerNodeCount;
	this->m_InputNodeCount = InputNodeCount;
	this->m_WeightArray = weightArray;
	this->m_BiasArray = biasArray;
}

Layer::Layer(unsigned int InputNodeCount, unsigned int LayerNodeCount) {
	this->m_LayerNodeCount = LayerNodeCount;
	this->m_InputNodeCount = InputNodeCount;
	srand(time(0) * (double)rand());

	for (unsigned int i = 0; i < m_LayerNodeCount; i++) {
		std::vector<double> WeightRow;
		for (unsigned int j = 0; j < InputNodeCount; j++) {
			WeightRow.push_back(getRandomWeight());
		}
		//randomly setting weights for each connection
		m_WeightArray.push_back(WeightRow);

		//randomly setting bias for each Nodes in layer
		m_BiasArray.push_back(getRandomBias());
	}
}

Layer::Layer(unsigned int InputNodeCount, unsigned int LayerNodeCount ,  int val) {
	this->m_LayerNodeCount = LayerNodeCount;
	this->m_InputNodeCount = InputNodeCount;
	srand(time(0) * (double)rand());

	for (unsigned int i = 0; i < m_LayerNodeCount; i++) {
		std::vector<double> WeightRow;
		for (unsigned int j = 0; j < InputNodeCount; j++) {
			WeightRow.push_back(val);
		}
		//randomly setting weights for each connection
		m_WeightArray.push_back(WeightRow);

		//randomly setting bias for each Nodes in layer
		m_BiasArray.push_back(val);
	}
}

void Layer::feedForward( std::vector<double> InputArray)
{
	this->m_NodeArray.clear();
	double sum = 0;
	for (unsigned int iLnode = 0; iLnode < m_LayerNodeCount; iLnode++) {
		for (unsigned int jIWeight = 0; jIWeight < m_InputNodeCount; jIWeight++) {
			sum += this->m_WeightArray[iLnode][jIWeight]*InputArray[jIWeight];
		}
		m_NodeArray.push_back(sum + m_BiasArray[iLnode]);
	}
}

double Layer::getRandomWeight() {
	return (1 - ((double)rand() / RAND_MAX) * 2);
}

double Layer::getRandomBias() {
	return (1 - ((double)rand() / RAND_MAX)*2);
}

void Layer::printWeights() {
	for (unsigned int i = 0; i < m_LayerNodeCount; i++) {
		std::cout << "     |";
		for (unsigned int j = 0; j < m_InputNodeCount; j++) {
			std::cout << "\t";
			fprintf(stdout, "%.6f",m_WeightArray[i][j]);
		}
		std::cout << "  |\n";
	}
}

void Layer::printBias()
{
	std::cout << "[ ";
	for (int i = 0; i < m_BiasArray.size(); i++) {
		
		fprintf(stdout, "%.6f", m_BiasArray[i]);
		std::cout << "  ";
	}
	std::cout << "]" << std::endl;
}

unsigned int Layer::getNumOfNodes()
{
	return m_LayerNodeCount;
}

unsigned int Layer::getNumOfInputs()
{
	return m_InputNodeCount;
}
