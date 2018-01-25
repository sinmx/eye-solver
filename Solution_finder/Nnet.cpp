#pragma once
#include "Nnet.h"
#include "utility.h"

#include <vector>
#include <algorithm>
using std::vector;

//initialize

Nnet::Nnet(){ Setup(0, 0, 0, 0); }
Nnet::Nnet(double Ninputs, double Nhiddens, int SizeHidden, double Noutputs) { Setup(Ninputs, Nhiddens, SizeHidden, Noutputs); }
Nnet::Nnet(std::string filename) {
	loadNet(filename);
}

int Nnet::Setup(double Ninputs, double Nhiddens, int SizeHidden, double Noutputs, bool loaded)
{
	if (!Ninputs || !Nhiddens || !SizeHidden || !Noutputs)//if an input is missing
	{
		return 0;
	}

	//set sizes
	input.size = Ninputs;
	output.size = Noutputs;
	m_Nhidden = SizeHidden;

	//resize layers
	input.neurons.resize(input.size);
	output.neurons.resize(output.size);
	hidden.resize(m_Nhidden);
	for (unsigned int i = 0; i < m_Nhidden; i++)
	{
		hidden[i].size = m_Nhidden;
		hidden[i].neurons.resize(hidden[i].size);
	}

	if (loaded)
	{
		age = 0;

	}


	//resize weights
	//randomize weights
	//randomize biases
	for (unsigned int i = 0; i < input.size; i++)
	{//INPUT
		input.neurons[i].weights.resize(hidden.front().size);
		if (!loaded)
		{
			for (unsigned int j = 0; j < hidden.front().size; j++)
				input.neurons[i].weights[j] = RandNum();
			input.neurons[i].bias = RandNum();
		}
	}
	for (unsigned int i = 0; i < output.size; i++)
	{//OUTPUT
		output.neurons[i].weights.resize(hidden.back().size);
		if (!loaded)
		{
			for (unsigned int j = 0; j < hidden.back().size; j++)
				output.neurons[i].weights[j] = RandNum();
			output.neurons[i].bias = RandNum();
		}
	}
	for (unsigned int i = 0; i < m_Nhidden-1; i++)
	{//HIDDEN
		for (unsigned int j = 0; j < hidden[i].size; j++)
		{
			hidden[i].neurons[j].weights.resize(hidden[i].size);
			if (!loaded)
			{
				for (unsigned int k = 0; k < hidden[i].size; k++)
					hidden[i].neurons[j].weights[k] = RandNum();
				hidden[i].neurons[j].bias = RandNum();
			}
		}
	}
	for (unsigned int i = 0; i < hidden.back().size; i++)
	{//LAST HIDDEN
		hidden.back().neurons[i].weights.resize(output.size);
		if (!loaded)
		{
			for (unsigned int j = 0; j < output.size; j++)
				hidden.back().neurons[i].weights[j] = RandNum();
			hidden.back().neurons[i].bias = RandNum();
		}
	}
	return 1;
}

unsigned int Nnet::GetLayerSize(type l)
{
	switch (l)
	{
	case Ninputs:
		return input.size;
		break;
	case Nhiddens:
		return m_Nhidden;
		break;
	case Noutputs:
		return output.size;
		break;
	case SizeHidden:
		return hidden.front().size;
		break;
	default:
		return -1;
		break;
	}
}

//set input and get output
vector<double> Nnet::Propigate(vector<double> inputs)
{
	//TIMER first
	startProp = clock(); //Start timer

	std::vector<double> biases, out;
		
	/* ORDER
		prev node * weight
		bias
		normalize
		set node
	*/

	//Input layer
	biases.resize(input.size);
	bool test = input.size == input.neurons.size();
	for (unsigned int b = 0; b < input.size; b++)
	{
		biases[b] = input.neurons[b].bias; 
	}
	AddBiases(inputs, biases, out);
	for (unsigned int i = 0; i < input.size; i++)
	{
		Normalize(out[i]);
		input.neurons[i].value = out[i];
	}

	//First Hidden Layer
	for (unsigned int i = 0; i < hidden.front().size; i++)
	{
		double adder =0 ;
		for (unsigned int j = 0; j < input.size; j++)
				adder += input.neurons[j].value * input.neurons[j].weights[i];//node*weight
		adder += hidden.front().neurons[i].bias;
		Normalize(adder);
		hidden.front().neurons[i].value = adder;
	}

	//Hidden Layers
	for (unsigned int i = 1; i < m_Nhidden; i++) {
		for (unsigned int j = 0; j < hidden[i].size; j++) {
			double adder=0;
			for (unsigned int k = 0; k < hidden[i].size; k++)
					adder += hidden[i - 1].neurons[j].value * hidden[i - 1].neurons[j].weights[k];
			adder += hidden[i].neurons[j].bias;
			Normalize(adder);
			hidden[i].neurons[j].value = adder;
		}
	}

	//Output Layer
	out.resize(output.size);
	for (unsigned int i = 0; i < output.size; i++)
	{
		double adder=0;
		for (unsigned int j = 0; j < m_Nhidden; j++)
			adder += hidden.back().neurons[j].value * output.neurons[i].weights[j];//node*weight
		adder += output.neurons[i].bias;
		//Normalize(adder);//normalize
		output.neurons[i].value = adder;
		out[i] = adder;
	}

	PassedProp = (clock() - startProp);// / CLOCKS_PER_SEC;
	return out;
}

