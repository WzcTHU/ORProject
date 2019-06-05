#include<iostream>
#include<sstream>
#include<time.h>
#include<algorithm>
#include<conio.h>
#include"GAApp.h"
#define CROSS_ITERATION 10000				//种群交叉次数
#define IDENTITY_CROSS_NUM 1			//每对染色体交叉次数
#define POPULATION_NUM 100
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
		m_chro.WalkGuest({}, 1);		//已访问列表为空，从1号车开始走
		population.push_back(m_chro);
		m_chro.CalFitValue();
		ss2 << m_chro.FitValue;
		m_chro.RecordToFile("population\\chro_FitValue_" + ss2.str() + ".txt");
	}
	//使用随机算法生成一批染色体
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
	//交叉片段
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
	vector<int> AllGuestList = {};
	for (int i = DEPOT_NUM; i < DEPOT_NUM + GUEST_NUM; i++) {
		AllGuestList.push_back(i);
	}
	vector<int> same_node = {};
	vector<int> c1_diff = {};
	vector<int> c2_diff = {};
	//从两个交叉片段中找重合客户点
	set_intersection(c1_cross_guest.begin(), c1_cross_guest.end(), c2_cross_guest.begin(), \
		c2_cross_guest.end(), back_inserter(same_node));
	//找到两个交叉片段中的不同客户点
	set_difference(c1_cross_guest.begin(), c1_cross_guest.end(), c2_cross_guest.begin(), \
		c2_cross_guest.end(), back_inserter(c1_diff));
	set_difference(c2_cross_guest.begin(), c2_cross_guest.end(), c1_cross_guest.begin(), \
		c1_cross_guest.end(), back_inserter(c2_diff));
	
	vector<int> UnvisitedGuestC1 = {};
	vector<int> UnvisitedGuestC2 = {};
	if ((c1_diff.size() == 0) && (c2_diff.size() == 0)) {
		//无不同点，两个片段涵盖客户刚好可以互相替换
		return { sub_c1, sub_c2 };
	}
	else {
		//有不同点，则c1_diff就是sub_c1中缺少的点，c2_diff就是sub_c1中重复的点
		for (vector<vector<Routine>>::iterator iter = sub_c1.Sequence.begin();\
			iter != sub_c1.Sequence.end(); iter++) {
			for (auto r : *iter) {
				for (auto g : r.GeneSequence) {
					if (find(c2_diff.begin(), c2_diff.end(), g.node.ID) != c2_diff.end()) {
						//该vehicle_routine中有重复点，则将该vehicle_routine中的每一个客户点都纳入未访问列表中，并将routine删去
						for (auto r : *iter) {
							for (auto g : r.GeneSequence) {
								if (g.node.type == 2) {
									UnvisitedGuestC1.push_back(g.node.ID);
								}
							}
						}
						sub_c1.Sequence.erase(iter);
						iter = sub_c1.Sequence.begin();
						//对于每一个vr来说，只要内部含有一个重复点，就直接跳出检查下一个vr
						break;
					}
				}
			}
		}
		for (auto n : c1_diff) {
			UnvisitedGuestC1.push_back(n);
		}
		for (vector<vector<Routine>>::iterator iter = sub_c2.Sequence.begin(); \
			iter != sub_c2.Sequence.end(); iter++) {
			for (auto r : *iter) {
				for (auto g : r.GeneSequence) {
					if (find(c1_diff.begin(), c1_diff.end(), g.node.ID) != c1_diff.end()) {
						//该vehicle_routine中有重复点，则将该vehicle_routine中的每一个客户点都纳入未访问列表中，并将routine删去
						for (auto r : *iter) {
							for (auto g : r.GeneSequence) {
								if (g.node.type == 2) {
									UnvisitedGuestC2.push_back(g.node.ID);
								}
							}
						}
						//对于每一个vr来说，只要内部含有一个重复点，就直接跳出检查下一个vr，并将当前vr抹除
						sub_c2.Sequence.erase(iter);
						iter = sub_c2.Sequence.begin();
						break;
					}
				}
			}
		}
		for (auto n : c2_diff) {
			UnvisitedGuestC2.push_back(n);
		}
		sort(UnvisitedGuestC1.begin(), UnvisitedGuestC1.end());
		sort(UnvisitedGuestC2.begin(), UnvisitedGuestC2.end());
		vector<int> VisitedGuestC1 = {};
		vector<int> VisitedGuestC2 = {};
		set_difference(AllGuestList.begin(), AllGuestList.end(), UnvisitedGuestC1.begin(), \
			UnvisitedGuestC1.end(), back_inserter(VisitedGuestC1));
		set_difference(AllGuestList.begin(), AllGuestList.end(), UnvisitedGuestC2.begin(), \
			UnvisitedGuestC2.end(), back_inserter(VisitedGuestC2));
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
	for (int cross_count = 0; cross_count < CROSS_ITERATION; cross_count++) {
		cout << "Iteration " << cross_count << endl;
		for (auto &c : population) {
			c.CalFitValue();
		}

		//随机选择种群中的两个染色体
		float dice1 = rand() / double(RAND_MAX);
		float dice2 = rand() / double(RAND_MAX);
		c1_index = int(dice1 * (population.size() - 1));
		c2_index = int(dice2 * (population.size() - 1));
		if (cross_count % 5 == 0) {
			//每5轮迭代，就将最优种加入交叉一次
			c1_index = ChooseBest(population);
		}
		vector<Chromosome> new_2_c = CrossOver(population[c1_index], population[c2_index]);
		int worst_index = ChooseWorst(population);
		//用交叉得到的两个子代替换掉population中最差的两个个体
		new_2_c[0].CalFitValue();
		new_2_c[1].CalFitValue();
		if ((new_2_c[0].FitValue < population[worst_index].FitValue) || (new_2_c[1].FitValue < population[worst_index].FitValue)) {
			population[worst_index] = (new_2_c[0].FitValue < new_2_c[1].FitValue)\
				? new_2_c[0] : new_2_c[1];
			cout << "population improved" << endl;
			cout << population[worst_index].FitValue << endl;
		}

		//寻找最优个体
		int best_index = ChooseBest(population);
		cout << "The best one is " << best_index << " .... cost: " \
			<< population[best_index].FitValue << endl;
		if (_kbhit()) break;
	}
	int best_index = ChooseBest(population);
	population[best_index].RecordToFile("best.txt");
}