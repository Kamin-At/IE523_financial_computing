// Repeated_squaring_algorithm.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "newmat.h"
#include <random>
#include <string>
#include <ctime>
#include <fstream>

using namespace std;

default_random_engine generator;
uniform_real_distribution<double> distribution(0.0, 1.0);

Matrix initialize_A(int no_of_rows)
{
    Matrix X(no_of_rows, no_of_rows);

    for (int i = 1; i <= no_of_rows; i++) {
        for (int j = 1; j <= no_of_rows; j++) {
            X(i, j) = (distribution(generator) - 0.5) * 10;//fill matrix X with y generated from Y ~ U(-5,5)
        }
    }
    return X;
}

Matrix BruteForce(Matrix A, int exponent, int no_of_rows)
{
    Matrix Out;
    for (int i = 0; i < exponent; i++) {
        if (i == 0) {
            Out = A;
        }
        else {
            Out = Out * A;
        }
    }
    return Out;
}

Matrix repeat_squaring(Matrix A, int exponent, int no_of_rows)
{
    if (exponent == 0) {
        IdentityMatrix I(no_of_rows);
        return I;
    }

    if (exponent % 2) {
        return A * repeat_squaring(A * A, exponent / 2, no_of_rows);
    }
    else {
        return repeat_squaring(A * A, exponent / 2, no_of_rows);
    }
}

int main(int argc, char* argv[])
{
    int no_of_rows = stoi(argv[2]);
    int exponent = stoi(argv[1]);
    ofstream fout("computing_time_comparison.csv");


    Matrix A, B, Result, Result2;
    clock_t time_before, time_after;
    
    A = initialize_A(no_of_rows);
    /*for (int i = 1; i <= no_of_rows; i++) {
        for (int j = 1; j <= no_of_rows; j++) {
            cout << A(i, j) << " ";
        }
        cout << endl;
    }*/
    time_before = clock();
    Result = repeat_squaring(A, exponent, no_of_rows);
    time_after = clock();
    float diff = ((float)time_after - (float)time_before);
    cout << "Repeated Squaring Result:" << endl;
    cout << "It took " << diff / CLOCKS_PER_SEC << " seconds to complete" << endl;
    
    time_before = clock();
    Result2 = BruteForce(A, exponent, no_of_rows);
    time_after = clock();
    diff = ((float)time_after - (float)time_before);
    cout << "Direct Multiplication Result:" << endl;
    cout << "It took " << diff / CLOCKS_PER_SEC << " seconds to complete" << endl;
    
    ////////////////////////////////////// TASK2 //////////////////////////////////////////////////
    
    fout << "exponent,Brute_force_time,repeated_squaring_time\n";
    double repeat_squaring_time, BruteForce_time;
    
    //The reason why I didn't use B_size = 5 is because the time used in repeat squaring is too small.
    int B_size = 50;

    B = initialize_A(B_size);//create B_sizexB_size matrix to run the comparison of time complexity
    for (int i = 1; i <= 310; i++) {
        time_before = clock();
        Result = repeat_squaring(B, i, B_size);
        time_after = clock();
        repeat_squaring_time = ((double)time_after - (double)time_before) / CLOCKS_PER_SEC;

        time_before = clock();
        Result2 = BruteForce(B, i, B_size);
        time_after = clock();
        BruteForce_time = ((double)time_after - (double)time_before) / CLOCKS_PER_SEC;

        fout << i << "," << BruteForce_time << "," << repeat_squaring_time << "\n";
    }
    fout.close();

}
