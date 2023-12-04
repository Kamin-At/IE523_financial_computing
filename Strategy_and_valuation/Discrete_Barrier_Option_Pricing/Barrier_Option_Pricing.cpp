// Barrier_Option_Pricing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <string>
#include <random>
#include <tuple>
#include <chrono>
using namespace std;

#define PI 3.14159265358979323846  /* pi */

double expiration, rf, vol, s0, k, q, B, up_factor, prob_survive, tmp_mean, tmp_vol;

int n, m;
unsigned seed = (unsigned)chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator(seed);
uniform_real_distribution<double> distribution(0., 1.);

// Numerical approximation to the Unit-Normal CDF 
double N(const double& z) {
    if (z > 6.0) { return 1.0; }; // this guards against overflow
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

double get_Pc(
    double s0, 
    double B,
    double st,
    double t,
    double vol
) {
    if ((s0 <= B) || (st <= B)) {
        return 1.;
    }
    else {
        return exp(-2. * log(s0 / B) * log(st / B) / (pow(vol, 2.) * t));
    }

}

double get_Pd(
    double s0,
    double st,
    double B,
    double t,
    int m
) {
    prob_survive = 1.;
    double ti;
    for (int i = 1; i <= m; i++) {
        ti = t * ((double)i / (double)m);
        tmp_mean = s0 + (ti / t) * (st - s0);
        tmp_vol = sqrt(ti * (t - ti) / t);
        prob_survive *= (1. - N((B - tmp_mean) / tmp_vol));
    }
    return prob_survive;
}

double option_price_call_black_scholes(
    const double& s0,       // spot (underlying) price
    const double& k,       // strike (exercise) price,
    const double& rf,       // interest rate
    const double& vol,   // volatility 
    const double& t) {  // time to maturity 
    double time_sqrt = sqrt(t);
    double d1 = (log(s0 / k) + rf * t) / (vol * time_sqrt) + 0.5 * vol * time_sqrt;
    double d2 = d1 - (vol * time_sqrt);
    return s0 * N(d1) - k * exp(-rf * t) * N(d2);
};


double get_closed_form_solution_down_and_out_call(
    double s0,
    double k,
    double B,
    double rf,
    double vol,
    double t
) {
    double lambda = (rf + 0.5 * pow(vol, 2.)) / pow(vol, 2.);
    double x1 = log(s0 / B) / (vol * sqrt(t)) + lambda * vol * sqrt(t);
    double y1 = log(B / s0) / (vol * sqrt(t)) + lambda * vol * sqrt(t);
    double y = log(pow(B, 2.) / (s0 * k)) / (vol * sqrt(t)) + lambda * vol * sqrt(t);

    if (B > k) {
        
        return s0 * N(x1) -
            k * exp(-rf * t) * N(x1 - vol * sqrt(t)) -
            s0 * pow(B / s0, 2. * lambda) * N(y1) +
            k * exp(-rf * t) * pow(B / s0, 2. * lambda - 2.) * N(y1 - vol * sqrt(t));
    }
    else {
        double plain_call = option_price_call_black_scholes(s0, k, rf, vol, t);
        double call_down_in = s0 * pow(B / s0, 2. * lambda) * N(y) -
            k * exp(-rf * t) * pow(B / s0, 2. * lambda - 2.) * N(y - vol * sqrt(t));
        return plain_call - call_down_in;
    }
}

double option_price_put_black_scholes(
    const double& s0,      // spot price
    const double& k,      // Strike (exercise) price,
    const double& rf,      // interest rate
    const double& vol,  // volatility
    const double& t) 
{
    double time_sqrt = sqrt(t);
    double d1 = (log(s0 / k) + rf * t) / (vol * time_sqrt) + 0.5 * vol * time_sqrt;
    double d2 = d1 - (vol * time_sqrt);
    return k * exp(-rf * t) * N(-d2) - s0 * N(-d1);
};

double get_closed_form_solution_down_and_out_put(
    double s0,
    double k,
    double B,
    double rf,
    double vol,
    double t
) {
    if (s0 < B) {
        return 0.;
    }
    
    double lambda = (rf + 0.5 * pow(vol, 2.)) / pow(vol, 2.);
    double x1 = log(s0 / B) / (vol * sqrt(t)) + lambda * vol * sqrt(t);
    double y1 = log(B / s0) / (vol * sqrt(t)) + lambda * vol * sqrt(t);
    double y = log(pow(B, 2.) / (s0 * k)) / (vol * sqrt(t)) + lambda * vol * sqrt(t);

    double plain_put_val = option_price_put_black_scholes(s0, k, rf, vol, t);
    double put_down_in =
        -s0 * N(-x1) +
        k * exp(-rf * t) * N(-x1 + vol * sqrt(t)) +
        s0 * pow(B / s0, 2 * lambda) * (N(y) - N(y1)) -
        k * exp(-rf * t) * pow(B / s0, 2 * lambda - 2) * (N(y - vol * sqrt(t)) - N(y1 - vol * sqrt(t)));


    return plain_put_val - put_down_in;
}

int main(int argc, char* argv[])
{
    expiration = stod(argv[1]);
    rf = stod(argv[2]);
    vol = stod(argv[3]);
    s0 = stod(argv[4]);
    k = stod(argv[5]);
    n = atoi(argv[6]);
    m = atoi(argv[7]);
    B = stod(argv[8]);
    q = 0.;

    up_factor = exp(vol * sqrt(expiration / ((float)m)));

    double call_option_price = 0.0;
    double put_option_price = 0.0;
    double call_option_price_adj = 0.0;
    double put_option_price_adj = 0.0;


    double delta_T = expiration / ((float)m);
    double delta_R = (rf - 0.5 * pow(vol, 2)) * delta_T;
    double delta_SD = vol * sqrt(delta_T);

    double delta_R_full_path = (rf - 0.5 * pow(vol, 2)) * expiration;
    double delta_SD_full_path = vol * sqrt(expiration);

    double R = exp(rf * expiration);

    double S1, S2, S3, S4;

    double Pc1, Pc2, Pc3, Pc4;

    for (int i = 0; i < n; i++)
    {
        S1 = S2 = S3 = S4 = s0;
        double a, b, x, y;

        x = distribution(generator);
        y = distribution(generator);
        a = sqrt(-2.0 * log(x)) * cos(2 * PI * y);
        b = sqrt(-2.0 * log(x)) * sin(2 * PI * y);

        S1 = S1 * exp(delta_R_full_path + delta_SD_full_path * a);
        S2 = S2 * exp(delta_R_full_path - delta_SD_full_path * a);
        S3 = S3 * exp(delta_R_full_path + delta_SD_full_path * b);
        S4 = S4 * exp(delta_R_full_path - delta_SD_full_path * b);

        Pc1 = get_Pc(s0, B, S1, expiration, vol);
        Pc2 = get_Pc(s0, B, S2, expiration, vol);
        Pc3 = get_Pc(s0, B, S3, expiration, vol);
        Pc4 = get_Pc(s0, B, S4, expiration, vol);

        call_option_price_adj += 
            (1 - Pc1) * max(S1 - k, 0.) +
            (1 - Pc2) * max(S2 - k, 0.) +
            (1 - Pc3) * max(S3 - k, 0.) +
            (1 - Pc4) * max(S4 - k, 0.);
        put_option_price_adj +=
            (1 - Pc1) * max(k - S1, 0.) +
            (1 - Pc2) * max(k - S2, 0.) +
            (1 - Pc3) * max(k - S3, 0.) +
            (1 - Pc4) * max(k - S4, 0.);

        //////////////////////////////// Explicit Simulation ///////////////////////////
        S1 = S2 = S3 = S4 = s0;
        bool no_breach_S1, no_breach_S2, no_breach_S3, no_breach_S4;
        no_breach_S1 = no_breach_S2 = no_breach_S3 = no_breach_S4 = true;

        for (int j = 0; (j < m) && (no_breach_S1 || no_breach_S2 || no_breach_S3 || no_breach_S4); j++)
        {
            // create the unit normal variates using the Box-Muller Transform
            x = distribution(generator);
            y = distribution(generator);
            a = sqrt(-2.0 * log(x)) * cos(2 * PI * y);
            b = sqrt(-2.0 * log(x)) * sin(2 * PI * y);

            if (no_breach_S1)
                S1 = S1 * exp(delta_R + delta_SD * a);
            if (no_breach_S2)
                S2 = S2 * exp(delta_R - delta_SD * a);
            if (no_breach_S3)
                S3 = S3 * exp(delta_R + delta_SD * b);
            if (no_breach_S4)
                S4 = S4 * exp(delta_R - delta_SD * b);

            // check if path has breached the Barier
            if ((S1 <= B) || (!no_breach_S1))
            {
                no_breach_S1 = false;
                S1 = 0.;
            }

            if ((S2 <= B) || (!no_breach_S2))
            {
                no_breach_S2 = false;
                S2 = 0.;
            }

            if ((S3 <= B) || (!no_breach_S3))
            {
                no_breach_S3 = false;
                S3 = 0.;
            }

            if ((S4 <= B) || (!no_breach_S4))
            {
                no_breach_S4 = false;
                S4 = 0.;
            }
        }

        if (no_breach_S1) {
            call_option_price += max(S1 - k, 0.);
            put_option_price += max(k - S1, 0.);
        }
        if (no_breach_S2) {
            call_option_price += max(S2 - k, 0.);
            put_option_price += max(k - S2, 0.);
        }
        if (no_breach_S3) {
            call_option_price += max(S3 - k, 0.);
            put_option_price += max(k - S3, 0.);
        }
        if (no_breach_S4) {
            call_option_price += max(S4 - k, 0.);
            put_option_price += max(k - S4, 0.);
        }
    }
    call_option_price = call_option_price / (4. * (float) n * R);
    put_option_price = put_option_price / (4. * (float) n * R);
    call_option_price_adj = call_option_price_adj / (4. * (float)n * R);
    put_option_price_adj = put_option_price_adj / (4. * (float)n * R);

    cout << "--------------------------------" << endl;
    cout << "European Down-and-Out Continuous Barrier Options Pricing via Monte Carlo Simulation" << endl;
    cout << "Expiration Time (Years) = " << expiration << endl;
    cout << "Risk Free Interest Rate = " << rf << endl;
    cout << "Volatility (%age of stock value) = " << vol * 100 << endl;
    cout << "Initial Stock Price = " << s0 << endl;
    cout << "Strike Price = " << k << endl;
    cout << "Number of Trials = " << n << endl;
    cout << "Number of Divisions = " << m << endl;
    cout << "--------------------------------" << endl;
    cout << "--------------------------------" << endl;
    cout << "The average Call Price by explicit simulation\t= " << call_option_price << endl;
    cout << "The call price using the (1-p)-adjustment term\t= " << call_option_price_adj << endl;
    cout << "Theoretical Call Price\t= " << get_closed_form_solution_down_and_out_call(s0, k, B, rf, vol, expiration) << endl;    
    cout << "--------------------------------" << endl;
    cout << "The average Put Price by explicit simulation\t= " << put_option_price << endl;
    cout << "The put price using the (1-p)-adjustment term\t= " << put_option_price_adj << endl;
    cout << "Theoretical Put Price\t= " << get_closed_form_solution_down_and_out_put(s0, k, B, rf, vol, expiration) << endl;
    cout << "--------------------------------" << endl;

    ///////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////// PART 2 ////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    double Pd1, Pd2, Pd3, Pd4;
    call_option_price_adj = 0.;
    put_option_price_adj = 0.;
    call_option_price = 0.;
    put_option_price = 0.;

    for (int i = 0; i < n; i++)
    {
        S1 = S2 = S3 = S4 = s0;
        double a, b, x, y;

        x = distribution(generator);
        y = distribution(generator);
        a = sqrt(-2.0 * log(x)) * cos(2 * PI * y);
        b = sqrt(-2.0 * log(x)) * sin(2 * PI * y);

        S1 = S1 * exp(delta_R_full_path + delta_SD_full_path * a);
        S2 = S2 * exp(delta_R_full_path - delta_SD_full_path * a);
        S3 = S3 * exp(delta_R_full_path + delta_SD_full_path * b);
        S4 = S4 * exp(delta_R_full_path - delta_SD_full_path * b);

        Pd1 = get_Pd(s0, S1, B, expiration, m);
        Pd2 = get_Pd(s0, S2, B, expiration, m);
        Pd3 = get_Pd(s0, S3, B, expiration, m);
        Pd4 = get_Pd(s0, S4, B, expiration, m);

        call_option_price_adj +=
            Pd1 * max(S1 - k, 0.) +
            Pd2 * max(S2 - k, 0.) +
            Pd3 * max(S3 - k, 0.) +
            Pd4 * max(S4 - k, 0.);
        put_option_price_adj +=
            Pd1 * max(k - S1, 0.) +
            Pd2 * max(k - S2, 0.) +
            Pd3 * max(k - S3, 0.) +
            Pd4 * max(k - S4, 0.);

        //////////////////////////////// Explicit Simulation ///////////////////////////
        S1 = S2 = S3 = S4 = s0;
        bool no_breach_S1, no_breach_S2, no_breach_S3, no_breach_S4;
        no_breach_S1 = no_breach_S2 = no_breach_S3 = no_breach_S4 = true;

        for (int j = 0; (j < m) && (no_breach_S1 || no_breach_S2 || no_breach_S3 || no_breach_S4); j++)
        {
            // create the unit normal variates using the Box-Muller Transform
            x = distribution(generator);
            y = distribution(generator);
            a = sqrt(-2.0 * log(x)) * cos(2 * PI * y);
            b = sqrt(-2.0 * log(x)) * sin(2 * PI * y);

            if (no_breach_S1)
                S1 = S1 * exp(delta_R + delta_SD * a);
            if (no_breach_S2)
                S2 = S2 * exp(delta_R - delta_SD * a);
            if (no_breach_S3)
                S3 = S3 * exp(delta_R + delta_SD * b);
            if (no_breach_S4)
                S4 = S4 * exp(delta_R - delta_SD * b);

            // check if path has breached the Barier
            if ((S1 <= B) || (!no_breach_S1))
            {
                no_breach_S1 = false;
            }

            if ((S2 <= B) || (!no_breach_S2))
            {
                no_breach_S2 = false;
            }

            if ((S3 <= B) || (!no_breach_S3))
            {
                no_breach_S3 = false;
            }

            if ((S4 <= B) || (!no_breach_S4))
            {
                no_breach_S4 = false;
            }
        }

        if (no_breach_S1) {
            call_option_price += max(S1 - k, 0.);
            put_option_price += max(k - S1, 0.);
        }
        if (no_breach_S2) {
            call_option_price += max(S2 - k, 0.);
            put_option_price += max(k - S2, 0.);
        }
        if (no_breach_S3) {
            call_option_price += max(S3 - k, 0.);
            put_option_price += max(k - S3, 0.);
        }
        if (no_breach_S4) {
            call_option_price += max(S4 - k, 0.);
            put_option_price += max(k - S4, 0.);
        }
    }

    

    call_option_price = call_option_price / (4. * (float)n * R);
    put_option_price = put_option_price / (4. * (float)n * R);
    call_option_price_adj = call_option_price_adj / (4. * (float)n * R);
    put_option_price_adj = put_option_price_adj / (4. * (float)n * R);

    cout << "--------------------------------" << endl;
    cout << "European Down-and-Out Continuous Barrier Options Pricing via Monte Carlo Simulation" << endl;
    cout << "Expiration Time (Years) = " << expiration << endl;
    cout << "Risk Free Interest Rate = " << rf << endl;
    cout << "Volatility (%age of stock value) = " << vol * 100 << endl;
    cout << "Initial Stock Price = " << s0 << endl;
    cout << "Strike Price = " << k << endl;
    cout << "Number of Trials = " << n << endl;
    cout << "Number of Divisions = " << m << endl;
    cout << "--------------------------------" << endl;
    cout << "The average Call Price via explicit simulation of price paths\t\t\t= " << call_option_price << endl;
    cout << "The average Call Price with Brownian-Bridge correction on the final price\t= " << call_option_price_adj << endl;
    cout << "The average Put Price via explicit simulation of price paths\t\t\t= " << put_option_price << endl;
    cout << "The average Put Price with Brownian-Bridge correction on the final price\t= " << put_option_price_adj << endl;
    cout << "--------------------------------" << endl;



}