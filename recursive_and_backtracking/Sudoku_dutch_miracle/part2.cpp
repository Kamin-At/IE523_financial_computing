// Soduku Solver using Brute-Force Search implemted using 
// recursion.
// Written for IE523: Financial Computation by Prof. Sreenivas
// and GE523: Discrete Event Systems
//
#include <iostream>
#include "sudoku.h"
#include <filesystem>


int main (int argc, char * const argv[]) 
{
	Sudoku x;
	x.read_puzzle(argc, argv);
	x.print_puzzle();
	cout << endl;
	/////////////////////// USE this part for Assignment part 1 /////////////////////////////// 
	/////////////////////// Fine a solution of Dutch Miracle with printing ////////////////////
	/////////////////////// when 1 or 2 Dutch Miracle constraints are not satisfied.///////////
	/*tuple<int, bool, int, int, int, int, int, int, int, int, int, bool> check_whole_board_result = x.check_if_legal_whole_board();
	if ((!get<1>(check_whole_board_result))||(!get<11>(check_whole_board_result))) {
		cout << "Partially-Filled Sudoku Puzzle does not meet Dutch Miracle requirements... Quitting!" << endl;
		if (!get<1>(check_whole_board_result)) {
			cout << "The number " << get<0>(check_whole_board_result) << " appears multiple times along the positive-diagonal" << endl;
		}
		
		if (!get<11>(check_whole_board_result)) {
			if (get<2>(check_whole_board_result) >= 0) {
				cout << "puzzle[" << get<2>(check_whole_board_result) << "][" << get<3>(check_whole_board_result) << "] = " << get<4>(check_whole_board_result) << ", ";
			}
			if (get<5>(check_whole_board_result) >= 0) {
				cout << "puzzle[" << get<5>(check_whole_board_result) << "][" << get<6>(check_whole_board_result) << "] = " << get<7>(check_whole_board_result) << ", ";
			}
			cout << "puzzle[" << get<8>(check_whole_board_result) << "][" << get<9>(check_whole_board_result)  << "] = " << get<10>(check_whole_board_result) << ", and they do not meet the minimum-difference of 4" << endl;
		}
	}
	else {
		x.Solve(0, 0);
	}*/
	/////////////////////////////////////////////////////////////////////////////////////////// 
	/////////////////////// USE this part for Assignment part 2 /////////////////////////////// 
	/////////////////////// Find all the Dutch Miracle sudoku solutions ///////////////////////
	/////////////////////// (without printing) ////////////////////////////////////////////////
	tuple<int, bool, int, int, int, int, int, int, int, int, int, bool> check_whole_board_result = x.check_if_legal_whole_board();
	
	cout << "Enumerating all solutions to the Dutch Miracle Sudoku instance shown above" << endl;
	if ((!get<1>(check_whole_board_result))||(!get<11>(check_whole_board_result))) {
		cout << "Partially-Filled Sudoku Puzzle does not meet Dutch Miracle requirements... Quitting!" << endl;
	}
	else {
		x.alternate_Solve(0, 0);
	}
	/////////////////////////////////////////////////////////////////////////////////////////// 
    return 0;
}
