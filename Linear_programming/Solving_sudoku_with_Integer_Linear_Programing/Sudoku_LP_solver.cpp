//This code is the modified version of https://www.gurobi.com/documentation/9.5/examples/sudoku_cpp_cpp.html 

#include "gurobi_c++.h"
#include <sstream>
#include <fstream>
#include <deque>
#include <tuple>
using namespace std;

#define sd 3
#define n (sd*sd)

string itos(int i) { stringstream s; s << i; return s.str(); }

void read_puzzle(int argc, char* argv[], int(&puzzle)[n][n], int (&original_puzzle)[n][n])
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
            while (s >> str_num) {
                //cout << str_num << endl;
                for (int j = 0; j < n; j++) {
                    int val = (int)str_num[j] - 48; // 0-based
                    if (val >= 0) {
                        puzzle[row_cnt][j] = val;
                        original_puzzle[row_cnt][j] = val;
                    }
                    else {
                        puzzle[row_cnt][j] = 0;
                        original_puzzle[row_cnt][j] = 0;
                    }
                }
            }
            row_cnt += 1;
        }
    }
    else {
        cout << "not found the file!" << endl;
    }
}

void print_puzzle(int(&puzzle)[n][n])
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
                cout << ". ";
            }
        }
        cout << endl;
    }
}

void reset_puzzle(int(&puzzle)[n][n], int(&original_puzzle)[n][n]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            puzzle[i][j] = original_puzzle[i][j];
        }
    }
}

bool solve_sudoku(int(&puzzle)[n][n], int(&original_puzzle)[n][n],
    int & num_solutions,
    deque<deque<tuple<int, int, int>>> &old_solutions_constraints, 
    bool is_first_assignment,
    GRBEnv &env)
{
    
    try { 

    /*GRBEnv env = GRBEnv();
    env.set("OutputFlag", "0");*/
    GRBModel model = GRBModel(env);
    model.set("OutputFlag", "0");
    GRBVar vars[n][n][n];
    int i, j, v;

    // Create 3-D array of model variables

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            for (v = 0; v < n; v++) {
                string s = "G_" + itos(i) + "_" + itos(j) + "_" + itos(v);
                vars[i][j][v] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, s);
            }
        }
    }

    // Add constraints

    // Each cell must take one value

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            GRBLinExpr expr = 0;
            for (v = 0; v < n; v++)
                expr += vars[i][j][v];
            string s = "V_" + itos(i) + "_" + itos(j);
            model.addConstr(expr, GRB_EQUAL, 1.0, s);
        }
    }

    // Each value appears once per row

    for (i = 0; i < n; i++) {
        for (v = 0; v < n; v++) {
            GRBLinExpr expr = 0;
            for (j = 0; j < n; j++)
                expr += vars[i][j][v];
            string s = "R_" + itos(i) + "_" + itos(v);
            model.addConstr(expr == 1.0, s);
        }
    }

    // Each value appears once per column

    for (j = 0; j < n; j++) {
        for (v = 0; v < n; v++) {
            GRBLinExpr expr = 0;
            for (i = 0; i < n; i++)
                expr += vars[i][j][v];
            string s = "C_" + itos(j) + "_" + itos(v);
            model.addConstr(expr == 1.0, s);
        }
    }

    // Each value appears once per sub-grid

    for (v = 0; v < n; v++) {
        for (int i0 = 0; i0 < sd; i0++) {
            for (int j0 = 0; j0 < sd; j0++) {
                GRBLinExpr expr = 0;
                for (int i1 = 0; i1 < sd; i1++) {
                    for (int j1 = 0; j1 < sd; j1++) {
                        expr += vars[i0 * sd + i1][j0 * sd + j1][v];
                    }
                }

                string s = "Sub_" + itos(v) + "_" + itos(i0) + "_" + itos(j0);
                model.addConstr(expr == 1.0, s);
            }
        }
    }

    // remove repeated solutions
    //deque<deque<tuple<int, int, int>>>
    
    for (v = 0; v < old_solutions_constraints.size(); v++) {
        GRBLinExpr expr = 0.;
        for (i = 0; i < old_solutions_constraints[v].size(); i++) {
            expr += vars[get<0>(old_solutions_constraints[v][i])][get<1>(old_solutions_constraints[v][i])][get<2>(old_solutions_constraints[v][i])];
        }
        string s = "Repeated constraint";
        //cout << "old_solutions_constraints[v].size(): " << old_solutions_constraints[v].size() << endl;
        model.addConstr(expr, GRB_LESS_EQUAL, old_solutions_constraints[v].size() - 1.0, s);
    }

    // Fix variables associated with pre-specified cells

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (puzzle[i][j] > 0) {
                vars[i][j][puzzle[i][j] - 1].set(GRB_DoubleAttr_LB, 1.0);
            }
        }
    }

    // Optimize model
    model.optimize();
    deque<tuple<int, int, int>> out = {};
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            for (v = 0; v < n; v++) {
                if (vars[i][j][v].get(GRB_DoubleAttr_X) > 0.5){
                    puzzle[i][j] = v + 1;
                    out.push_back(make_tuple(i, j, v));
                }
            }
        }
    }
    num_solutions += 1;
    if (!is_first_assignment) {
        //cout << "before old_solutions_constraints.push_back(out): " << out.size() << endl;
        old_solutions_constraints.push_back(out);
    }
    cout << endl;
    cout << "Solution#" << num_solutions << ":";
    print_puzzle(puzzle);
    reset_puzzle(puzzle, original_puzzle);
    return true;
    }
    catch (GRBException e) {
        /*cout << "Error code = " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;*/
        return false;
    }
    catch (...) {
        //cout << "Error during optimization" << endl;
        return false;
    }
}



int main(int argc, char* argv[])
{
    int puzzle[n][n], original_puzzle[n][n];
    int num_solutions = 0;
    GRBEnv env = GRBEnv();
    env.set("OutputFlag", "0");
    deque<deque<tuple<int, int, int>>> old_solutions_constraints = {};
    read_puzzle(argc, argv, puzzle, original_puzzle);
    print_puzzle(puzzle);
    cout << endl;
    cout << "Using a Binary-ILP Formulation" << endl;
    solve_sudoku(puzzle, original_puzzle, num_solutions, old_solutions_constraints, true, env);
    num_solutions = 0;
    bool tmp_bool = true;
    cout << endl;
    cout << "Using a Binary-ILP Formulation" << endl;
    while(tmp_bool) {
        tmp_bool = solve_sudoku(puzzle, original_puzzle, num_solutions, old_solutions_constraints, false, env);
    }
    return 0;
}