/*
 *  sudoku.h
 *  Sudoku
 *  Created by Prof. Ramavarapu Sreenivas 
 *  Inspired by: http://web.eecs.utk.edu/courses/spring2012/cs140/Notes/Sudoku/index.html
*/

#ifndef sudoku
#define sudoku

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <deque>
#include<tuple>

//using std::vector;
using namespace std;


class Sudoku 
{ 
	// Private
	int puzzle[9][9];
	int original_puzzle[9][9];
	int num_sol = 0;
	deque<deque<deque<int>>> diagonal_ind = {
		{{0,0}},
		{{0,1}, {1,0}},
		{{2,0}, {1,1}, {0,2}},
		{{3,0}, {2,1}, {1,2}, {0,3}},
		{{4,0}, {3,1}, {2,2}, {1,3}, {0,4}},
		{{5,0}, {4,1}, {3,2}, {2,3}, {1,4}, {0,5}},
		{{6,0}, {5,1}, {4,2}, {3,3}, {2,4}, {1,5}, {0,6}},
		{{7,0}, {6,1}, {5,2}, {4,3}, {3,4}, {2,5}, {1,6}, {0,7}},
		{{8,0}, {7,1}, {6,2}, {5,3}, {4,4}, {3,5}, {2,6}, {1,7}, {0,8}},
		{{8,1}, {7,2}, {6,3}, {5,4}, {4,5}, {3,6}, {2,7}, {1,8}},
		{{8,2}, {7,3}, {6,4}, {5,5}, {4,6}, {3,7}, {2,8}},
		{{8,3}, {7,4}, {6,5}, {5,6}, {4,7}, {3,8}},
		{{8,4}, {7,5}, {6,6}, {5,7}, {4,8}},
		{{8,5}, {7,6}, {6,7}, {5,8}},
		{{8,6}, {7,7}, {6,8}},
		{{8,7}, {7,8}},
		{{8,8}}
	};

	// Private member function that checks if the named row is valid
	bool row_valid(int row)
	{
		// write code that checks if "row" is valid
		// must uniquely contain values from 1 to 9 
		vector<int> tmp = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		for (int i = 0; i < 9; i++) {
			if ((puzzle[row][i] <= 9)&&(puzzle[row][i] >= 1)) {
				if (tmp[puzzle[row][i]] == 1) {
					return false;
				}
				else {
					tmp[puzzle[row][i]] = 1;
				}
			}
			else if (puzzle[row][i] == 0) {
				//Do nothing
			}
			else {
				return false;
			}
		}
		return true;
	}
	
	// Private member function that checks if the named column is valid
	bool col_valid(int col)
	{
		// check validity of "col" 
		// must uniquely contain values from 1 to 9 
		vector<int> tmp = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		for (int i = 0; i < 9; i++) {
			if ((puzzle[i][col] <= 9) && (puzzle[i][col] >= 1)) {
				if (tmp[puzzle[i][col]] == 1) {
					return false;
				}
				else {
					tmp[puzzle[i][col]] = 1;
				}
			}
			else if (puzzle[i][col] == 0) {
				//Do nothing
			}
			else {
				return false;
			}
		}
		return true;
	}
	
