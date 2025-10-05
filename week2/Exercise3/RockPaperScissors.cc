////////////////////////////////////////////////////
// Make a program that plays Rock Paper Scissors. //
////////////////////////////////////////////////////

#include <iostream>
#include <stdio.h>

bool check_input(std::string input) {
    if (!((input == "Rock") || (input == "Paper") || (input == "Scissors")))
    {
        std::cout << input << " is invalid. Valid options are 'Rock', 'Paper', or 'Scissors'." << std::endl;
        return false;
    }
    return true;
}

void play_rps(std::string player1, std::string player2) {
    // perform checks
    bool p1Valid = check_input(player1);
    bool p2Valid = check_input(player2);
    if ((p1Valid) && (p2Valid)) {
        if (player1 == player2) {
            std::cout << "Tie!";
        } else if (
            ((player1 == "Rock") && (player2 == "Scissors"))
            || ((player1 == "Scissors") && (player2 == "Paper"))
            || ((player1 == "Paper") && (player2 == "Rock"))
        ) {
            std::cout << player1 << " beats " << player2 << ", Player 1 wins!";
        } else {
            std::cout << player2 << " beats " << player1 << ", Player 2 wins!";
        }
    }
}

int main() {
    // Read in moves from command line
    std::string player1, player2;
    std::cout << "Welcome to Rock, Paper, Scissors!" << std::endl;
    std::cout << "Player 1 move: ";
    std::getline(std::cin, player1);
    std::cout << "Player 2 move: ";
    std::getline(std::cin, player2);

    play_rps(player1, player2);
}