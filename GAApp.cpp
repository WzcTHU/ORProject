#include<iostream>
#include<sstream>
#include<time.h>
#include<algorithm>
#include<conio.h>
#include"GAApp.h"
#define CROSS_ITERATION 20000				//��Ⱥ�������
#define IDENTITY_CROSS_NUM 1			//ÿ��Ⱦɫ�彻�����
#define POPULATION_NUM 100
using namespace std;

void GAApp::InitAll() {
	InitNN("input_distance-time.txt");
	InitNodeList("input_node.txt");
	InitGL();
	InitNodeID();
	InitVehicleID();
}

void GAApp::MakeInitPopulation(int greedy_num, int random_num) {
	//ʹ��̰���㷨����һ��Ⱦɫ��
	cout << "Generaing initial population..." << endl;
	for (int i = 0; i < greedy_num; i++) {
		cout << "Num: " << i + 1 << endl;
		Chromosome m_chro;
		//stringstream ss1;
		//ss1 << i;
		stringstream ss2;
		m_chro.WalkGuest({}, 1);		//�ѷ����б�Ϊ�գ���1�ų���ʼ��
		population.push_back(m_chro);
		m_chro.CalFitValue(1);
		ss2 << m_chro.FitValue;
		m_chro.RecordToFile("population\\chro_FitValue_" + ss2.str() + ".txt");
	}
	//ʹ������㷨����һ��Ⱦɫ��
}

vector<Chromosome> GAApp::CrossOver(Chromosome c1, Chromosome c2) {
	srand((unsigned int)time(NULL));
	float dice1 = rand() / double(RAND_MAX);
	float dice2 = rand() / double(RAND_MAX);
	int c1_cross = int(dice1 * (c1.Sequence.size() - 1));
	int c2_cross = int(dice2 * (c2.Sequence.size() - 1));
	vector<int> c1_cross_guest = {};
	vector<int> c2_cross_guest = {};
	Chromosome sub_c1 = c1;
	Chromosome sub_c2 = c2;
	int len_c1 = sub_c1.Sequence.size() + 1;
	int len_c2 = sub_c2.Sequence.size() + 1;
	//����Ƭ��
	sub_c1.Sequence[c1_cross] = c2.Sequence[c2_cross];
	sub_c2.Sequence[c2_cross] = c1.Sequence[c1_cross];
	for (auto r : c1.Sequence[c1_cross]) {
		for (auto g : r.GeneSequence) {
			if (g.node.type == 2) {
				c1_cross_guest.push_back(g.node.ID);
			}
		}
	}
	for (auto r : c2.Sequence[c2_cross]) {
		for (auto g : r.GeneSequence) {
			if (g.node.type == 2) {
				c2_cross_guest.push_back(g.node.ID);
			}
		}
	}
	sort(c1_cross_guest.begin(), c1_cross_guest.end());
	sort(c2_cross_guest.begin(), c2_cross_guest.end());

	vector<int> same_node = {};
	vector<int> c1_diff = {};
	vector<int> c2_diff = {};
	//����������Ƭ�������غϿͻ���
	set_intersection(c1_cross_guest.begin(), c1_cross_guest.end(), c2_cross_guest.begin(), \
		c2_cross_guest.end(), back_inserter(same_node));
	//�ҵ���������Ƭ���еĲ�ͬ�ͻ���
	set_difference(c1_cross_guest.begin(), c1_cross_guest.end(), c2_cross_guest.begin(), \
		c2_cross_guest.end(), back_inserter(c1_diff));
	set_difference(c2_cross_guest.begin(), c2_cross_guest.end(), c1_cross_guest.begin(), \
		c1_cross_guest.end(), back_inserter(c2_diff));
	
	vector<int> UnvisitedGuestC1 = {};
	vector<int> UnvisitedGuestC2 = {};
	if ((c1_diff.size() == 0) && (c2_diff.size() == 0)) {
		//�޲�ͬ�㣬����Ƭ�κ��ǿͻ��պÿ��Ի����滻
		return { sub_c1, sub_c2 };
	}
	else {
		//�в�ͬ�㣬��c1_diff����sub_c1��ȱ�ٵĵ㣬c2_diff����sub_c1���ظ��ĵ�
		for (vector<vector<Routine>>::iterator iter = sub_c1.Sequence.begin();\
			iter != sub_c1.Sequence.end(); ) {
			int no_sub = 0;
			//int at_end = 0;
			for (auto r : *iter) {
				for (auto g : r.GeneSequence) {
					if (find(c2_diff.begin(), c2_diff.end(), g.node.ID) != c2_diff.end()) {
						iter = sub_c1.Sequence.erase(iter);
						no_sub = 1;
						//����ÿһ��vr��˵��ֻҪ�ڲ�����һ���ظ��㣬��ֱ�����������һ��vr
						break;
					}
				}
			}
			if (no_sub == 0) {
				iter++;
			}
			//if (iter == sub_c1.Sequence.end()) {
			//	break;
			//}
		}
		sub_c1.Sequence.push_back(c2.Sequence[c2_cross]);
		vector<int> VisitedGuestC1 = {};
		vector<int> VisitedGuestC2 = {};
		for (auto vr : sub_c1.Sequence) {
			for (auto r : vr) {
				for (auto g : r.GeneSequence) {
					if (g.node.type == 2) {
						VisitedGuestC1.push_back(g.node.ID);
					}
				}
			}
		}
		sort(VisitedGuestC1.begin(), VisitedGuestC1.end());
		for (vector<vector<Routine>>::iterator iter = sub_c2.Sequence.begin(); \
			iter != sub_c2.Sequence.end(); ) {
			int no_sub = 0;
			for (auto r : *iter) {
				for (auto g : r.GeneSequence) {
					if (find(c1_diff.begin(), c1_diff.end(), g.node.ID) != c1_diff.end()) {
						iter = sub_c2.Sequence.erase(iter);
						no_sub = 1;
						break;
					}
				}
			}
			if (no_sub == 0) {
				iter++;
			}
		}
		//for (auto n : c2_diff) {
		//	UnvisitedGuestC2.push_back(n);
		//}
		sub_c2.Sequence.push_back(c1.Sequence[c1_cross]);
		for (auto vr : sub_c2.Sequence) {
			for (auto r : vr) {
				for (auto g : r.GeneSequence) {
					if (g.node.type == 2) {
						VisitedGuestC2.push_back(g.node.ID);
					}
				}
			}
		}
		sort(VisitedGuestC2.begin(), VisitedGuestC2.end());
		float dice1 = rand() / double(RAND_MAX);
		float dice2 = rand() / double(RAND_MAX);
		int vehicle1 = int(10000 * dice1);
		int vehicle2 = int(10000 * dice2);
		sub_c1.WalkGuest(VisitedGuestC1, vehicle1);
		sub_c2.WalkGuest(VisitedGuestC2, vehicle2);
		return { sub_c1, sub_c2 };
	}
}

