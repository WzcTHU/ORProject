#pragma once
#ifndef GAAPP_H
#define GAAPP_H
#include"Chromosome.h"

class GAApp {
public:
	vector<Chromosome> population;
public:
	GAApp() {};
	~GAApp() {};
	void InitAll();
	void MakeInitPopulation(int greedy_num, int random_num);
	void Run();
};
#endif // !GAAPP_H
