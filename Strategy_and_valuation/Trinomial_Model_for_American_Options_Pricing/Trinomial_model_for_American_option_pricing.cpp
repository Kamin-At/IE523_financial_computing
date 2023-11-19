// Trinomial_model_for_American_option_pricing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <unordered_map>
#include <deque>
using namespace std;

double expiration, rf, vol, s0, k, q, dt, u, new_u, tmp, pu, pd, R;
int n_stage, cnt, cnt_hit;
//unordered_map<string, double> mapper = {};
//deque <deque <double>> mapper = {};

double** mapper;

double N(const double& z) {
    if (z > 6.0) { return 1.0; };// this guards against overflow 
    if (z < -6.0) { return 0.0; };
    double b1 = 0.31938153;
    double b2 = -0.356563782;
    double b3 = 1.781477937;
    double b4 = -1.821255978;
    double b5 = 1.330274429;
    double p = 0.2316419;
    double c2 = 0.3989423;
    double a = fabs(z);
    double t = 1.0 / (1.0 + a * p);
    double b = c2 * exp((-z) * (z / 2.0));
    double n = ((((b5 * t + b4) * t + b3) * t + b2) * t + b1) * t;
    n = 1.0 - b * n;
    if (z < 0.0) n = 1.0 - n;
    return n;
};

double option_price_call_black_scholes(const double& s0,       // spot (underlying) price
    const double& k,       // strike (exercise) price,
    const double& rf,       // interest rate
    const double& vol,   // volatility 
    const double& expiration) {  // time to maturity 
    double time_sqrt = sqrt(expiration);
    double d1 = (log(s0 / k) + rf * expiration) / (vol * time_sqrt) + 0.5 * vol * time_sqrt;
    double d2 = d1 - (vol * time_sqrt);
    return s0 * N(d1) - k * exp(-rf * expiration) * N(d2);
};

double option_price_put_black_scholes(const double& s0,      // spot price
    const double& k,      // Strike (exercise) price,
    const double& rf,      // interest rate
    const double& vol,  // volatility
    const double& expiration) {
    double time_sqrt = sqrt(expiration);
    double d1 = (log(s0 / k) + rf * expiration) / (vol * time_sqrt) + 0.5 * vol * time_sqrt;
    double d2 = d1 - (vol * time_sqrt);
    return k * exp(-rf * expiration) * N(-d2) - s0 * N(-d1);
};

double compute_european_option_price(
    int i,
    int j,
    double cur_price,
    int option_type//1 for call and 0 for put
) {
    double result;
    if (mapper[i][j + i] != -1) {
        cnt_hit += 1;
        return mapper[i][j+i];
    }

    if (i == n_stage) {
        if (option_type == 1) {//call option
            result = max(cur_price - k, 0.);
        }
        else if (option_type == 0) {//put option
            result = max(k - cur_price, 0.);
        }
        mapper[i][j + i] = result;
        
        return result;
    }

    result = pu * compute_european_option_price(
        i + 1, 
        j + 1, 
        u * cur_price,
        option_type) +
        pd * compute_european_option_price(
            i + 1, 
            j - 1, 
            cur_price / u,
            option_type) +
        (1 - pu - pd) * compute_european_option_price(
            i + 1, 
            j, 
            cur_price,
            option_type);

    mapper[i][j + i] = result;

    return result;
}

double compute_american_option_price(
    int i,
    int j,
    double cur_price,
    int option_type//1 for call and 0 for put
) {
    double result, current_value;
    if (mapper[i][j + i] != -1) {
        return mapper[i][j + i];
    }

    if (i == n_stage) {
        if (option_type == 1) {//call option
            result = max(cur_price - k, 0.);
        }
        else if (option_type == 0) {//put option
            result = max(k - cur_price, 0.);
        }
        mapper[i][j + i] = result;

        return result;
    }

    if (option_type == 1) {//call option
        current_value = cur_price - k;
    }
    else if (option_type == 0) {//put option
        current_value = k - cur_price;
    }

    result = max(
        current_value
        ,
        (
            pu * compute_american_option_price(
            i + 1,
            j + 1,
            u * cur_price,
            option_type) +
            pd * compute_american_option_price(
                i + 1,
                j - 1,
                cur_price / u,
                option_type) +
            (1 - pu - pd) * compute_american_option_price(
                i + 1,
                j,
                cur_price,
                option_type)
        ) / R
    );

    mapper[i][j + i] = result;

    return result;
}

void reinitialize_mapper(double **mapper) {//Just to reuse mapper again when pricing other new options
    for (int i = 0; i <= n_stage; i++)
        for (int j = 0; j <= 2 * n_stage + 1; j++)
            mapper[i][j] = -1;
}

