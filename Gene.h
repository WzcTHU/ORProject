#pragma once
#ifndef GENE_H
#define GENE_H
#include"Basic.h"

class Gene {
public:
	Node node;
	Vehicle vehicle;		//�Ըýڵ����ĳ�ID
public:
	Gene() {};
	~Gene() {};
};

class Routine {
public:
	vector<Gene> GeneSequence;
public:
	Routine() : GeneSequence({}) {};
	~Routine() {};
};
#endif // !GENE_H
