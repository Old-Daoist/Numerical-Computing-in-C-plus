#include <iostream>
#include "../include/GuessGame.hpp"

using namespace std;

int main()
{
    char again;

    do
    {
        GuessGame game;

        game.chooseDifficulty();
        game.generateNumber();
        game.play();

        cout << "\nPlay again? (y/n): ";
        cin >> again;

    } while (again == 'y' || again == 'Y');

    cout << "Thanks for playing!\n";

    return 0;
}