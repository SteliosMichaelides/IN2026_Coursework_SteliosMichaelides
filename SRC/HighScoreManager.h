#ifndef __HIGHSCOREMANAGER_H__
#define __HIGHSCOREMANAGER_H__

#include <vector>
#include <string>
#include "HighScore.h"

class HighScoreManager
{
public:
    HighScoreManager();
    ~HighScoreManager();

    // Loads scores
    void Load();

    // Writes scores
    void Save();

    // Returns true if score is high enough to enter the table
    bool IsHighScore(int score) const;

    // Adds an entry, sorts, trims
    void AddScore(const std::string &name, int score);

    // Get all entries sorted
    const std::vector<HighScore> &GetScores() const { return mScores; }

    // Build the display table
    std::string GetFormattedTable() const;

    static const int MAX_ENTRIES = 5;

private:
    std::vector<HighScore> mScores;
    std::string mFilename;
};

#endif