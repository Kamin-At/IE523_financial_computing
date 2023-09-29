// Tower_of_hanoi_3peg.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <deque>
#include <vector>

using namespace std;

void create_k_peg(int k, vector<deque<int>>  &x){
    for (int i = 0; i < k; i++) {
        deque <int> tmp_deq = {};
        x.push_back(tmp_deq);
    }
}

void initialize_n_block(int n, vector<deque<int>> &x) {
    for (int i = 1; i <= n; i++) {
        x[0].push_back(i);
    }
}

void print_all_peg_status(vector<deque<int>> &x){
    for (int i = 0; i < x.size(); i ++) {
        cout << "checking peg: " << i << endl;
            for (int j = 0; j < x[i].size(); j++) {
                cout << x[i][j] << endl;
            }
        cout << "-----------------------------------" << endl;
    }
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

void move_single_step(int from_peg, int to_peg, vector<deque<int>> &x, int & num_operation) {
    //Try to push
    x[to_peg].push_front(x[from_peg][0]);
    x[from_peg].pop_front();
    num_operation += 1;
}

void move_top_n_3peg(int n, int from_peg, int parking_peg, int to_peg, vector<deque<int>>& x, int & num_operation) {

    if (n == 1) {
        move_single_step(from_peg, to_peg, x, num_operation);
        check_is_all_legal(x);
    }
    else {
        move_top_n_3peg(n - 1, from_peg, to_peg, parking_peg, x, num_operation);
        check_is_all_legal(x);
        move_single_step(from_peg, to_peg, x, num_operation);
        check_is_all_legal(x);
        move_top_n_3peg(n - 1, parking_peg, from_peg, to_peg, x, num_operation);
        check_is_all_legal(x);
    }
}

void tw_hanoi_kpeg( int n,
                    int from_peg,
                    int to_peg,
                    deque<int> & free_space,
                    int parking_space,
                    vector<deque<int>> & x, 
                    int & num_operation){
    deque<int> order = {};

    for (int i = free_space.size() - 1; i >= 0; i--) {
        if (x[from_peg].size() < 2) {
            break;
        }
        move_top_n_3peg(x[from_peg].size() / 2, from_peg, parking_space, free_space[i], x, num_operation);
        order.push_back(free_space[i]);
        free_space.pop_back();
    }
    move_top_n_3peg(x[from_peg].size(), from_peg, parking_space - 1, to_peg, x, num_operation);
    free_space.push_front(from_peg);
    for (int i = order.size() - 1; i >= 0; i--) {
        tw_hanoi_kpeg(x[order[i]].size(), order[i], to_peg, free_space, parking_space - 1, x, num_operation);
    }
}

int get_number_to_move(int n){
    return n / 2;
}

void move_topn( int n,
                int from_peg,
                int to_peg,
                deque<int>& free_space,
                vector<deque<int>>& x,
                int& num_operation) {
    int current_desitnation;
    if (free_space.size() > 1) {
        if (n > 1) {
            current_desitnation = free_space[0];
            free_space.pop_front();
            int m = get_number_to_move(n);
            free_space.push_back(to_peg);
            move_topn(m, from_peg, current_desitnation, free_space, x, num_operation);
            free_space.pop_back();
            move_topn(n-m, from_peg, to_peg, free_space, x, num_operation);
            move_topn(m, current_desitnation, to_peg, free_space, x, num_operation);
        }
        else {
            move_single_step(from_peg, to_peg, x, num_operation);
        }
    }
    else {
        move_top_n_3peg(n, from_peg, free_space[0], to_peg, x, num_operation);
    }
}

int main()
{
    vector<deque<int>> l = {};
    int k, n;
    int num_operation = 0;
    cout << "input your N : " << endl;
    cin >> n;
    cout << "input your K : " << endl;
    cin >> k;
    
    //Initialize all the pegs and add n blocks to the peg 0.
    create_k_peg(k, l);
    initialize_n_block(n, l);

    deque<int> free_space = {};
    for (int i = 1; i < k - 1; i++) {
        free_space.push_back(i);
    }

    //tw_hanoi_kpeg(n, 0, k-1, free_space, k-1, l, num_operation);
    move_topn(n, 0, k - 1, free_space, l, num_operation);

    print_all_peg_status(l);
    cout << "total num_operation = " << num_operation << endl;
    ///////////////////////////////////////////////////////////
    l.clear();
    create_k_peg(k, l);
    initialize_n_block(n, l);
    num_operation = 0;

    move_top_n_3peg(n, 0, 1, k-1, l, num_operation);
    cout << "total num_operation (using 3 peg) = " << num_operation << endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
