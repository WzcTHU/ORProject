#include<iostream>
#include<stdlib.h>
#include<algorithm>
#include<time.h>
#include"Chromosome.h"
using namespace std;
#define MAX_DIS 100000
#define MAX_NEIGHBOUR 10

vector<int> FindClosest(int start_ID, vector<int> NodeDis, vector<int> VisitedGuest, int is_guest) {
	vector<int> temp_v(NodeDis);
	vector<int> result = {};
	if (is_guest == 1) {
		for (auto ID : VisitedGuest) {			//将已经访问的点距离赋值为最大值，防止被检出
			NodeDis[ID] = MAX_DIS;
			temp_v[ID] = MAX_DIS;
		}
	}
	if ((is_guest == 1) && (start_ID <= GUEST_NUM + DEPOT_NUM - 1)) {	
		//如果是寻找最近客户点，且不是从充电桩出发寻找
		if (start_ID != 0) {
			NodeDis[0] = MAX_DIS;					//将车场排除在外
		}
		NodeDis[start_ID] = MAX_DIS;			//将本身排除在外
	}
	sort(NodeDis.begin(), NodeDis.end());
	int min_dis = NodeDis[0];
	vector<int>::const_iterator start = NodeDis.begin();
	vector<int>::const_iterator end = start + MAX_NEIGHBOUR;
	vector<int> close_neighbour(start, end);
	if (min_dis == MAX_DIS) return { -1 };		//表示所有客户都已访问到
	if (is_guest == 0) {						//若为搜索充电站
		for (int i = 0; i < NodeDis.size(); i++) {
			if (temp_v[i] == min_dis) {
				return { i };
			}
		}
	}
	else {
		//对于客户点，寻找最近的MAX_NEIGHBOUR个点，且已经找过的不列入其中
		for (int i = 0; i < NodeDis.size(); i++) {
			if ((find(close_neighbour.begin(), close_neighbour.end(), temp_v[i]) \
				!= close_neighbour.end()) && (temp_v[i] != MAX_DIS)){
				result.push_back(i);
			}
		}
		return result;
	}
	return { -1 };
}

//对近邻点按照时间进行排序
int SortByTime(vector<int> &close_neighbour) {
	vector<int> time_list = {};
	for (auto ID : close_neighbour) {
		time_list.push_back(NodeList[ID].Et);
	}
	sort(time_list.begin(), time_list.end());
	int early_ID = 0;
	for (int i = 0; i < close_neighbour.size();i++) {
		if (NodeList[close_neighbour[i]].Et == time_list[0]) {
			early_ID = close_neighbour[i];
		}
	}
	for (vector<int>::iterator it = close_neighbour.begin(); it != close_neighbour.end(); ) {
		if (*it == early_ID) {
			close_neighbour.erase(it);
			break;
		}
		it++;
	}
	return early_ID;
}

//充电函数
void Charge(Node &start_node, int TargetChargeID, Vehicle &this_vehicle) {
	this_vehicle.CurTime += TimeNN[start_node.ID][TargetChargeID] + CHARGE_TIME;	//更新当前时间
	this_vehicle.ChargeCount++;									//充电次数加一
	this_vehicle.LeftElec = this_vehicle.DriveRange;			//将电充满
	this_vehicle.TotalDis += DisNN[start_node.ID][TargetChargeID];		//更新行驶距离
	start_node = NodeList[TargetChargeID];						//更新起始点为当前充电桩
}

