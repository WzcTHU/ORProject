#pragma once
#ifndef BASIC_H
#define BASIC_H
#define UNLOAD_TIME 0.5 * 60
#define CHARGE_TIME 0.5 * 60
#define DEPOT_TIME 60
#define GUEST_NUM 1000
#define CHARGE_NUM 100
#define DEPOT_NUM 1
#define MAX_VEHICLE 10000
#define WAIT_COST 24 / 60
#define CHARGE_COST 50    //һ�γ���Сʱ��100RMB/h������Ϊ�˷���ʹ�ó��������㣬�س�����粻�Ƴɱ�
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
	float CurVol;			//���ڼ�¼��ǰ�ۼ�װ�����
	float CurWei;			//���ڼ�¼��ǰ�ۼ�װ������
	int DriveRange;
	float UnitCost;
	float BasicCost;
	int LeftElec;			//���ڼ�¼��ǰʣ�����
	int TotalDis;			//���ڼ�¼��ǰ�ۼ���ʻ·��
	int TotalWait;			//���ڵ�ǰ�ܵȴ�ʱ��
	int ChargeCount;		//���ڼ�¼������
	int CurTime;			//���ڼ�¼��ǰʱ��
	int FirstMark;			//���ڱ�ǵ�ǰ�����Ƿ��Ѿ����������һ���ͻ���

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
void InitGL();

extern vector<vector<int>> DisNN;		//���������������
extern vector<vector<int>> TimeNN;		//���������ʱ�����
extern vector<int> VehicleID;
extern vector<int> NodeID;
extern vector<Node> NodeList;			//���ڱ������нڵ���Ϣ
extern vector<int> AllGuestList;
#endif // !BASIC_H
