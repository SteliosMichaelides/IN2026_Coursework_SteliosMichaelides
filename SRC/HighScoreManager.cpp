#include "HighScoreManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

HighScoreManager::HighScoreManager() : mFilename("highscores.txt")
{
}

HighScoreManager::~HighScoreManager()
{
}

void HighScoreManager::Load()
{
    mScores.clear();
    std::ifstream file(mFilename.c_str());
    if (!file.is_open())
        return;

    std::string name;
    int score;

    // One entry per line
    while (file >> name >> score)
    {
        mScores.push_back(HighScore(name, score));
    }
    file.close();

    std::sort(mScores.begin(), mScores.end());
    if (mScores.size() > MAX_ENTRIES)
        mScores.resize(MAX_ENTRIES);
}

void HighScoreManager::Save()
{
    std::ofstream file(mFilename.c_str());
    if (!file.is_open())
        return;

    for (size_t i = 0; i < mScores.size(); ++i)
    {
        file << mScores[i].name << " " << mScores[i].score << "\n";
    }
    file.close();
}

bool HighScoreManager::IsHighScore(int score) const
{
    if (score <= 0)
        return false;
    if (mScores.size() < MAX_ENTRIES)
        return true;
    return score > mScores.back().score;
}

void HighScoreManager::AddScore(const std::string &name, int score)
{
    // Replace spaces with underscores
    std::string clean_name = name.empty() ? "PLAYER" : name;
    for (size_t i = 0; i < clean_name.size(); ++i)
    {
        if (clean_name[i] == ' ')
            clean_name[i] = '_';
    }

    mScores.push_back(HighScore(clean_name, score));
    std::sort(mScores.begin(), mScores.end());
    if (mScores.size() > MAX_ENTRIES)
        mScores.resize(MAX_ENTRIES);
    Save();
}

std::string HighScoreManager::GetFormattedTable() const
{
    std::ostringstream oss;
    oss << "HIGH SCORES";
    if (mScores.empty())
    {
        oss << "    (none yet)";
    }
    else
    {
        for (size_t i = 0; i < mScores.size(); ++i)
        {
            oss << "    " << (i + 1) << ". "
                << mScores[i].name << " - " << mScores[i].score;
        }
    }
    oss << "    Press ESC to return";
    return oss.str();
}
