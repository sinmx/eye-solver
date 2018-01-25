#pragma once
#include "Nnet.h"

struct point
{
	int x, y;
};

class lilNet :
	public Nnet
{
public:
	lilNet(Nnet old)
	{
		getSmalls();
	}

	vector<double> Propigate(vector<double> inputs)
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
			double adder = 0;
			for (unsigned int j = 0; j < input.size; j++) 
			{
				bool pruned = false;
				for (unsigned int p = 0; p < prunes.size(); p++)
					if ((i == prunes[p].x && j == prunes[p].y))
					{
						pruned = true;
						p = prunes.size();
					}
				if (!pruned)
					adder += input.neurons[j].value * input.neurons[j].weights[i];//node*weight
			}
			adder += hidden.front().neurons[i].bias;
			Normalize(adder);
			hidden.front().neurons[i].value = adder;
		}

		//Hidden Layers
		for (unsigned int i = 1; i < m_Nhidden; i++) {
			for (unsigned int j = 0; j < hidden[i].size; j++) 
			{
				double adder = 0;
				for (unsigned int k = 0; k < hidden[i].size; k++)
				{
					bool pruned = false;
					for (unsigned int p = 0; p < prunes.size(); p++)
						if ((i == prunes[p].x && j == prunes[p].y))
						{
							pruned = true;
							p = prunes.size();
						}
					if (!pruned)
						adder += hidden[i - 1].neurons[j].value * hidden[i - 1].neurons[j].weights[k];
				}
					
				adder += hidden[i].neurons[j].bias;
				Normalize(adder);
				hidden[i].neurons[j].value = adder;
			}
		}

		//Output Layer
		out.resize(output.size);
		for (unsigned int i = 0; i < output.size; i++)
		{
			double adder = 0;
			for (unsigned int j = 0; j < m_Nhidden; j++)
			{
				bool pruned = false;
				for (unsigned int p = 0; p < prunes.size(); p++)
					if ((SizeHidden-1 == prunes[p].x && j == prunes[p].y))
					{
						pruned = true;
						p = prunes.size();
					}
				if (!pruned)
					adder += hidden.back().neurons[j].value * output.neurons[i].weights[j];//node*weight
			}
				
			adder += output.neurons[i].bias;
			//Normalize(adder);//normalize
			output.neurons[i].value = adder;
			out[i] = adder;
		}

		PassedProp = (clock() - startProp);// / CLOCKS_PER_SEC;
		return out;
	}



private:
	void getSmalls()
	{
		for (int i = 0; i < Nhiddens; i++)
			for (int j = 0; j < SizeHidden; j++)
				if (hidden[i].neurons[j].value < trate && hidden[i].neurons[j].value > -trate)//if uselessly small
					prunes.push_back({ i,j });
	}

	vector<point> prunes;
};

