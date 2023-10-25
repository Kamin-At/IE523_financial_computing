// CardGame_strategy_valuation.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

//unordered_set<string> table;
unordered_map<string, double> table;
string hash_deck(int total_black_cards, int total_red_cards) {
	return to_string(total_black_cards) + '_' + to_string(total_red_cards);
}

double compute_game_value2(int total_black_cards, int total_red_cards) {
	if (table.find(hash_deck(total_black_cards, total_red_cards)) != table.end()) {
		return table[hash_deck(total_black_cards, total_red_cards)];
	}
	
	if (total_red_cards == 0) {
		return total_black_cards;
	}
	if (total_black_cards == 0) {
		return 0.;
	}
	double total_remaining = total_black_cards + total_red_cards + 0.;
	double prob_to_draw_black = (total_black_cards + 0.) / (total_remaining);
	double prob_to_draw_red = (total_red_cards + 0.) / (total_remaining);
	double current_payoff_if_stop = total_black_cards - total_red_cards + 0.;
	double expected_payoff_if_continue =	prob_to_draw_red * (compute_game_value2(total_black_cards, total_red_cards - 1))+ \
											prob_to_draw_black * (compute_game_value2(total_black_cards - 1, total_red_cards));
	table[hash_deck(total_black_cards, total_red_cards)] = max(expected_payoff_if_continue, current_payoff_if_stop);
	return table[hash_deck(total_black_cards, total_red_cards)];
}
int main(int argc, char * argv[])
{
    cout << "argv[1]: " << argv[1] << endl;
    //Deck D(stoi(argv[1]));
	int total_num_cards = stoi(argv[1]);
	int num_red = total_num_cards/2;
	int num_black = total_num_cards - num_red;

    cout << "total_card: " << total_num_cards << " num_red: " << num_red << " num_black: " << num_black << endl;
    cout << "Value of the game = " << compute_game_value2(num_red, num_black) << endl;
}