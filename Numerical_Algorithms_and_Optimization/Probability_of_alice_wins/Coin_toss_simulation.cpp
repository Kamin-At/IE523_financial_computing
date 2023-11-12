// Coin_toss_simulation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <unordered_map> 
#include <math.h>
#include <deque>
#include <string>
#include <chrono>
#include <random>
#include <fstream>

using namespace std;

default_random_engine generator;
uniform_real_distribution<double> distribution(0.0, 1.0);

string hash_pair(int n, int r) {
    return to_string(n) + "_" + to_string(r);
}

long n_C_r(int n, int r, unordered_map<string, long>& mapper) {
    string x = hash_pair(n, r);
    if (mapper.find(x) == mapper.end()) {
        double tmp = 1;
        for (int i = 1; i <= r; i++) {
            tmp *= (double)(n - i + 1) / ((double)i);
        }
        mapper[x] = (long)tmp;
        //cout << x << " => " << (long)tmp << " " << tmp << endl;
        return (long)tmp;
    }
    else {
        //cout << x << " => " << mapper[x] << endl;
        return mapper[x];
    }
}


double analytical_approach(int n, double p, double q, unordered_map<string, long> & mapper) {
    double Alice = 0.;
    for (int r = 0; r <= n; r++) {
        //cout << "Q (r = " << r << ")" << endl;
        double tmp = 0.;
        for (int j = r + 1; j <= n; j++) {
            tmp += n_C_r(n, j, mapper) * pow(p, j) * pow(1 - p, n - j);
        }
        Alice += (n_C_r(n, r, mapper) * pow(q, r) * pow(1 - q, n - r)) * tmp;
        //cout << "Alice = " << Alice << endl;
    }
    return Alice;
}

int coin_toss(int n, double p) {
    int cnt = 0;
    for (int i = 0; i < n; i++) {
        if (distribution(generator) <= p) {
            cnt += 1;
        }
    }
    return cnt;
}

double experimental_approach(int n, double p, double q, int num_trial) {
    double cnt = 0.;
    for (int i = 0; i < num_trial; i++) {
        if (coin_toss(n, p) > coin_toss(n, q)) {
            cnt += 1;
        }
    }
    return cnt / num_trial;
}

int main(int argc, char* argv[])
{
    auto start = std::chrono::high_resolution_clock::now();
    unordered_map<string, long> mapper;
    double p = atof(argv[1]);
    double q = atof(argv[2]);
    double tmp_result_exp = 0.;
    double tmp_result_ana = 0.;
    int num_trial = 1000000;//atoi(argv[3]);
    ofstream myFile("comparison_analytical_vs_experimental_results.csv");
    myFile << "N,Analytical probability of Alice wins,Experimental probability of Alice wins\n";
    cout << "Probability of head up for Alice = " << p << endl;
    cout << "Probability of head up for Bob = " << q << endl;

    deque<double> d = {-1., 0.};
    int i = 1;

    deque<double> analytical_results = {};

    while (d[1] > d[0]) {
        //cout << "i: " << i << endl;
        tmp_result_ana = analytical_approach(i, p, q, mapper);
        analytical_results.push_back(tmp_result_ana);
        //cout << "i: " << i << " Analytical result: " << tmp_result_ana << " Experimental result: " << tmp_result_exp << endl;
        
        d.push_back(tmp_result_ana);
        d.pop_front();
        i++;
    }

    cout << "The optimal number of coin tosses is " << i - 2 << endl;
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Time taken by function: " << duration.count()/1000000. << " seconds" << endl;
    
    /////////////// TASK 2 /////////////////////////////////////////
    
    for (int i = 1; i <= 30; i++) {
        if (i > analytical_results.size()){
            tmp_result_ana = analytical_approach(i, p, q, mapper);
        }
        else {
            tmp_result_ana = analytical_results[i - 1];
        }
        tmp_result_exp = experimental_approach(i, p, q, num_trial);
        myFile << i << "," << tmp_result_ana << "," << tmp_result_exp << "\n";
    }
    myFile.close();


}