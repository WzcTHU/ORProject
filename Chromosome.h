#pragma once
#ifndef CHROMOSOME_H
#define CHROMOSOME_H
#include<vector>
#include"Gene.h"
using std::vector;

class Chromosome {
public:
	int FitValue;		//��Ӧ��
	vector<Gene> Sequence;

public:
	Chromosome() : Sequence({}) {};
	~Chromosome() {};
	void CalFitValue();
	void BuildInitGreedy();			//��̰���㷨�����ʼ��
	void BuildInitRandom();			//��������ʼ��
	void FTDIH();
	void OTDIH();
};
#endif // !CHROMOSOME_H
