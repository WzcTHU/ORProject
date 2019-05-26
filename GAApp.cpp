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
	//ʹ��̰���㷨����һ��Ⱦɫ��
	cout << "Generaing initial population..." << endl;
	for (int i = 0; i < greedy_num; i++) {
		Chromosome m_chro;
		m_chro.BuildInitGreedy();
		population.push_back(m_chro);
	}
	//ʹ������㷨����һ��Ⱦɫ��
}

void GAApp::Run() {
	InitAll();
	MakeInitPopulation(1, 0);
	for (auto gene : population[0].Sequence) {
		cout << gene.vehicle.ID << " " << gene.vehicle.type << " " << gene.node.ID << endl;
	}
}