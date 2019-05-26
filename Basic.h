#pragma once
#ifndef BASIC_H
#define BASIC_H
#define UNLOAD_TIME 0.5 * 60
#define CHARGE_TIME 0.5 * 60
#define GUEST_NUM 1000
#define CHARGE_NUM 100
#define DEPOT_NUM 1
#define MAX_VEHICLE 1000
#include<vector>
#include<string>
using std::vector;
using std::string;

class Node {
public:
	int type;
	int ID;
	int Et;
	int Lt;
	float Weight;
	float Volume;
public:
	Node(int m_type, int m_ID) {
		type = m_type;
		ID = m_ID;
	};
	Node() {};
	~Node() {};
};

class Vehicle {
public:
	int ID;
	int type;
	float Volume;
	float Weight;
	float CurVol;			//用于记录当前累计装载体积
	float CurWei;			//用于记录当前累计装载重量
	int DriveRange;
	float UnitCost;
	float BasicCost;
	int LeftElec;			//用于记录当前剩余电量
	int TotalDis;			//用于记录当前累计行驶路程
	int TotalWait;			//用于当前总等待时间
	int ChargeCount;		//用于记录充电次数
	int CurTime;			//用于记录当前时间
	int FirstMark;			//用于标记当前车辆是否已经到达过至少一个客户点

public:
	Vehicle() {};
	Vehicle(int m_type, int m_ID) {
		ID = m_ID;
		type = m_type;
		if (type == 1) {
			Volume = 12.0;
			Weight = 2.0;
			DriveRange = 100000;
			UnitCost = 0.012;
			BasicCost = 200;
		}
		if (type == 2) {
			Volume = 16.0;
			Weight = 2.5;
			DriveRange = 120000;
			UnitCost = 0.014;
			BasicCost = 300;
		}
		LeftElec = DriveRange;
		CurVol = 0.0;
		CurWei = 0.0;
		TotalDis = 0;
		TotalWait = 0;
		ChargeCount = 0;
		CurTime = 8 * 60;
		FirstMark = 0;
	};
	~Vehicle() {};
};
void InitNN(string filename);
void InitVehicleID();
void InitNodeID();
void InitNodeList(string filename);

extern vector<vector<int>> DisNN;		//任意两点间距离矩阵
extern vector<vector<int>> TimeNN;		//任意两点间时间矩阵
extern vector<int> VehicleID;
extern vector<int> NodeID;
extern vector<Node> NodeList;			//用于保存所有节点信息
#endif // !BASIC_H
