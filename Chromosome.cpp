#include<iostream>
#include<stdlib.h>
#include<algorithm>
#include<time.h>
#include<fstream>
#include"Chromosome.h"
using namespace std;
#define MAX_DIS 100000
#define MAX_NEIGHBOUR 5

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
int GoBack(Node &start_node, Vehicle &this_vehicle, Routine &routine) {
	if (this_vehicle.LeftElec >= DisNN[start_node.ID][0]) {
		//电量足够返回车场
		this_vehicle.TotalDis += DisNN[start_node.ID][0];			//更新行驶距离
		this_vehicle.LeftElec -= DisNN[start_node.ID][0];
		start_node = NodeList[0];
		Gene this_G;
		this_G.vehicle = this_vehicle;
		this_G.node = start_node;
		routine.GeneSequence.push_back(this_G);		//将起始点加入routine
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
			routine.GeneSequence.push_back(this_G);		//将起始点加入routine
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

void Chromosome::WalkGuest(vector<int> VisitedGuest, int start_vehile_ID) {
	srand((unsigned int)time(NULL));
	//vector<int> VisitedGuest = {};		//用于保存已访问的客户ID
	for (int i = start_vehile_ID; i <= VehicleID.size(); i++) {		//对每一辆车进行循环
		if (VisitedGuest.size() == GUEST_NUM) break;
		Node start_node = NodeList[0];		//每一辆车都是从车场出发
		int job_finish_by_time = 0;
		int change_type_2 = 0;				//强制使用大车
		int type = 1;
		int state = -1;			//用于记录当前构造完的routine是否可行

		if (change_type_2 == 0) {
			float dice = rand() / double(RAND_MAX);
			if (VisitedGuest.size() <= (GUEST_NUM - 10)) {
				type = (dice > 0.5) ? 1 : 2;	
			}
			/*else if ((GUEST_NUM - 500) < VisitedGuest.size() <= (GUEST_NUM - 200)) {
				type = (dice > 0.7) ? 1 : 2;	
			}
			else if ((GUEST_NUM - 200) < VisitedGuest.size() <= (GUEST_NUM - 10)) {
				type = (dice > 0.2) ? 1 : 2;	
			}*/
			else {
				type = (dice > 0.01) ? 1 : 2;	//剩余客户较少时，倾向于使用1型车
			}
		}
		else if (change_type_2 == 1) {
			type = 2;
			change_type_2 = 0;				//清空强制转换标记
		}

		Vehicle this_vehicle(type, i);		//实例化选中的车
		//用于保存某辆车形成的所有routine
		vector<Routine> VehicleRoutine = {};
		TravelAgain:

		//vector<Gene> routine = {};			
		//用于保存某辆车形成的回路
		Routine routine;
		Gene this_G;
		this_G.vehicle = this_vehicle;
		this_G.node = start_node;
		routine.GeneSequence.push_back(this_G);		//将起始点加入routine

		//拷贝一个已访问列表，防止某条routine失效时其已访问节点被记录到VisitedGuest中
		vector<int> TempVisitedGuest(VisitedGuest);


		//cout << VisitedGuest.size() << endl;
		//if (VisitedGuest.size() == 1000) {
		//	int stop = 0;
		//}
		while (1) {			//对同一辆车，构造完其送货子序列(routine)
			vector<int>::const_iterator start = DisNN[start_node.ID].begin();
			vector<int>::const_iterator end = start + GUEST_NUM + DEPOT_NUM;
			vector<int> neighbour_guest(start, end);			//截取车场和客户点部分向量(每一行)
			auto TargetGuestID = FindClosest(start_node.ID, neighbour_guest, TempVisitedGuest, 1);		//查找离当前点最近的点ID

			if (TargetGuestID[0] == -1) {
				//即FindClosest()返回-1，所有客户点都已经到达，则执行返回车场操作
				state = GoBack(start_node, this_vehicle, routine);
				job_finish_by_time = 1;				//借用强制退出标记
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
						if ((this_vehicle.LeftElec - DisNN[start_node.ID][early_ID] \
							>= DisNN[early_ID][TargetChargeID]) || (this_vehicle.LeftElec -\
							DisNN[start_node.ID][early_ID] >= DisNN[early_ID][0])) {
							Deliver(start_node, early_ID, this_vehicle);
							this_G.node = start_node;
							this_G.vehicle = this_vehicle;
							routine.GeneSequence.push_back(this_G);							//将当前点加入routine
							TempVisitedGuest.push_back(start_node.ID);			//将当前点加入已访问客户列表
						}
						else {
							state = GoBack(start_node, this_vehicle, routine);
							break;
						}
					}
					else {
						//不能在最晚服务时间之前到达，搜索下一个近邻点，如果所有近邻点都已经搜索过，则回车场
						if (TargetGuestID.size() == 0) {
							//给出由于因时间而结束的标记，该车不再从车场派出
							job_finish_by_time = 1;
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
						routine.GeneSequence.push_back(this_G);		//将当前点加入routine
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
				if (NodeList[early_ID].Volume >= this_vehicle.Volume) {
					job_finish_by_time = 1;			//只是借用这个标记，直接退出当前routine，改用大车
					change_type_2 = 1;
				}
				if (NodeList[early_ID].Weight >= this_vehicle.Weight) {
					job_finish_by_time = 1;			//只是借用这个标记，直接退出当前routine，改用大车
					change_type_2 = 1;
				}
				state = GoBack(start_node, this_vehicle, routine);
				break;
			}
		}
		if (state == 0) {
			//成功返回车场，routine有效，则将其加入Sequence中，并将TempVisitedGuest保存为VisitedGuest
			//Sequence.insert(Sequence.end(), routine.begin(), routine.end());
			VehicleRoutine.push_back(routine);
			VisitedGuest = TempVisitedGuest;
			//只要不是因为到时而结束任务，则回车场重新装货、充电，再次派出
			if (job_finish_by_time == 0) {
				this_vehicle.CurVol = 0;
				this_vehicle.CurWei = 0;
				this_vehicle.CurTime += DEPOT_TIME;
				this_vehicle.LeftElec = this_vehicle.DriveRange;
				this_vehicle.TotalWait += DEPOT_TIME;
				goto TravelAgain;
			}
			else {
				Sequence.push_back(VehicleRoutine);
			}
		}
	}
}

void Chromosome::CalFitValue() {
	int vehicle_ID = 1;
	int Wait = 0;
	int Basic = 0;
	int Charge = 0;
	float Travel = 0;
	int count = 0;
	for (int i = 0; i < Sequence.size() - 1; i++) {
		int routine_num = Sequence[i].size();
		int last_routine_len = Sequence[i][routine_num - 1].GeneSequence.size();
		Vehicle this_vehicle = Sequence[i][routine_num - 1].\
			GeneSequence[last_routine_len - 1].vehicle;
		if (this_vehicle.FirstMark != 0) {
			count++;
			Wait += (this_vehicle.TotalWait * WAIT_COST);
			Basic += (this_vehicle.BasicCost);
			Charge += (this_vehicle.ChargeCount * CHARGE_COST);
			Travel += (this_vehicle.TotalDis * this_vehicle.UnitCost);
		}
	}
	FitValue = Wait + Basic + Charge + Travel;
	//cout << "Total vehicle number: " << count + 1 << endl;
	//cout << "basic cost: " << Basic << endl;
	//cout << "wait cost: " << Wait << endl;
	//cout << "charge cost: " << Charge << endl;
	//cout << "travel cost: " << Travel << endl;
	//cout << "total cost: " << FitValue << endl;
}

//将该染色体信息记录到文件中
void Chromosome::RecordToFile(string filename) {
	ofstream output_file(filename);
	if (output_file.is_open()) {
		output_file << "total cost: " << FitValue << endl;
		for (auto vr : Sequence) {
			for (auto r : vr) {
				for (auto g : r.GeneSequence) {
					output_file << g.vehicle.ID << " " << g.vehicle.type << " " << g.node.ID << endl;
				}
			}
		}
		output_file.close();
	}
}
//还可以考虑优化返回车场途中，经过充电站之后，继续送货