	// Private member function that checks if the named 3x3 block is valid
	bool block_valid(int row, int col)
	{
		// check 3 x 3 block validity 
		// must uniquely contain values from 1 to 9 
		vector<int> tmp = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if ((puzzle[3 * row + i][3 * col + j] <= 9) && (puzzle[3 * row + i][3 * col + j] >= 1)) {
					if (tmp[puzzle[3 * row + i][3 * col + j]] == 0) {
						tmp[puzzle[3 * row + i][3 * col + j]] = 1;
					}
					else {
						return false;
					}
				}
				else if (puzzle[3 * row + i][3 * col + j] == 0) {
					//Do nothing
				}
				else {
					return false;
				}
			}
		}
		return true;
	}

	tuple<int, bool> diagonal_repeating_valid(int row, int col) {
		//Check upper diagonal
		int i = row;
		int j = col;
		vector<int> tmp = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		while ((i >= 0)&&(j < 9)) {
			if (puzzle[i][j] != 0) {
				if (tmp[puzzle[i][j]] != 0) {
					return make_tuple(puzzle[i][j], false);
				}
				else {
					tmp[puzzle[i][j]] += 1;
				}
			}
			i -= 1;
			j += 1;
		}
		i = row + 1;
		j = col - 1;
		while ((i < 9) && (j >= 0)) {
			if (puzzle[i][j] != 0) {
				if (tmp[puzzle[i][j]] != 0) {
					return make_tuple(puzzle[i][j], false);
				}
				else {
					tmp[puzzle[i][j]] += 1;
				}
			}
			i += 1;
			j -= 1;
		}
		return make_tuple(-1, true);
	}

	tuple<int, int, int, int, int, int, bool> adjacent_diagonal_valid(int row, int col) {
		bool invalid_check = false;
		int tmp_row1 = -1;
		int tmp_row2 = -1;
		int tmp_col1 = -1;
		int tmp_col2 = -1;
		if ((row > 0) && (col < 8)) {
			if (puzzle[row - 1][col + 1] != 0) {
				if (abs(puzzle[row][col] - puzzle[row - 1][col + 1]) < 4) {
					invalid_check = true;
					tmp_row1 = row - 1;
					tmp_col1 = col + 1;
				}
			}
		}
		if ((row < 8) && (col > 0)) {
			if (puzzle[row + 1][col - 1] != 0) {
				if (abs(puzzle[row][col] - puzzle[row + 1][col - 1]) < 4) {
					invalid_check = true;
					tmp_row2 = row + 1;
					tmp_col2 = col - 1;
				}
			}
		}
		if (invalid_check) {
			int tmp_val1 = -1;
			int tmp_val2 = -1;
			
			if (tmp_row1 >= 0) {
				tmp_val1 = puzzle[tmp_row1][tmp_col1];
			}
			if (tmp_row2 >= 0) {
				tmp_val2 = puzzle[tmp_row2][tmp_col2];
			}
			return make_tuple(tmp_row1, tmp_col1, tmp_val1, tmp_row2, tmp_col2, tmp_val2, false);
		}

		return make_tuple(tmp_row1, tmp_col1, -1, tmp_row2, tmp_col2, -1, true);
	}

	void reset_puzzle() {
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				puzzle[i][j] = original_puzzle[i][j];
			}
		}
	}
	