void Nnet::Normalize(double &input){
	input = 1 / (1 + pow(e, -input));
}

void Nnet::AddBiases(vector<double> cur,vector<double> biases, vector<double> &out)
{
	if (cur.size() == biases.size())
	{
		for (unsigned int i = 0; i < cur.size(); i++)
			out.push_back(cur[i]+biases[i]);
	}
	else
		std::cout << "ERROR: Invalid vector size";
}

void Nnet::Mutate(double rate)
{
	//TIMER first
	startMut = clock(); //Start timer

	trate = rate;

	for (unsigned int i = 0; i < input.size; i++) {//input
		for (unsigned int j = 0; j < hidden.front().size; j++) {
			if (rate > RandNum())//if rand is less that its mutate rate
				MutTable(input.neurons[i].weights[j]);//mutate
		}
	}
	for (unsigned int i = 1; i < m_Nhidden; i++) {//hidden
		for (unsigned int j = 0; j < hidden[i].size; j++) {
			for (unsigned int k = 0; k < hidden[i].size; k++) {

				if (rate > RandNum())//if rand is less that its mutate rate
					MutTable(hidden[i - 1].neurons[j].weights[k]);//mutate
			}
		}
	}
	for (unsigned int i = 0; i < hidden.back().size; i++) {//last hidden
		for (unsigned int j = 0; j < output.size; j++) {
			if (rate > RandNum())//if rand is less that its mutate rate
				MutTable(hidden.back().neurons[i].weights[j]);//mutate
			//std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b";
			//std::cout << "\b\b\b\b\b\b\b\b\b\b\b\b";
		}
	}

	//biases
	for (unsigned int i = 0; i < input.size; i++) {
		if (rate > RandNum())
			MutTable(input.neurons[i].bias);
	}
	for (unsigned int i = 0; i < m_Nhidden; i++) {
		for (unsigned int j = 0; j < hidden[i].size; j++) {
			if (rate > RandNum())
				MutTable(hidden[i].neurons[j].bias);
		}
	}
	for (unsigned int i = 0; i < output.size; i++) {
		if (rate > RandNum())
			MutTable(output.neurons[i].bias);
	}

	PassedMut = (clock() - startMut);// / CLOCKS_PER_SEC;
}

void Nnet::MutTable(double &weight)
{
	const int MAX_WEIGHT = 9999;

	int choice = (int)(RandNum() * 3);
	double r = RandNum();

	//limit min/max
	if (weight > MAX_WEIGHT)
		weight = MAX_WEIGHT;
	else if (weight < -MAX_WEIGHT)
		weight = -MAX_WEIGHT;

	switch (choice)
	{
	case 0://change weight by percent
		weight *= r*1.5;
		break;
	case 1://invert weight
		weight *= -1;
			break;
	case 2://aproach a round number
		weight = (int)weight * r;
		break;
	}
}

vector<double> Nnet::getLastLayer()
{
	vector<double> out;
	for (unsigned int i = 0; i < output.size; i++)
	{
		out.push_back(output.neurons[i].value);
	}
	return out;
}

//in seconds
//call after propigate and mutate
double Nnet::GetSpeed() {
	double total = PassedMut + PassedProp;
	PassedMut = 0;	//prevents double counting	//
	PassedProp = 0;	//							//
	return total;
}

