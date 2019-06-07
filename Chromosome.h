#pragma once
#ifndef CHROMOSOME_H
#define CHROMOSOME_H
#include<vector>
#include<string>
#include"Gene.h"
using std::vector;
using std::string;

class Chromosome {
public:
	int FitValue;		//适应度
	vector<vector<Routine>> Sequence;

public:
	Chromosome() : Sequence({}) {};
	~Chromosome() {};
	void CalFitValue(int show_detial = 0);
	void RecordToFile(string filename);
	void WalkGuest(vector<int> VisitedGuest, int start_vehile_ID, int add = 0);			//用贪心算法构造初始解
};
#endif // !CHROMOSOME_H
