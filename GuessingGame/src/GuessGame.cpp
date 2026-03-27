#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../include/GuessGame.hpp"

using namespace std;

GuessGame::GuessGame()
{
    attempts = 0;
    score = 0;
}

void GuessGame::chooseDifficulty()
{
    int choice;

    cout << "Choose Difficulty\n";
    cout << "1. Easy (1-50)\n";
    cout << "2. Medium (1-100)\n";
    cout << "3. Hard (1-500)\n";
    cout << "Enter your choice between 1 and 3:\n"; 

    cin >> choice;

    if (choice == 1)
    {
        range = 50;
        maxAttempts = 10;
    }
    else if (choice == 2)
    {
        range = 100;
        maxAttempts = 20;
    }
    else
    {
        range = 500;
        maxAttempts = 25;
    }
}

void GuessGame::generateNumber()
{
    srand(time(0));
    secretNumber = rand() % range + 1;
}

void GuessGame::play()
{
    cout << "\nGuess a number between 1 and " << range << endl;

    while (attempts < maxAttempts)
    {
        cout << "Enter guess: ";
        cin >> guess;

        attempts++;

        checkGuess();

        if (guess == secretNumber)
            break;
    }

    if (guess != secretNumber)
    {
        cout << "You lost! The number was: " << secretNumber << endl;
    }

    calculateScore();
}

void GuessGame::checkGuess()
{
    if (guess > secretNumber)
        cout << "Too High!\n";

    else if (guess < secretNumber)
        cout << "Too Low!\n";

    else
        cout << "Correct!\n";
}

void GuessGame::calculateScore()
{
    if (guess == secretNumber)
    {
        score = 100 - attempts * 10;

        if (score < 0)
            score = 0;

        cout << "Attempts: " << attempts << endl;
        cout << "Score: " << score << endl;
    }
}