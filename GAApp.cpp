#include<iostream>
#include"GAApp.h"
using namespace std;

void GAApp::InitAll() {
	InitNN("input_distance-time.txt");
	InitNodeList("input_node.txt");
	InitNodeID();
	InitVehicleID();
}

void GAApp::MakeInitPopulation(int greedy_num, int random_num) {
	//使用贪心算法生成一批染色体
	cout << "Generaing initial population..." << endl;
	for (int i = 0; i < greedy_num; i++) {
		Chromosome m_chro;
		m_chro.BuildInitGreedy();
		population.push_back(m_chro);
	}
	//使用随机算法生成一批染色体
}

void GAApp::Run() {
	InitAll();
	MakeInitPopulation(1, 0);
	for (auto gene : population[0].Sequence) {
		cout << gene.vehicle.ID << " " << gene.vehicle.type << " " << gene.node.ID << endl;
	}
}