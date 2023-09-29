// NQueens2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <deque>

using namespace std;

bool is_next_move_valid(int next_move, deque<int> &table) {
    //horizontal check
    for (int i = 0; i < table.size(); i++) {
        if (table[i] == next_move) {
            return false;
        }
    }

    //diagonal check
    for (int i = 0; i< table.size(); i++) {
        if ((table[i] - (table.size() - i) == next_move)||(table[i] + (table.size() - i) == next_move)) {
            return false;
        }
    }

    return true;
}

bool solve(int col, int n, deque<int> &table, int &num_solutions) {
    if (col >= n) {
        num_solutions += 1;
        cout << "solution = ";
        for (int i = 0; i < table.size(); i++) {
            cout << table[i] << ", ";
        }
        cout << endl;
    }
    else {
        bool is_at_least_1_sol = false;
        for (int i = 1; i <= n; i++) {
            if (is_next_move_valid(i, table)) {
                table.push_back(i);
                if (!solve(col + 1, n, table, num_solutions)) {
                    table.pop_back();
                }
                else {
                    is_at_least_1_sol = true;
                }
            }
        }
        if (is_at_least_1_sol) {
            return true;
        }
    }
    return false;
}

int main()
{
    int n;
    cout << "input your N: " << endl;
    cin >> n;
    deque<int> table = {};
    int num_solutions = 0;
    solve(0, n, table, num_solutions);
    cout << "total solution of " << n << "x" << n <<" = " << num_solutions << endl;
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
