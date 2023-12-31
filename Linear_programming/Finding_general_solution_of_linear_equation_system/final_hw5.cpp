// Computing the General Solution to Ax=b using the NEWMAT library
// Modified by Kamin Atsavasirilert from Prof. R.S. Sreenivas's work for IE523: Financial Computing

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <fstream>
#include "newmatap.h"           
#include "newmat.h"
#include "newmatio.h"
#include <algorithm>

#define EPSILON 0.0000001
// global vector-of-vectors that keeps the basic solutions that we find
// For example, for the example in page 6 of lesson 2, when all the work
// is completed, solution_set will be a 4 x 4 matrix whose columns are
// s1, s2, s3 and s4.
vector < vector < double > > solution_set;

// This routine computes the rank of a matrix by counting the number of 
// non-zero singular-values of the matrix.
// See http://en.wikipedia.org/wiki/Singular_value_decomposition for details
// regarding the singular-value-decomposition of a matrix

int Rank(Matrix A)
{
	int n_rows = A.nrows();
	int n_cols = A.ncols();
	int cnt = 0;
	if (n_rows >= n_cols) {
		DiagonalMatrix D(n_cols);
		SVD(A, D);
		for (int i = 1; i <= n_cols; i++) {
			if (abs(D(i)) > EPSILON) {
				cnt += 1;
			}
		}
	}
	else {
		DiagonalMatrix D(n_rows);
		SVD(A.t(), D);
		for (int i = 1; i <= n_rows; i++) {
			if (abs(D(i)) > EPSILON) {
				cnt += 1;
			}
		}
	}
	return cnt;
}

// This routine finds the basic solution where we have rank(A) 
// many columns and y is in the linear span of the cols of A
ColumnVector Find_Basic_Solution(Matrix A, ColumnVector y)
{
	// Write code that implements equation 1 of lesson 2 of my
    // notes.  A should be of full rank and y is a column vector
	if (Rank(A) == min(A.ncols(), A.nrows()))
	{
		return ((A.t() * A).i()) * A.t() * y;
	}
	else {
		cout << "Matrix: " << endl;
		cout << setw(9) << setprecision(3) << A;
		cout << "is not of full-rank... exiting..." << endl;
		exit(0);
	}
}

void create_solution_vector(Matrix A, ColumnVector y)
{
	// pick rank(A)-many columns of A and check if this "thinned-out"
	// matrix has a rank that equals rank(A).  If yes, then find the 
	// solution.
    // This part of the code uses what you did in the first programming
    // assignment -- where you enumerated all possible ways of picking
    // k objects from a set of n objects.
    // This time -- you are picking rank(A)-many columns of A from the
    // set of all possible columns of A.
	int rank_of_A = Rank(A);
	{
		// Create a boolean array where the first rank(A)-many items 
		// are "true" (i.e. 1) the remaining n-rank(A) items are 
		// "false" (i.e. 0)
		vector <bool> did_i_pick_this_col(A.ncols());
		fill(did_i_pick_this_col.begin(), 
			 did_i_pick_this_col.begin() + rank_of_A, true);
		
		// I am going to use the prev_permutation STL in C++ (see 
		// http://www.cplusplus.com/reference/algorithm/prev_permutation/ )
		// to generate the required combinatorial outputs 
		do 
		{
            vector <vector <double> > B;
            // pick rank(A)-many columns of A and store in B which is a
            // vector-of-vectors
			// Look at your solution to the 1st programming assignment
            // for help, if you need it.
			
            // Look at your solution the 1st programming assignment
			for (int i = 0; i < A.ncols(); ++i) 
			{
				// fill this code appropriately
				if (did_i_pick_this_col[i]) {
					
					B.push_back({});
					for (int j = 0; j < A.nrows(); j++) {
						B.back().push_back(A(j + 1, i + 1));
					}
				}
			}

            // At the end of this part -- we have rank(A)-many columns of A
            // that are stored in B
			
			// check rank condition of selected colums of A
			Matrix Column_reduced_version_of_A(A.nrows(), rank_of_A);
			
			for (int j = 1; j <= A.nrows(); j++) {
				for (int k = 1; k <= rank_of_A; k++) {
					Column_reduced_version_of_A(j, k) = B[k - 1][j - 1];
				}
			}
				
			if (Rank(Column_reduced_version_of_A) == rank_of_A)
			{
				// we found a submatrix of A with the same rank...
				// have to compute a basic solution with this submatrix
				ColumnVector soln(rank_of_A);
				soln = Find_Basic_Solution(Column_reduced_version_of_A, y);
                vector <double> temp;

				// write code that inserts zeros in "soln" for each column
                // of A that was not picked, stores the result in temp.  For
                // example, see the bottom of page 6 of lesson 2 -- there is
                // a zero inserted in each basic-solution to reflect the
                // column of A that was not picked...
                // at the end of this "temp" is essentially "soln" with extra
                // zeros inserted into it.  Store temp for later use in the
                // matrix "solution_set" (which is a global array stored as a
                // vector-of-vectors...
				int cnt_num_ans = 1;
				for (int i = 0; i < did_i_pick_this_col.size(); i++) {
					if (did_i_pick_this_col[i]) {
						temp.push_back(soln(cnt_num_ans));
						cnt_num_ans += 1;
					}
					else {
						temp.push_back(0);
					}
				}


                solution_set.push_back(temp);
            }
		} while (prev_permutation(did_i_pick_this_col.begin(), did_i_pick_this_col.end())); 
	}
}

