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
		for (auto ID : VisitedGuest) {			//���Ѿ����ʵĵ���븳ֵΪ���ֵ����ֹ�����
			NodeDis[ID] = MAX_DIS;
			temp_v[ID] = MAX_DIS;
		}
	}
	if ((is_guest == 1) && (start_ID <= GUEST_NUM + DEPOT_NUM - 1)) {	
		//�����Ѱ������ͻ��㣬�Ҳ��Ǵӳ��׮����Ѱ��
		if (start_ID != 0) {
			NodeDis[0] = MAX_DIS;					//�������ų�����
		}
		NodeDis[start_ID] = MAX_DIS;			//�������ų�����
	}
	sort(NodeDis.begin(), NodeDis.end());
	int min_dis = NodeDis[0];
	vector<int>::const_iterator start = NodeDis.begin();
	vector<int>::const_iterator end = start + MAX_NEIGHBOUR;
	vector<int> close_neighbour(start, end);
	if (min_dis == MAX_DIS) return { -1 };		//��ʾ���пͻ����ѷ��ʵ�
	if (is_guest == 0) {						//��Ϊ�������վ
		for (int i = 0; i < NodeDis.size(); i++) {
			if (temp_v[i] == min_dis) {
				return { i };
			}
		}
	}
	else {
		//���ڿͻ��㣬Ѱ�������MAX_NEIGHBOUR���㣬���Ѿ��ҹ��Ĳ���������
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

//�Խ��ڵ㰴��ʱ���������
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

//��纯��
void Charge(Node &start_node, int TargetChargeID, Vehicle &this_vehicle) {
	this_vehicle.CurTime += TimeNN[start_node.ID][TargetChargeID] + CHARGE_TIME;	//���µ�ǰʱ��
	this_vehicle.ChargeCount++;									//��������һ
	this_vehicle.LeftElec = this_vehicle.DriveRange;			//�������
	this_vehicle.TotalDis += DisNN[start_node.ID][TargetChargeID];		//������ʻ����
	start_node = NodeList[TargetChargeID];						//������ʼ��Ϊ��ǰ���׮
}

//�س�������
int GoBack(Node &start_node, Vehicle &this_vehicle, Routine &routine) {
	if (this_vehicle.LeftElec >= DisNN[start_node.ID][0]) {
		//�����㹻���س���
		this_vehicle.TotalDis += DisNN[start_node.ID][0];			//������ʻ����
		this_vehicle.LeftElec -= DisNN[start_node.ID][0];
		start_node = NodeList[0];
		Gene this_G;
		this_G.vehicle = this_vehicle;
		this_G.node = start_node;
		routine.GeneSequence.push_back(this_G);		//����ʼ�����routine
		return 0;
	}
	else {
		//���������Է��س�����Ѱ��������׮
		vector<int>::const_iterator start = DisNN[start_node.ID].begin() + GUEST_NUM + DEPOT_NUM;
		vector<int>::const_iterator end = start + CHARGE_NUM;
		vector<int> neighbour_charge(start, end);			//��ȡ���׮����(ÿһ��)
		int TargetChargeID = FindClosest(start_node.ID, neighbour_charge, {}, 0)[0] + GUEST_NUM + DEPOT_NUM;
		if (this_vehicle.LeftElec >= DisNN[start_node.ID][TargetChargeID]) {
			//���ʣ������㹻����������׮����ִ�г�����
			Charge(start_node, TargetChargeID, this_vehicle);
			//�����Ϸ��س���
			Gene this_G;
			this_G.vehicle = this_vehicle;
			this_G.node = start_node;
			routine.GeneSequence.push_back(this_G);		//����ʼ�����routine
			GoBack(NodeList[TargetChargeID], this_vehicle, routine);
		}
		else {
			//���س���ʧ��
			return -1;
		}
	}
}


//�ͻ�����
void Deliver(Node &start_node, int TargetGuestID, Vehicle &this_vehicle) {
	if (this_vehicle.CurTime + TimeNN[start_node.ID][TargetGuestID] \
		>= NodeList[TargetGuestID].Et) {
		//���������ʱ��֮�󵽴����Ҫ�ȴ�ֱ��ж��
		this_vehicle.CurTime += (TimeNN[start_node.ID][TargetGuestID] + UNLOAD_TIME);	//�����µ�ʱ��
	}
	else {
		//���������ʱ��֮ǰ�����Ҫ�ȴ�
		if (this_vehicle.FirstMark == 0) {
			//˵����ǰ�ͻ����ǳ�����һ������Ŀͻ��㣬����ͨ���������ʵ�ֲ��ȴ�,ֻ����±�Ǽ���
			this_vehicle.FirstMark = 1;
		}
		else {    //˵����ǰ�ͻ��㲻�ǳ�����һ������Ŀͻ��㣬��Ҫ����ȴ�ʱ��
			this_vehicle.TotalWait += (NodeList[TargetGuestID].Et - \
				(this_vehicle.CurTime + TimeNN[start_node.ID][TargetGuestID]));		//���µȴ�ʱ��
		}
		this_vehicle.CurTime = (NodeList[TargetGuestID].Et + UNLOAD_TIME);	//�����µ�ʱ��
	}
	this_vehicle.TotalDis += DisNN[start_node.ID][TargetGuestID];		//������ʻ����
	this_vehicle.CurVol += NodeList[TargetGuestID].Volume;		//���㵱ǰ�������
	this_vehicle.CurWei += NodeList[TargetGuestID].Weight;		//���㵱ǰ��������
	this_vehicle.LeftElec -= DisNN[start_node.ID][TargetGuestID]; //��������
	start_node = NodeList[TargetGuestID];			//������ʼ��Ϊ��ǰ�ͻ���
}

void Chromosome::WalkGuest(vector<int> VisitedGuest, int start_vehile_ID) {
	srand((unsigned int)time(NULL));
	//vector<int> VisitedGuest = {};		//���ڱ����ѷ��ʵĿͻ�ID
	for (int i = start_vehile_ID; i <= VehicleID.size(); i++) {		//��ÿһ��������ѭ��
		if (VisitedGuest.size() == GUEST_NUM) break;
		Node start_node = NodeList[0];		//ÿһ�������Ǵӳ�������
		int job_finish_by_time = 0;
		int change_type_2 = 0;				//ǿ��ʹ�ô�
		int type = 1;
		int state = -1;			//���ڼ�¼��ǰ�������routine�Ƿ����

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
				type = (dice > 0.01) ? 1 : 2;	//ʣ��ͻ�����ʱ��������ʹ��1�ͳ�
			}
		}
		else if (change_type_2 == 1) {
			type = 2;
			change_type_2 = 0;				//���ǿ��ת�����
		}

		Vehicle this_vehicle(type, i);		//ʵ����ѡ�еĳ�
		//���ڱ���ĳ�����γɵ�����routine
		vector<Routine> VehicleRoutine = {};
		TravelAgain:

		//vector<Gene> routine = {};			
		//���ڱ���ĳ�����γɵĻ�·
		Routine routine;
		Gene this_G;
		this_G.vehicle = this_vehicle;
		this_G.node = start_node;
		routine.GeneSequence.push_back(this_G);		//����ʼ�����routine

		//����һ���ѷ����б���ֹĳ��routineʧЧʱ���ѷ��ʽڵ㱻��¼��VisitedGuest��
		vector<int> TempVisitedGuest(VisitedGuest);


		//cout << VisitedGuest.size() << endl;
		//if (VisitedGuest.size() == 1000) {
		//	int stop = 0;
		//}
		while (1) {			//��ͬһ���������������ͻ�������(routine)
			vector<int>::const_iterator start = DisNN[start_node.ID].begin();
			vector<int>::const_iterator end = start + GUEST_NUM + DEPOT_NUM;
			vector<int> neighbour_guest(start, end);			//��ȡ�����Ϳͻ��㲿������(ÿһ��)
			auto TargetGuestID = FindClosest(start_node.ID, neighbour_guest, TempVisitedGuest, 1);		//�����뵱ǰ������ĵ�ID

			if (TargetGuestID[0] == -1) {
				//��FindClosest()����-1�����пͻ��㶼�Ѿ������ִ�з��س�������
				state = GoBack(start_node, this_vehicle, routine);
				job_finish_by_time = 1;				//����ǿ���˳����
				break;
			}
			
		SearchAnotherNeighbour:	
			int early_ID = SortByTime(TargetGuestID);
			if((this_vehicle.CurVol + NodeList[early_ID].Volume <= this_vehicle.Volume) && \
			   (this_vehicle.CurWei + NodeList[early_ID].Weight <= this_vehicle.Weight)){
				//�������һ���㻹���㹻��������ִ�����������ж�
				if (this_vehicle.LeftElec >= DisNN[start_node.ID][early_ID]) {
					//�жϵ����Ƿ��㹻����һ����
					if (this_vehicle.CurTime + TimeNN[start_node.ID][early_ID] \
						<= NodeList[early_ID].Lt) {
						//�ж��Ƿ������������ʱ��֮ǰ����
						//�������������²鿴ʣ����������ִ������β������ܵ����κ�һ����վ��
						//���ǲ���ֱ�ӻس������Ͳ�ִ����β�����ֱ�ӷ��س���������ִ���ͻ�
						vector<int>::const_iterator start = DisNN[early_ID].begin() + GUEST_NUM + DEPOT_NUM;
						vector<int>::const_iterator end = start + CHARGE_NUM;
						vector<int> neighbour_charge(start, end);			//��ȡ���׮����(ÿһ��)
						int TargetChargeID = FindClosest(early_ID, neighbour_charge, {}, 0)[0] + GUEST_NUM + DEPOT_NUM;
						if ((this_vehicle.LeftElec - DisNN[start_node.ID][early_ID] \
							>= DisNN[early_ID][TargetChargeID]) || (this_vehicle.LeftElec -\
							DisNN[start_node.ID][early_ID] >= DisNN[early_ID][0])) {
							Deliver(start_node, early_ID, this_vehicle);
							this_G.node = start_node;
							this_G.vehicle = this_vehicle;
							routine.GeneSequence.push_back(this_G);							//����ǰ�����routine
							TempVisitedGuest.push_back(start_node.ID);			//����ǰ������ѷ��ʿͻ��б�
						}
						else {
							state = GoBack(start_node, this_vehicle, routine);
							break;
						}
					}
					else {
						//�������������ʱ��֮ǰ���������һ�����ڵ㣬������н��ڵ㶼�Ѿ�����������س���
						if (TargetGuestID.size() == 0) {
							//����������ʱ��������ı�ǣ��ó����ٴӳ����ɳ�
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
					//���������Ե�����һ���㣬������������վ
					vector<int>::const_iterator start = DisNN[start_node.ID].begin() + GUEST_NUM + DEPOT_NUM;
					vector<int>::const_iterator end = start + CHARGE_NUM;
					vector<int> neighbour_charge(start, end);			//��ȡ���׮����(ÿһ��)
					int TargetChargeID = FindClosest(start_node.ID, neighbour_charge, {}, 0)[0] + GUEST_NUM + DEPOT_NUM;
					if (this_vehicle.LeftElec >= DisNN[start_node.ID][TargetChargeID]) {
						//���ʣ������㹻����������׮����ִ�г�����
						Charge(start_node, TargetChargeID, this_vehicle);
						this_G.node = start_node;
						this_G.vehicle = this_vehicle;
						routine.GeneSequence.push_back(this_G);		//����ǰ�����routine
					}
					else {
						//���򣬷��س���
						state = GoBack(start_node, this_vehicle, routine);
						break;
					}
				}
			}
			//����Ҫ����ͨ�����վ����������˳��վ������ܷ�ص�������⣬���ز�ȥ����routineֱ������
			//��������ķ���ʱ����15��30����������ͻ���ص������ֻ��Ҫ95���ӣ����ֻҪ�����㹻��һ�����԰�ʱ���س�����
			//����Ҫ�Գ����ر�ʱ����ж����ж�
			else {	
				//�������㣬�س���
				if (NodeList[early_ID].Volume >= this_vehicle.Volume) {
					job_finish_by_time = 1;			//ֻ�ǽ��������ǣ�ֱ���˳���ǰroutine�����ô�
					change_type_2 = 1;
				}
				if (NodeList[early_ID].Weight >= this_vehicle.Weight) {
					job_finish_by_time = 1;			//ֻ�ǽ��������ǣ�ֱ���˳���ǰroutine�����ô�
					change_type_2 = 1;
				}
				state = GoBack(start_node, this_vehicle, routine);
				break;
			}
		}
		if (state == 0) {
			//�ɹ����س�����routine��Ч���������Sequence�У�����TempVisitedGuest����ΪVisitedGuest
			//Sequence.insert(Sequence.end(), routine.begin(), routine.end());
			VehicleRoutine.push_back(routine);
			VisitedGuest = TempVisitedGuest;
			//ֻҪ������Ϊ��ʱ������������س�������װ������磬�ٴ��ɳ�
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

//����Ⱦɫ����Ϣ��¼���ļ���
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
//�����Կ����Ż����س���;�У��������վ֮�󣬼����ͻ