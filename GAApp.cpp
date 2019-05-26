#include<iostream>
#include<sstream>
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
		cout << "Num: " << i + 1 << endl;
		Chromosome m_chro;
		//stringstream ss1;
		//ss1 << i;
		stringstream ss2;
		m_chro.BuildInitGreedy();
		population.push_back(m_chro);
		m_chro.CalFitValue();
		ss2 << m_chro.FitValue;
		m_chro.RecordToFile("population\\chro_FitValue_" + ss2.str() + ".txt");
	}
	//使用随机算法生成一批染色体
}

void GAApp::Run() {
	InitAll();
	MakeInitPopulation(10, 0);
	//for (auto gene : population[0].Sequence) {
	//	cout << gene.vehicle.ID << " " << gene.vehicle.type << " " << gene.vehicle.TotalDis << " " << gene.node.ID << endl;
	//}
	//population[0].CalFitValue();
	//cout << population[0].FitValue << endl;
}