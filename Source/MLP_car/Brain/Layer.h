#ifndef LAYER_H
#define LAYER_H
#include <cstdlib>
#include <time.h>
#include <vector>
#include <iostream>

typedef std::vector<double> node;


class Layer
{
private:
	unsigned int m_LayerNodeCount;
	unsigned int m_InputNodeCount;
public:

	//Indexed as follows weightArray[layerNode][inputArrayIndex] -> gives the weight of the connection 
	std::vector < node > m_WeightArray;

	//stores the bias value of each node -> max = no.of Layernodes
	std::vector <double> m_BiasArray;

	//stores the computed sum of weighted inputs and bias -> max = no.of Layernodes
	std::vector <double> m_NodeArray;  
public:
	Layer(unsigned int InputNodeCount, unsigned int LayerNodeCount); //spawn a random weights and biases;
	Layer(unsigned int InputNodeCount, unsigned int LayerNodeCount, int val);
	Layer(unsigned int InputNodeCount, unsigned int LayerNodeCount,std::vector<node> weightArray, std::vector<double> biasArray);//Copy the weights and biases
	void feedForward(std::vector<double> InputArray);
	void activationFunction();

private:
	double reLU(double x);
	double sigmoid(double x);
	double Tanh(double x);
	double getRandomWeight();
	double getRandomBias();

public:
	void printWeights();
	void printBias();
	unsigned int getNumOfNodes();
	unsigned int getNumOfInputs();


	friend std::ostream& operator<< (std::ostream& out,Layer layer) {
		out << "Layer Info " << std::endl;
		out << "Node Info  -> (Input = " << layer.m_InputNodeCount << ",NumOfNodes = " << layer.m_LayerNodeCount << " )" << std::endl;
		return out;
	}
};


#endif // !LAYER_H