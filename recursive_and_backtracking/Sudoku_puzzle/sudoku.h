#ifndef sudoku
#define sudoku

#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>


using std::vector;
using namespace std;


class Sudoku 
{ 
	// Private
	int puzzle[9][9];
	int original_puzzle[9][9];
	int num_sol = 0;
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
		filename = filename + ".txt";
		
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
	
	bool check_if_legal(int row, int col) {
		//row and col check
		if (!((row_valid(row)) && (col_valid(col)))) {
			return false;
		}
		//block check
		if (!block_valid(row/3, col/3)) {
			return false;
		}
		return true;
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
				if (check_if_legal(x, y)) {
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
				if (check_if_legal(x, y)) {
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