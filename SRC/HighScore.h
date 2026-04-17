#ifndef __HIGHSCORE_H__
#define __HIGHSCORE_H__

#include <string>

struct HighScore
{
    std::string name;
    int score;

    HighScore() : name(""), score(0) {}
    HighScore(const std::string &n, int s) : name(n), score(s) {}

    // Sorting in descending
    bool operator<(const HighScore &other) const
    {
        return score > other.score;
    }
};

#endif