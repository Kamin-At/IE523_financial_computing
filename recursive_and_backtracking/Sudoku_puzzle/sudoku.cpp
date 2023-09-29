#include <iostream>
#include "sudoku.h"
#include <filesystem>


int main (int argc, char * const argv[]) 
{
	Sudoku x;
	x.read_puzzle(argc, argv);
	x.print_puzzle();
	cout << endl;
	x.Solve(0,0);
	x.alternate_Solve(0,0);
	//x.print_puzzle();
	
    return 0;
}
