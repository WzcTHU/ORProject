#include<fstream>
#include<iostream>
#include"Basic.h"
using namespace std;
int total_num = DEPOT_NUM + GUEST_NUM + CHARGE_NUM;
vector<vector<int>> DisNN(total_num, vector<int>(total_num, 0));		//任意两点间距离矩阵
vector<vector<int>> TimeNN(total_num, vector<int>(total_num, 0));		//任意两点间时间矩阵
vector<int> VehicleID;
vector<int> NodeID;
vector<Node> NodeList;			//用于保存所有节点信息


void InitNN(string filename) {
	ifstream input_file(filename);
	cout << "Opening file: " << filename << endl;
	if (!input_file.is_open()) {
		cout << "Failed while opening the" << filename << endl;
	}
	int data_ID = 0;
	int start_ID = 0;
	int end_ID = 0;
	int d = 0;
	int t = 0;
	for (int i = 0; i < total_num; i++) {
		for (int j = 0; j < total_num; j++) {
			if (i != j) {
				input_file >> data_ID >> start_ID >> end_ID >> d >> t;
				DisNN[i][j] = d;
				TimeNN[i][j] = t;
			}
			else {
				DisNN[i][j] = 0;
				TimeNN[i][j] = 0;
			}
		}
	}
	input_file.close();
}

void InitVehicleID() {
	for (int i = 0; i < MAX_VEHICLE; i++) {
		VehicleID.push_back(i + 1);
	}
}
void InitNodeID() {
	for (int i = 0; i < (GUEST_NUM + CHARGE_NUM + DEPOT_NUM); i++) {
		NodeID.push_back(i);
	}
}

void InitNodeList(string filename) {
	//从文件读入所有节点信息
	ifstream input_file(filename);
	cout << "Opening file: " << filename << endl;
	if (!input_file.is_open()) {
		cout << "Failed while opening the" << filename << endl;
	}
	int ID = 0;
	int type = 0;
	int Et = 0;
	int Lt = 0;
	float Weight = 0;
	float Volume = 0;
	for (int i = 0; i < total_num; i++) {
		input_file >> ID >> type >> Weight >> Volume >> Et >> Lt;
		Node m_node(type, ID);
		m_node.Weight = Weight;
		m_node.Volume = Volume;
		m_node.Et = Et;
		m_node.Lt = Lt;
		NodeList.push_back(m_node);
	}
	input_file.close();
}