// For this part of the code I assume "solution_set" contains all
// basic solutions to Ax = y. All I have to do, is to pick
// rank(solution_set)-many columns of "solution_set", check if
// the picked-columns have a rank that is equal to rank(solution_set)
// and present the general solution as the affine-combination of
// them.
// The matrix A and y (that are passed to this function) are only
// needed for some print-statements. It is not used in any computation.
void trim_the_solution_set(Matrix A, ColumnVector y)
{
	Matrix solution(A.ncols(), solution_set.size());
	for (int j = 1; j <= A.ncols(); j++) {
		for (int k = 1; k <= solution_set.size(); k++) {
			solution(j, k) = solution_set[k - 1][j - 1];
		}
	}
	
	int rank_of_solution_set = Rank(solution);
	
	
	{
		// Create a boolean array where the first rank(solution)-many items 
		// are "true" (i.e. 1) the remaining n-rank(solution) items are 
		// "false" (i.e. 0)
		vector <bool> did_i_pick_this(solution.ncols());
		fill(did_i_pick_this.begin(), 
			 did_i_pick_this.begin() + rank_of_solution_set, true);
		
		// I am going to use the prev_permutation STL in C++ (see 
		// http://www.cplusplus.com/reference/algorithm/prev_permutation/ )
		// to generate the required combinatorial outputs 
		do 
		{
			vector <vector <double> > B;
			
			// write code that picks (rank_of_solution_set)-many columns
            // of the local-matrix "solution" and stores these columns in B
			for (int i = 0; i < solution.ncols(); i++)
			{
				// fill this code appropriately
				if (did_i_pick_this[i]) {
					B.push_back({});
					for (int j = 0; j < solution.nrows(); j++) {
						B.back().push_back(solution(j + 1, i + 1));
					}
				}
			}
            // check rank condition of selected colums of A
            // I need the matrix "Column_reduced_version" in place of B
            // because "Rank" only works with Matrices (not vector-of-vectors) :-)
			Matrix Column_reduced_version(solution.nrows(), rank_of_solution_set);
			for (int j = 1; j <= solution.nrows(); j++)
				for (int k = 1; k <= rank_of_solution_set; k++)
					Column_reduced_version(j,k) = B[k-1][j-1];
			if (Rank(Column_reduced_version) == rank_of_solution_set)
			{
				// we found a submatrix of solution with the same rank...
				cout << "Solving:" << endl;
				cout << setw(10) << A << endl;
				cout << "* solution = " << endl << endl;
				cout << setw(10) << y << endl;
				cout << "solution is the affine-combination of these vectors" << endl;
				cout << setw(10) <<  Column_reduced_version << endl;
				cout << "Verification: (check each column below and y-vector above)" << endl;
				cout << setw(10) << A*Column_reduced_version << endl;
				exit(0);
			}
		} while (prev_permutation(did_i_pick_this.begin(), did_i_pick_this.end())); 
	}
}

void process_data(char* argv[])
{
	int no_of_rows, no_of_cols;
	double element;
	
	// First command line input is the name of the input file
	ifstream input_file(argv[1]);
	if (input_file.is_open()) // If the input file exists in the local directory
	{
		input_file >> no_of_rows; // read the number of rows of A matrix
		input_file >> no_of_cols; // read the number of rows of A matrix
		
		// Create the space for the matrix and column vector
		Matrix A(no_of_rows, no_of_cols);
		ColumnVector y(no_of_rows);
		
		//read the A matrix
		for (int i = 1; i <= no_of_rows; i++)
		{
			for (int j = 1; j <= no_of_cols; j++)
			{
				input_file >> element;
				A(i,j) = element;
			}
		}
		
		// read the y vector
		for (int i = 1; i <= no_of_rows; i++)
		{
			input_file >> element;
			y(i) = element;
		}
		// check if there is a solution, if "yes" then compute genl soln
		Matrix B(no_of_rows, no_of_cols+1);
		B = (A | y);
		if (Rank(B) == Rank(A))
		{
			create_solution_vector(A, y);
			trim_the_solution_set(A, y);
		}
		else {
			// we found a submatrix of solution with the same rank...
			cout << "Solving:" << endl;
			cout << setw(10) << A << endl;
			cout << "* vector_x = " << endl << endl;
			cout << setw(10) << y << endl;
			cout << "There is no solution to this equation" << endl;
			exit(0);
		}
	}
	else {
		// I need all this stuff to print out the name of the PWD
		//char *path=NULL;
		//size_t size;
		//path = getcwd(path, size);
		cout << "Input file: " << argv[1] << " does not exist in present folder"  << endl;
	}	
}
	
int main (int argc, char* argv[])
{
	if (argc == 1) 
		cout << "Input filename missing" << endl;
	else 
		process_data(argv);
}