void Nnet::saveNet(std::string name)
{
	name.append(".bin");
	std::ofstream out(name, std::ios_base::binary);
	if (out.good())
	{
		//save sizes
		std::cout << "Writing Sizes of file" << std::endl;				//ORDER
		out.write((char *)&input.size, sizeof(unsigned int));			//input layer size
		out.write((char *)&output.size, sizeof(unsigned int));			//output layer size
		out.write((char *)&hidden.front().size, sizeof(unsigned int));	//hidden layers size
		out.write((char *)&m_Nhidden, sizeof(unsigned int));			//number of hidden layers

		std::cout << "Writing  biases" << std::fixed << std::endl;	//biases
		for (unsigned int i = 0; i < input.size; i++)
			out.write((char *)&input.neurons[i].bias, sizeof(double));
		for (unsigned int i = 0; i < output.size; i++)
			out.write((char *)&output.neurons[i].bias, sizeof(double));
		for (unsigned int i = 0; i < m_Nhidden; i++)
			for (unsigned int j = 0; j < hidden[i].size; j++)
				out.write((char *)&output.neurons[i].bias, sizeof(double));

		std::cout << "Writing  weights" << std::fixed << std::endl;	//weights
		for (unsigned int i = 0; i < input.size; i++)
			for (unsigned int j = 0; j < hidden.front().size; j++)
				out.write((char *)&input.neurons[i].weights[j], sizeof(double));
		for (unsigned int i = 0; i < output.size; i++)
			for (unsigned int j = 0; j < hidden.back().size; j++)
				out.write((char *)&output.neurons[i].weights[j], sizeof(double));
		for (unsigned int i = 0; i < m_Nhidden - 1; i++)
			for (unsigned int j = 0; j < hidden[i].size; ++j)
				for (unsigned int k = 0; k < hidden[i + 1].size; k++)
					out.write((char*)&hidden[i].neurons[j].weights[k],sizeof(double));
		for (unsigned int i = 0; i < hidden.back().size; i++)//Last Hidden Layer
			for (unsigned int j = 0; j < output.size; j++)
				out.write((char*)&hidden.back().neurons[i].weights[j], sizeof(double));
				

		out.close();
	}
}

Nnet Nnet::loadNet(std::string filename)
{
	unsigned int sizeInput;
	unsigned int sizeOutput;
	unsigned int sizeHidden;
	unsigned int m_Nhidden;

	std::ifstream in(filename, std::ios_base::binary);
	if (in.good())
	{
		//Get Nnet sizes
		std::cout << "Reading sizes: " << std::endl;
		in.read((char *)&sizeInput, sizeof(unsigned int));	//input layer size
		in.read((char *)&sizeOutput, sizeof(unsigned int));	//output layer size
		in.read((char *)&sizeHidden, sizeof(unsigned int));	//hidden layers size
		in.read((char *)&m_Nhidden, sizeof(unsigned int));	//number of hidden layers
		std::cout << "Done" << std::endl;

		//Resize Nnet
		Nnet net;
		net.Setup(sizeInput, m_Nhidden, sizeHidden, sizeOutput,true);

		//Load Biases
		std::cout << "Reading  biases: " << std::fixed;//biases
		for (unsigned int i = 0; i < net.input.size; i++)
			in.read((char *)&net.input.neurons[i].bias, sizeof(double));
		for (unsigned int i = 0; i < net.output.size; i++)
			in.read((char *)&net.output.neurons[i].bias, sizeof(double));
		for (unsigned int i = 0; i < net.m_Nhidden; i++)
			for (unsigned int j = 0; j < net.hidden[i].size; j++)
				in.read((char *)&net.output.neurons[i].bias, sizeof(double));
		std::cout << "Done" << std::endl;

		//Load Weights
		std::cout << "Reading  weights: " << std::fixed << std::endl;//weights
		for (unsigned int i = 0; i < net.input.size; i++)
			for (unsigned int j = 0; j < net.hidden.front().size; j++)
				in.read((char *)&net.input.neurons[i].weights[j], sizeof(double));
		for (unsigned int i = 0; i < net.output.size; i++)
			for (unsigned int j = 0; j < net.hidden.back().size; j++)
				in.read((char *)&net.output.neurons[i].weights[j], sizeof(double));
		for (unsigned int i = 0; i < net.m_Nhidden - 1; i++)
			for (unsigned int j = 0; j < net.hidden[i].size; ++j)
				for (unsigned int k = 0; k < net.hidden[i + 1].size; k++)
					in.read((char*)&net.hidden[i].neurons[j].weights[k], sizeof(double));
		for (unsigned int i = 0; i < net.hidden.back().size; i++)//Last Hidden Layer
			for (unsigned int j = 0; j < net.output.size; j++)
				in.read((char*)&net.hidden.back().neurons[i].weights[j], sizeof(double));
		std::cout << "Done" << std::endl;

		*this = net;
		return net;
	}
	return Nnet();//error
}