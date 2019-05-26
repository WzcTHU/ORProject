#pragma once
#ifndef GENE_H
#define GENE_H
#include"Basic.h"

class Gene {
public:
	Node node;
	Vehicle vehicle;		//对该节点服务的车ID
public:
	Gene() {};
	~Gene() {};
};
#endif // !GENE_H
