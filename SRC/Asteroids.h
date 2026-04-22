#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h" 
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"
#include "GameState.h"
#include "HighScoreManager.h"
#include "IPowerUpListener.h"

class GameObject;
class Spaceship;
class GUILabel;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener, public IPowerUpListener
{
public:
	Asteroids(int argc, char *argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	// Declaration of IScoreListener interface //////////////////////////////////

	void OnScoreChanged(int score);

	// Declaration of the IPlayerLister interface //////////////////////////////

	void OnPlayerKilled(int lives_left);
	void OnLivesChanged(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

	// IPowerUpListener
	void OnPowerUpCollected(PowerUpType type);


private:
	GameState mState;

	shared_ptr<Spaceship> mSpaceship;

	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	shared_ptr<GUILabel> mTitleLabel;
	shared_ptr<GUILabel> mMenuLabel;
	shared_ptr<GUILabel> mInstructionsLabel;
	shared_ptr<GUILabel> mHighScoresLabel;
	shared_ptr<GUILabel> mNameEntryLabel;
	shared_ptr<GUILabel> mDifficultyLabel;
	shared_ptr<GUILabel> mPowerUpStatusLabel;


	uint mLevel;
	uint mAsteroidCount;
	bool mPowerUpsEnabled;
	std::string mPlayerNameInput;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateGUI();
	void CreateAsteroids(const uint num_asteroids);
	shared_ptr<GameObject> CreateExplosion();
	std::list< weak_ptr<GameObject> > mSpawnedObjects;



	void ChangeState(GameState new_state);
    void StartNewGame();
    void ResetGame();
    void SpawnPowerUp();
    void UpdatePowerUpLabel(const std::string& text, int duration_ms);
	
	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;
	const static uint SPAWN_POWERUP     = 3;
    const static uint CLEAR_POWERUP_LBL = 4;
    const static uint GO_TO_NAME_ENTRY  = 5;


	ScoreKeeper mScoreKeeper;
	Player mPlayer;
	HighScoreManager mHighScores;
};

#endif