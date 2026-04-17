#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

enum GameState
{
    STATE_MENU,             // Main menu (Start / Difficulty / Instructions / High Scores)
    STATE_INSTRUCTIONS,     // How to play screen
    STATE_HIGH_SCORES,      // High score table view
    STATE_PLAYING,          // Active gameplay
    STATE_GAME_OVER_ENTRY   // Player typing their name for high score table
};

#endif