public:
	// Public member function that reads the incomplete puzzle
	// we are not doing any checks on the input puzzle -- that is,
	// we are assuming they are indeed valid
	void read_puzzle(int argc, char * const argv[])
	{
		// write code that reads the input puzzle using the 
		// guidelines of figure 23 of the bootcamp material
		string filename = argv[1];
		cout << "Input File Name: " << filename << endl;
		filename = filename; //+ ".txt";
		
		ifstream input_file(filename);
		if (input_file.is_open()) {
			string line;
			int row_cnt = 0;
			string str_num;

			while (getline(input_file, line)) {
				stringstream s(line);
				int col_cnt = 0;
				while (s >> str_num) {
					puzzle[row_cnt][col_cnt] = stoi(str_num);
					original_puzzle[row_cnt][col_cnt] = stoi(str_num);
					col_cnt += 1;
				}
				row_cnt += 1;
			}
		}
		else {
			cout << "not found the file!" << endl;
		}
	}
	
	// Public member function that prints the puzzle when called
	void print_puzzle()
	{
		cout << endl << "Board Position" << endl;
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				// check if we have a legitimate integer between 1 and 9
				if ((puzzle[i][j] >= 1) && (puzzle[i][j] <= 9))
				{
					// printing initial value of the puzzle with some formatting
					cout << puzzle[i][j] << " ";
				}
				else {
					// printing initial value of the puzzle with some formatting
					cout << "X ";
				}
			}
			cout << endl;
		}
	}
	
	bool check_if_legal(int row, int col, int verbose) {
		//row and col check
		if (!((row_valid(row)) && (col_valid(col)))) {
			return false;
		}
		//block check
		if (!block_valid(row/3, col/3)) {
			return false;
		}
		//diagonal repeating check
		tuple<int, bool> tmp_out = diagonal_repeating_valid(row, col);
		if (!get<1>(tmp_out)) {
			if (verbose) {
				cout << "The number " << get<0>(tmp_out) << " appears multiple times along the positive-diagonal" << endl;
			}
			return false;
		}
		//adjacent diagonal check

		tuple<int, int, int, int, int, int, bool> tmp_out2 = adjacent_diagonal_valid(row, col);
		if (!get<6>(tmp_out2)) {
			if (verbose) {
				if (get<0>(tmp_out2) >= 0) {
					cout << "puzzle[" << get<0>(tmp_out2) << "][" << get<1>(tmp_out2) << "] = " << get<2>(tmp_out2) << ", ";
				}
				if (get<3>(tmp_out2) >= 0) {
					cout << "puzzle[" << get<3>(tmp_out2) << "][" << get<4>(tmp_out2) << "] = " << get<5>(tmp_out2) << ", ";
				}
				cout << "puzzle[" << row << "][" << col << "] = " << puzzle[row][col] << ", and they do not meet the minimum-difference of 4" << endl;
			}
				return false;
		}

		return true;
	}

	tuple<int, bool, int, int, int, int, int, int, int, int, int, bool> check_if_legal_whole_board() {
		bool at_least_one_invalid = false;
		
		tuple<int, bool> tmp_out = make_tuple(-1, true);
		for (int i = 0; i < diagonal_ind.size(); i++) {
			for(int j =0; j < diagonal_ind[i].size(); j++)
				if (puzzle[diagonal_ind[i][j][0]][diagonal_ind[i][j][1]] != 0) {
					//diagonal repeating check
					tmp_out = diagonal_repeating_valid(diagonal_ind[i][j][0], diagonal_ind[i][j][1]);
					at_least_one_invalid = !get<1>(tmp_out);
					if (at_least_one_invalid) {
						break;
					}
				}
			if (at_least_one_invalid) {
				break;
			}
		}
		bool at_least_one_invalid2 = false;
		tuple<int, int, int, int, int, int, bool> tmp_out2 = make_tuple(-1, -1, -1, -1, -1, -1, true);
		int tmp_row = -1;
		int tmp_col = -1;
		int tmp_val = -1;
		for (int i = 0; i < diagonal_ind.size(); i++) {
			for (int j = 0; j < diagonal_ind[i].size(); j++)
				if (puzzle[diagonal_ind[i][j][0]][diagonal_ind[i][j][1]] != 0) {
					//adjacent diagonal check
					tmp_out2 = adjacent_diagonal_valid(diagonal_ind[i][j][0], diagonal_ind[i][j][1]);
					at_least_one_invalid2 = !get<6>(tmp_out2);
					if (at_least_one_invalid2) {
						tmp_row = diagonal_ind[i][j][0];
						tmp_col = diagonal_ind[i][j][1];
						tmp_val = puzzle[diagonal_ind[i][j][0]][diagonal_ind[i][j][1]];
						break;
					}
				}
			if (at_least_one_invalid2) {
				break;
			}
		}
		return make_tuple(
			get<0>(tmp_out),
			get<1>(tmp_out),
			get<0>(tmp_out2),
			get<1>(tmp_out2),
			get<2>(tmp_out2),
			get<3>(tmp_out2),
			get<4>(tmp_out2),
			get<5>(tmp_out2),
			tmp_row,
			tmp_col,
			tmp_val,
			get<6>(tmp_out2));
	}

	// Public member function that (recursively) implements the brute-force 
	// search for possible solutions to the incomplete Sudoku puzzle
	bool Solve(int row, int col)
	{
		// this part of the code identifies the row and col number of the 
		// first incomplete (i.e. 0) entry in the puzzle.  If the puzzle has
		// no zeros, the variable row will be 9 => the puzzle is done, as 
		// each entry is row-, col- and block-valid...

		// use the pseudo code of figure 3 of the description
		// Find next unfilled indices x and y
		int x = -1;
		int y = -1;
		//cout << "row: " << row << "col: " << col << endl;
		bool is_break = false;
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (puzzle[i][j] == 0) {
					x = i;
					y = j;
					//cout << "found unfilled at x: " << x << "y: " << y << endl;
					is_break = true;
					break;
				}
			}
			if (is_break) {
				break;
			}
		}

		if (!is_break) {
			cout << "Solution #1";
			print_puzzle();
			cout << endl;
			reset_puzzle();
			return true;
		}
		else {
			for (int i = 1; i < 10; i++) {
				puzzle[x][y] = i;
				if (check_if_legal(x, y, 1)) {
					if (Solve(x, y)) {
						return true;
					}
				}
			}
		}
		puzzle[x][y] = 0;
		return false;
	}

	bool alternate_Solve(int row, int col)
	{
		// this part of the code identifies the row and col number of the 
		// first incomplete (i.e. 0) entry in the puzzle.  If the puzzle has
		// no zeros, the variable row will be 9 => the puzzle is done, as 
		// each entry is row-, col- and block-valid...

		// use the pseudo code of figure 3 of the description
		// Find next unfilled indices x and y
		int x = -1;
		int y = -1;
		//cout << "row: " << row << "col: " << col << endl;
		bool is_break = false;
		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (puzzle[i][j] == 0) {
					x = i;
					y = j;
					//cout << "found unfilled at x: " << x << "y: " << y << endl;
					is_break = true;
					break;
				}
			}
			if (is_break) {
				break;
			}
		}

		if (!is_break) {
			num_sol += 1;
			cout << "Solution #" << num_sol;			
			print_puzzle();
			cout << endl;
			//return true;
		}
		else {
			for (int i = 1; i < 10; i++) {
				puzzle[x][y] = i;
				if (check_if_legal(x, y, 0)) {
					if (alternate_Solve(x, y)) {
						//return true;
					}
				}
			}
		}
		puzzle[x][y] = 0;
		return false;
	}
};

#endif