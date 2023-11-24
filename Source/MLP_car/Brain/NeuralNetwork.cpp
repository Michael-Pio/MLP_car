#include "NeuralNetwork.h"

void PRINT::printVec1d(std::vector<double> vec) {
	std::cout << "  {";
	for (int i = 0; i < vec.size(); i++) {
		std::cout << vec[i];
		if (i != vec.size() - 1) {
			std::cout << ",";
		}
	}
	std::cout << "}  " << std::endl;
}

NeuralNetwork::NeuralNetwork()
{
	this->m_NetStructure = { 8,8,2 };
	
	for (unsigned int i = 0; i < this->m_NetStructure.size() - 1; i++) {
		Layer layer = Layer(this->m_NetStructure[i], this->m_NetStructure[i + 1]);
		this->m_NetLayers.push_back(layer);
	}

}
//@dummy baba
NeuralNetwork::NeuralNetwork(int val)
{
	this->m_NetStructure = { 8,8,2 };

	for (unsigned int i = 0; i < this->m_NetStructure.size() - 1; i++) {
		Layer layer = Layer(this->m_NetStructure[i], this->m_NetStructure[i + 1],val);
		this->m_NetLayers.push_back(layer);
	}
}

NeuralNetwork::NeuralNetwork(std::vector<unsigned int> structure) {

	this->m_NetStructure = structure;
	
	for (unsigned int i = 0; i < structure.size() - 1; i++) {
		Layer layer = Layer(structure[i], structure[i + 1]);
		this->m_NetLayers.push_back(layer);
	}
	
}

std::vector<double> NeuralNetwork::feedForward(std::vector<double> input)
{
	if (input.size() != this->m_NetStructure[0]) {
		std::cout << "[Error] Input size of Brain ,doesn't match the input size of network" << std::endl;
		std::cout << "\tRequired size = " << this->m_NetStructure[0] << "  given = " << input.size();
		//system("pause");
		//exit(0);

		return std::vector<double>();
	}

	for (unsigned int i = 0; i < m_NetLayers.size();i++) {
		if (i == 0) {
			m_NetLayers[i].feedForward(input);
			m_NetLayers[i].activationFunction();
		}
		else if (i == m_NetLayers.size() - 1) {
			m_NetLayers[i].feedForward(m_NetLayers[i - 1].m_NodeArray);
		}
		else {
			m_NetLayers[i].feedForward(m_NetLayers[i - 1].m_NodeArray);
			m_NetLayers[i].activationFunction();
		}
	}

	return m_NetLayers[m_NetLayers.size()-1].m_NodeArray;
}

void NeuralNetwork::displayDetails()
{
	std::cout << "Network Details:-" << std::endl;
	std::cout << "\t (1) Network Topology  : ";
	std::cout << "\t[ ";
	for (unsigned int Layer: m_NetStructure) {
		std::cout << Layer << " ";
	}
	std::cout << "]" <<std::endl;
	std::cout << "\t (2) Network Layers WandB  : "<<std::endl;

	std::cout << "\t\tLayer_0  \tNode count = " << m_NetLayers[0].getNumOfInputs() << std::endl;
	
	for (unsigned int i = 0; i < m_NetLayers.size(); i++)
	{
		std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
		std::cout << "\t\tLayer_" << i+1 << "  \tNode count = " << m_NetLayers[i].getNumOfNodes() << std::endl;
		m_NetLayers[i].printWeights();
		std::cout << "\t Biases = ";
		m_NetLayers[i].printBias();
	}
	std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
	
}


void NeuralNetwork::saveModel(const std::string& filename) {
	std::ofstream file(filename);
	if (file.is_open()) {
		for (Layer layer : this->m_NetLayers) {
			file << "W," << layer.getNumOfInputs() << "," << layer.getNumOfNodes() << ",";
			for (const auto& row : layer.m_WeightArray) {
				for (const auto& weight : row)
				{
					file << weight << ",";
				}
			}

			file << std::endl;

			file << "b," << layer.getNumOfInputs() << "," << layer.getNumOfNodes() << ",";
			for (const auto& bias : layer.m_BiasArray)
			{
				file << bias << ",";
			}
			file << std::endl;
		}
		file.close();
		std::cout << "file successfully saved in " << filename << std::endl;
	}
	else {
		std::cout << "[Error] file not saved in "<<filename << std::endl;
	}
}

//Key Learning Techniques
void NeuralNetwork::backProp() // sorry need to be Implemented Later
{

}