int ChooseBest(vector<Chromosome> population) {
	int best_index = 0;
	int best_cost = 0;
	for (int i = 0; i < population.size(); i++) {
		if (i == 0) {
			best_cost = population[i].FitValue;
		}
		else {
			if (population[i].FitValue < best_cost) {
				best_cost = population[i].FitValue;
				best_index = i;
			}
		}
	}
	return best_index;
}

int ChooseWorst(vector<Chromosome> population) {
	int worst_index = 0;
	int worst_cost = 0;
	for (int i = 0; i < population.size(); i++) {
		if (i == 0) {
			worst_cost = population[i].FitValue;
		}
		else {
			if (population[i].FitValue > worst_cost) {
				worst_cost = population[i].FitValue;
				worst_index = i;
			}
		}
	}
	return worst_index;
}

void GAApp::Run() {
	InitAll();
	MakeInitPopulation(POPULATION_NUM, 0);
	srand((unsigned int)time(NULL));
	int c1_index = 0;
	int c2_index = 0;
	for (auto &c : population) {
		c.CalFitValue();
	}
	for (int cross_count = 0; cross_count < CROSS_ITERATION; cross_count++) {
		cout << "Iteration " << cross_count << endl;

		//���ѡ����Ⱥ�е�����Ⱦɫ��
		float dice1 = rand() / double(RAND_MAX);
		float dice2 = rand() / double(RAND_MAX);
		c1_index = int(dice1 * (population.size() - 1));
		c2_index = int(dice2 * (population.size() - 1));
		if (cross_count % 5 == 0) {
			//ÿ5�ֵ������ͽ������ּ��뽻��һ��
			c1_index = ChooseBest(population);
		}
		vector<Chromosome> new_2_c = CrossOver(population[c1_index], population[c2_index]);
		int worst_index = ChooseWorst(population);
		//�ý���õ��������Ӵ��滻��population��������������
		new_2_c[0].CalFitValue();
		new_2_c[1].CalFitValue();
		if ((new_2_c[0].FitValue < population[worst_index].FitValue) || (new_2_c[1].FitValue < population[worst_index].FitValue)) {
			population[worst_index] = (new_2_c[0].FitValue < new_2_c[1].FitValue)\
				? new_2_c[0] : new_2_c[1];
			cout << "population improved" << endl;
			cout << population[worst_index].FitValue << endl;
		}

		//Ѱ�����Ÿ���
		int best_index = ChooseBest(population);
		cout << "The best one is " << best_index << " .... cost: " \
			<< population[best_index].FitValue << endl;
		if (_kbhit()) break;
	}
	int best_index = ChooseBest(population);
	population[best_index].CalFitValue(1);
	population[best_index].RecordToFile("best.txt");
}