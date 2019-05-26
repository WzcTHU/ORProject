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
	int FitValue;		//��Ӧ��
	vector<Gene> Sequence;

public:
	Chromosome() : Sequence({}) {};
	~Chromosome() {};
	void CalFitValue();
	void RecordToFile(string filename);
	void BuildInitGreedy();			//��̰���㷨�����ʼ��
	void BuildInitRandom();			//��������ʼ��
	void FTDIH();
	void OTDIH();
};
#endif // !CHROMOSOME_H