//回车场函数
int GoBack(Node &start_node, Vehicle &this_vehicle, vector<Gene> &routine) {
	if (this_vehicle.LeftElec >= DisNN[start_node.ID][0]) {
		//电量足够返回车场
		this_vehicle.TotalDis += DisNN[start_node.ID][0];			//更新行驶距离
		start_node = NodeList[0];
		Gene this_G;
		this_G.vehicle = this_vehicle;
		this_G.node = start_node;
		routine.push_back(this_G);		//将起始点加入routine
		return 0;
	}
	else {
		//电量不足以返回车场，寻找最近充电桩
		vector<int>::const_iterator start = DisNN[start_node.ID].begin() + GUEST_NUM + DEPOT_NUM;
		vector<int>::const_iterator end = start + CHARGE_NUM;
		vector<int> neighbour_charge(start, end);			//截取充电桩向量(每一行)
		int TargetChargeID = FindClosest(start_node.ID, neighbour_charge, {}, 0)[0] + GUEST_NUM + DEPOT_NUM;
		if (this_vehicle.LeftElec >= DisNN[start_node.ID][TargetChargeID]) {
			//如果剩余电量足够到达最近充电桩，则执行充电操作
			Charge(start_node, TargetChargeID, this_vehicle);
			//充电完毕返回车场
			Gene this_G;
			this_G.vehicle = this_vehicle;
			this_G.node = start_node;
			routine.push_back(this_G);		//将起始点加入routine
			GoBack(NodeList[TargetChargeID], this_vehicle, routine);
		}
		else {
			//返回车场失败
			return -1;
		}
	}
}


//送货函数
void Deliver(Node &start_node, int TargetGuestID, Vehicle &this_vehicle) {
	if (this_vehicle.CurTime + TimeNN[start_node.ID][TargetGuestID] \
		>= NodeList[TargetGuestID].Et) {
		//在最早服务时间之后到达，则不需要等待直接卸货
		this_vehicle.CurTime += (TimeNN[start_node.ID][TargetGuestID] + UNLOAD_TIME);	//计算新的时间
	}
	else {
		//在最早服务时间之前到达，需要等待
		if (this_vehicle.FirstMark == 0) {
			//说明当前客户点是车辆第一个到达的客户点，可以通过晚出发来实现不等待,只需更新标记即可
			this_vehicle.FirstMark = 1;
		}
		else {    //说明当前客户点不是车辆第一个到达的客户点，需要计算等待时间
			this_vehicle.TotalWait += (NodeList[TargetGuestID].Et - \
				(this_vehicle.CurTime + TimeNN[start_node.ID][TargetGuestID]));		//更新等待时间
		}
		this_vehicle.CurTime = (NodeList[TargetGuestID].Et + UNLOAD_TIME);	//计算新的时间
	}
	this_vehicle.TotalDis += DisNN[start_node.ID][TargetGuestID];		//更新行驶距离
	this_vehicle.CurVol += NodeList[TargetGuestID].Volume;		//计算当前货物体积
	this_vehicle.CurWei += NodeList[TargetGuestID].Weight;		//计算当前货物重量
	this_vehicle.LeftElec -= DisNN[start_node.ID][TargetGuestID]; //电量减少
	start_node = NodeList[TargetGuestID];			//更新起始点为当前客户点
}