void NeuralNetwork::Mutate(float MutationRate,float MutationStddev)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<double> dist(-1.0, 1.0);

	
	//layerInd starts from 0 since it denotes the Hidden layers
	for (int layerInd = 0; layerInd < m_NetLayers.size(); layerInd++)
	{

		for (unsigned int nodeInd = 0; nodeInd < m_NetLayers[layerInd].m_WeightArray.size(); nodeInd++) {
			std::uniform_int_distribution<> dist_weights(0, m_NetLayers[layerInd].m_WeightArray[nodeInd].size());
			int crossPoint_Weights = dist_weights(gen);
			for (unsigned int weightsInd = 0; weightsInd < m_NetLayers[layerInd].m_WeightArray[nodeInd].size(); weightsInd++) {
				if (dist(gen) < MutationRate) {
					m_NetLayers[layerInd].m_WeightArray[nodeInd][weightsInd] += dist(gen) * MutationStddev;
				}
			}
		}

		for (unsigned int biasInd = 0; biasInd < m_NetLayers[layerInd].m_BiasArray.size(); biasInd++) {
			if (dist(gen) < MutationRate) {
				m_NetLayers[layerInd].m_BiasArray[biasInd] += dist(gen) * MutationStddev;
			}
		}
	}

}

NeuralNetwork NeuralNetwork::CrossOver(NeuralNetwork& Parent1, NeuralNetwork& Parent2, float MutationRate)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	NeuralNetwork Offspring = Parent1;

	
	if (Parent1.m_NetStructure != Parent2.m_NetStructure) {
		std::cout << "[Error] NN1 doesn't have the same structure as NN2" << std::endl;
		return Offspring;
	}
	//layerInd starts from 0 since it denotes the Hidden layers
	for (int layerInd = 0; layerInd < Parent1.m_NetLayers.size() ; layerInd++)
	{

		for (unsigned int nodeInd = 0; nodeInd < Parent1.m_NetLayers[layerInd].m_WeightArray.size(); nodeInd++) {
			std::uniform_int_distribution<> dist_weights(0, Parent1.m_NetLayers[layerInd].m_WeightArray[nodeInd].size());
			unsigned int crossPoint_Weights = dist_weights(gen);
			for (unsigned int weightsInd = 0; weightsInd < Parent1.m_NetLayers[layerInd].m_WeightArray[nodeInd].size(); weightsInd++) {
				if (weightsInd < crossPoint_Weights) {
					Offspring.m_NetLayers[layerInd].m_WeightArray[nodeInd][weightsInd] = Parent1.m_NetLayers[layerInd].m_WeightArray[nodeInd][weightsInd];
				}
				else {
					Offspring.m_NetLayers[layerInd].m_WeightArray[nodeInd][weightsInd] = Parent2.m_NetLayers[layerInd].m_WeightArray[nodeInd][weightsInd];
				}
			}
		}
		std::uniform_int_distribution<> dist_bias(0, Parent1.m_NetLayers[layerInd].m_BiasArray.size());
		unsigned int crossPoint_Bias = dist_bias(gen);
		for (unsigned int biasInd = 0; biasInd < Parent1.m_NetLayers[layerInd].m_BiasArray.size(); biasInd++) {
			if (biasInd < crossPoint_Bias) {
				Offspring.m_NetLayers[layerInd].m_BiasArray[biasInd] = Parent1.m_NetLayers[layerInd].m_BiasArray[biasInd];
			}
			else {
				Offspring.m_NetLayers[layerInd].m_BiasArray[biasInd] = Parent2.m_NetLayers[layerInd].m_BiasArray[biasInd];
			}
		}

		
	}
	return Offspring;
}



NeuralNetwork::NeuralNetwork(const std::string& filename)
{
	std::ifstream file(filename);
	std::string line;
	int dum = 0;
	if (!file.is_open()) {
		std::cout << "[Error] Target Model not Found "<<filename << std::endl;
		exit(EXIT_FAILURE);
		return;
	}
	
	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		char name;
		ss >> name;
		if (name == 'W')
		{
			unsigned int input_size, node_count;
			char comma;
			ss>>comma >> input_size >> comma >> node_count >> comma;
			if (dum == 0) {
				this->m_NetStructure.push_back(input_size);
				dum++;
			}
			this->m_NetStructure.push_back(node_count);
			std::vector<std::vector<double>> weights;
			for (unsigned int i = 0; i < node_count; i++)
			{
				std::vector<double> row;
				for (unsigned int j = 0; j < input_size; j++)
				{
					double weight;
					ss >> weight >> comma;
					row.push_back(weight);
				}
				weights.push_back(row);
			}
			this->m_NetLayers.push_back(Layer(input_size,node_count, weights, std::vector<double>()));
		}
		else if (name == 'b')
		{
			int input_size, output_size;
			char comma;
			ss >>comma >> input_size >> comma >> output_size >> comma;
			std::vector<double> biases;
			for (int i = 0; i < output_size; i++)
			{
				double bias;
				ss >> bias >> comma;
				biases.push_back(bias);
			}
			this->m_NetLayers.back().m_BiasArray = biases;
		}
	}
	file.close();
}