int main(int argc, char* argv[])
{
    expiration = stod(argv[1]);
    n_stage = atoi(argv[2]);
    rf = stod(argv[3]);
    vol = stod(argv[4]);
    s0 = stod(argv[5]);
    k = stod(argv[6]);
    q = 0.;

    cnt = 0;
    cnt_hit = 0;

    dt = expiration / (double) n_stage;
    u = exp(vol * sqrt(2. * dt));
    new_u = pow(u, 0.5);

    R = exp((rf - q) * dt);
    tmp = pow(R, 0.5);

    pu = pow((tmp - 1./ new_u) / (new_u - 1. / new_u), 2.);
    pd = pow((new_u - tmp) / (new_u - 1. / new_u), 2.);

    //initialize the 2-d mapper array with -1 for memoization (the option value will never be negative, 
    //so if negative, we have not reached that state yet.)
    mapper = new double* [n_stage + 1];
    for (int i = 0; i <= n_stage; i++)
        mapper[i] = new double[2 * n_stage + 1];
    
    ///////////////////////////// PART 1 European option pricing with trinomial model///////////////////////////////////
    reinitialize_mapper(mapper);
    double call_price = exp(-rf * expiration) * compute_european_option_price(0, 0, s0, 1);
    double BSM_call_price = option_price_call_black_scholes(s0, k, rf, vol, expiration);
    reinitialize_mapper(mapper);
    double put_price = exp(-rf * expiration) * compute_european_option_price(0, 0, s0, 0);
    double BSM_put_price = option_price_put_black_scholes(s0, k, rf, vol, expiration);
    cout << "(Memoized) Recursive Trinomial European Option Pricing" << endl;
    cout << "Expiration Time (Years) = " << expiration << endl;
    cout << "Number of Divisions = " << rf << endl;
    cout << "Risk Free Interest Rate = " << n_stage << endl;
    cout << "Volatility (%tage of stock value) = " << (int) (vol * 100) << endl;
    cout << "Initial Stock Price = " << s0 << endl;
    cout << "Strike Price = " << k << endl;
    cout << "---------------------------------------" << endl;
    cout << "Up Factor = " << u << endl;
    cout << "Uptick Probability = " << pu << endl;
    cout << "---------------------------------------" << endl;
    cout << "Trinomial Price of an European Call Option = " << call_price << endl;
    cout << "Call Price according to Black-Scholes = " << BSM_call_price << endl;
    cout << "---------------------------------------" << endl;
    cout << "Trinomial Price of an European Put Option = " << put_price << endl;
    cout << "Put Price according to Black-Scholes = " << BSM_put_price << endl;
    cout << "---------------------------------------" << endl;
    cout << "Verifying Put-Call Parity: S+P-C = Kexp(-r*T)" << endl;
    cout << s0 << " + " << put_price << " - " << call_price << " = " << k << "exp(-" << rf << " * " << expiration << ")" << endl;
    cout << s0 + put_price - call_price << " = " << k *  exp(-rf * expiration) << endl;

    ///////////////////////////// PART 2 American option pricing with trinomial model///////////////////////////////////
    reinitialize_mapper(mapper);
    double amer_call_price = compute_american_option_price(0, 0, s0, 1);
    reinitialize_mapper(mapper);
    double amer_put_price = compute_american_option_price(0, 0, s0, 0);
    cout << endl;
    cout << endl;
    cout << "(Memoized) Recursive Trinomial American Option Pricing" << endl;
    cout << "Expiration Time (Years) = " << expiration << endl;
    cout << "Number of Divisions = " << rf << endl;
    cout << "Risk Free Interest Rate = " << n_stage << endl;
    cout << "Volatility (%tage of stock value) = " << (int)(vol * 100) << endl;
    cout << "Initial Stock Price = " << s0 << endl;
    cout << "Strike Price = " << k << endl;
    cout << "---------------------------------------" << endl;
    cout << "Up Factor = " << u << endl;
    cout << "Uptick Probability = " << pu << endl;
    cout << "Notick Probability = " << 1 - pu - pd << endl;
    cout << "Downtick Probability = " << pd << endl;
    cout << "---------------------------------------" << endl;
    cout << "Trinomial Price of an American Call Option = " << amer_call_price << endl;
    cout << "Trinomial Price of an American Put Option = " << amer_put_price << endl;
    cout << "---------------------------------------" << endl;
    cout << "Verifying Put-Call Parity: S+P-C = Kexp(-r*T) for American Options" << endl;
    cout << s0 << " + " << amer_put_price << " - " << amer_call_price << " = " << k << "exp(-" << rf << " * " << expiration << ")" << endl;
    cout << s0 + amer_put_price - amer_call_price << " ?=? " << k * exp(-rf * expiration) << endl;
    cout << "Looks Like Put-Call Parity does NOT hold" << endl;
}