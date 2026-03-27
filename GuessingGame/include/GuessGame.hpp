#ifndef GUESSGAME_H
#define GUESSGAME_H

class GuessGame
{
private:
    int secretNumber;
    int guess;
    int attempts;
    int maxAttempts;
    int range;
    int score;

public:
    GuessGame();

    void chooseDifficulty();
    void generateNumber();
    void play();
    void checkGuess();
    void calculateScore();
};

#endif