void Chromosome::BuildInitGreedy() {
	srand((unsigned int)time(NULL));
	vector<int> VisitedGuest = {};		//用于保存已访问的客户ID
	for (auto i : VehicleID) {		//对每一辆车进行循环
		if (VisitedGuest.size() == GUEST_NUM) break;
		Node start_node = NodeList[0];		//每一辆车都是从车场出发
		float dice = rand() / double(RAND_MAX);
		int type = (dice > 0.5) ? 1 : 2;	//以0.5的概率从1，2类型里面选择一种车
		Vehicle this_vehicle(type, i);		//实例化选中的车
		vector<Gene> routine = {};			//用于保存某辆车形成的回路

		int state = 0;			//用于记录当前构造完的routine是否可行

		Gene this_G;
		this_G.vehicle = this_vehicle;
		this_G.node = start_node;
		routine.push_back(this_G);		//将起始点加入routine

		//拷贝一个已访问列表，防止某条routine失效时其已访问节点被记录到VisitedGuest中
		vector<int> TempVisitedGuest(VisitedGuest);
		while (1) {			//对同一辆车，构造完其送货子序列(routine)
			vector<int>::const_iterator start = DisNN[start_node.ID].begin();
			vector<int>::const_iterator end = start + GUEST_NUM + DEPOT_NUM;
			vector<int> neighbour_guest(start, end);			//截取车场和客户点部分向量(每一行)
			auto TargetGuestID = FindClosest(start_node.ID, neighbour_guest, TempVisitedGuest, 1);		//查找离当前点最近的点ID

			if (TargetGuestID[0] == -1) {
				//即FindClosest()返回-1，所有客户点都已经到达，则执行返回车场操作
				state = GoBack(start_node, this_vehicle, routine);
				break;
			}
			
		SearchAnotherNeighbour:	
			int early_ID = SortByTime(TargetGuestID);
			if((this_vehicle.CurVol + NodeList[early_ID].Volume <= this_vehicle.Volume) && \
			   (this_vehicle.CurWei + NodeList[early_ID].Weight <= this_vehicle.Weight)){
				//如果到下一个点还有足够容量，则执行其它条件判断
				if (this_vehicle.LeftElec >= DisNN[start_node.ID][early_ID]) {
					//判断电量是否足够到下一个点
					if (this_vehicle.CurTime + TimeNN[start_node.ID][early_ID] \
						<= NodeList[early_ID].Lt) {
						//判断是否能在最晚服务时间之前到达
						//电量充足的情况下查看剩余电量，如果执行完这次操作不能到达任何一个电站，
						//或是不能直接回车场，就不执行这次操作，直接返回车场，否则执行送货
						vector<int>::const_iterator start = DisNN[early_ID].begin() + GUEST_NUM + DEPOT_NUM;
						vector<int>::const_iterator end = start + CHARGE_NUM;
						vector<int> neighbour_charge(start, end);			//截取充电桩向量(每一行)
						int TargetChargeID = FindClosest(early_ID, neighbour_charge, {}, 0)[0] + GUEST_NUM + DEPOT_NUM;
						if (this_vehicle.LeftElec - DisNN[start_node.ID][early_ID] >= DisNN[early_ID][TargetChargeID]) {
							Deliver(start_node, early_ID, this_vehicle);
							this_G.node = start_node;
							this_G.vehicle = this_vehicle;
							routine.push_back(this_G);							//将当前点加入routine
							TempVisitedGuest.push_back(start_node.ID);			//将当前点加入已访问客户列表
						}
						else {
							state = GoBack(start_node, this_vehicle, routine);
						}
					}
					else {
						//不能在最晚服务时间之前到达，搜索下一个近邻点，如果所有近邻点都已经搜索过，则回车场
						if (TargetGuestID.size() == 0) {
							state = GoBack(start_node, this_vehicle, routine);
							break;
						}
						else {
							goto SearchAnotherNeighbour;
						}
					}
					
				}
				else {
					//电量不足以到达下一个点，则搜索最近充电站
					vector<int>::const_iterator start = DisNN[start_node.ID].begin() + GUEST_NUM + DEPOT_NUM;
					vector<int>::const_iterator end = start + CHARGE_NUM;
					vector<int> neighbour_charge(start, end);			//截取充电桩向量(每一行)
					int TargetChargeID = FindClosest(start_node.ID, neighbour_charge, {}, 0)[0] + GUEST_NUM + DEPOT_NUM;
					if (this_vehicle.LeftElec >= DisNN[start_node.ID][TargetChargeID]) {
						//如果剩余电量足够到达最近充电桩，则执行充电操作
						Charge(start_node, TargetChargeID, this_vehicle);
						this_G.node = start_node;
						this_G.vehicle = this_vehicle;
						routine.push_back(this_G);		//将当前点加入routine
					}
					else {
						//否则，返回车场
						state = GoBack(start_node, this_vehicle, routine);
						break;
					}
				}
			}
			//还需要考虑通过充电站，如果到不了充电站则进行能否回到车场检测，若回不去这条routine直接作废
			//由于最晚的服务时间是15：30，而从任意客户点回到车场最长只需要95分钟，因此只要电量足够，一定可以按时返回车场，
			//不需要对车场关闭时间进行额外判断
			else {	
				//容量不足，回车场
				state = GoBack(start_node, this_vehicle, routine);
				break;
			}
		}
		if (state == 0) {
			//成功返回车场，routine有效，则将其加入Sequence中，并将TempVisitedGuest保存为VisitedGuest
			Sequence.insert(Sequence.end(), routine.begin(), routine.end());
			VisitedGuest = TempVisitedGuest;
		}
	}
}

//还可以考虑优化返回车场途中，经过充电站之后，继续送货