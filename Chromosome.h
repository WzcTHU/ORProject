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
	vector<Gene> Sequence;

public:
	Chromosome() : Sequence({}) {};
	~Chromosome() {};
	void CalFitValue();
	void RecordToFile(string filename);
	void BuildInitGreedy();			//用贪心算法构造初始解
	void BuildInitRandom();			//随机构造初始解
	void FTDIH();
	void OTDIH();
};
#endif // !CHROMOSOME_H
