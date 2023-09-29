// TW_hanoi_k_peg.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <deque>
#include <vector>
using namespace std;

void create_k_peg(int k, vector<deque<int>>& x) {
	for (int i = 0; i < k; i++) {
		deque <int> tmp_deq = {};
		x.push_back(tmp_deq);
	}
}

void initialize_n_block(int n, vector<deque<int>>& x) {
	for (int i = 1; i <= n; i++) {
		x[0].push_back(i);
	}
}

void print_all_peg_status(vector<deque<int>>& x) {
	for (int i = 0; i < x.size(); i++) {
		cout << "checking peg: " << i << endl;
		for (int j = 0; j < x[i].size(); j++) {
			cout << x[i][j] << endl;
		}
		cout << "-----------------------------------" << endl;
	}
}

void print_peg_status(vector<deque<int>>& x, int peg_number) {

	cout << "State of Peg " << peg_number + 1 << " (Top to Bottom):";
	for (int j = 0; j < x[peg_number].size(); j++) {
		cout << " "  << x[peg_number][j] ;
	}
	cout << endl;

}

bool check_is_all_legal(vector<deque<int>>& x) {
	for (int i = 0; i < x.size(); i++) {
		if (x[i].size() != 1) {
			for (int j = 1; j < x[i].size(); j++) {
				if (x[i][j] < x[i][j - 1]) {
					//throw invalid_argument("illegal move!");
					cout << "illegal move" << endl;
					return false;
				}
			}
		}
	}
	return true;
}

void move_single_step(int from_peg, int to_peg, vector<deque<int>>& x, int& num_operation) {
	//Try to push
	x[to_peg].push_front(x[from_peg][0]);
	int moving_disk = x[from_peg][0];
	x[from_peg].pop_front();
	num_operation += 1;
	if (check_is_all_legal(x)) {
		cout << "Move disk " << moving_disk << " from Peg " << from_peg + 1 << " to Peg " << to_peg + 1 << " (Legal)" << endl;
	}
	else {
		cout << "Move disk " << moving_disk << " from Peg " << from_peg + 1 << " to Peg " << to_peg + 1 << " (Illegal)" << endl;
	}
	
	
	
}

int pick_the_right_number_to_move(int n, int num_free_peg) {
	if (num_free_peg == 1) {
		return n - 1;
	}
	else {
		return n / 2;
	}
	
}

void move(int n,
	int from_peg,
	int to_peg,
	deque<int> &free_pegs,
	vector<deque<int>> &x,
	int & num_operation) {
	if (free_pegs.size() != 0) {
		if (n > 1) {
			int m = pick_the_right_number_to_move(n, free_pegs.size());
			int intermediate_peg = free_pegs[free_pegs.size() - 1];
			free_pegs.pop_back();
			free_pegs.push_back(to_peg);
			move(m, from_peg, intermediate_peg, free_pegs, x, num_operation);
			free_pegs.pop_back();
			move(n - m, from_peg, to_peg, free_pegs, x, num_operation);
			free_pegs.push_back(from_peg);
			move(m, intermediate_peg, to_peg, free_pegs, x, num_operation);
			free_pegs.pop_back();
			free_pegs.push_back(intermediate_peg);

		}
		else {
			move_single_step(from_peg, to_peg, x, num_operation);
		}
	}
	else {
		move_single_step(from_peg, to_peg, x, num_operation);
	}
	
}


int main()
{
	vector<deque<int>> l = {};
	int k, n;
	int num_operation = 0;
	cout << "Number of pegs? ";
	cin >> k;
	cout << "Number of disks? ";
	cin >> n;
	cout << "-----------------------------------" << endl;
	//Initialize all the pegs and add n blocks to the peg 0.
	create_k_peg(k, l);
	initialize_n_block(n, l);
	print_peg_status(l, 0);
	cout << "Number of Steps = " << num_operation << endl;
	cout << "-----------------------------------" << endl;

	deque<int> free_pegs = {};
	for (int i = 1; i < k - 1; i++) {
		free_pegs.push_back(i);
	}
	//print_all_peg_status(l);
	move(n, 0, k-1, free_pegs, l, num_operation);
	cout << "-----------------------------------" << endl;
	print_peg_status(l, k - 1);
	cout << "Number of Steps = " << num_operation << endl;
	//print_all_peg_status(l);
	cout << "-----------------------------------";
}