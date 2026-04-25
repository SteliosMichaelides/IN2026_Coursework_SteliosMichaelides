#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include "PowerUp.h"
#include "ExtraLifePowerUp.h"
#include "InvulnerabilityPowerUp.h"
#include "WeaponUpgradePowerUp.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv),
	  mState(STATE_MENU),
	  mLevel(0),
	  mAsteroidCount(0),
	  mPowerUpsEnabled(true),
	  mPlayerNameInput("")
{
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat diffuse_light[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

	Animation* life_anim = AnimationManager::GetInstance().CreateAnimationFromFile("life", 256, 256, 256, 256, "heart.png");
	Animation* invuln_anim = AnimationManager::GetInstance().CreateAnimationFromFile("invuln", 256, 256, 256, 256, "shield.png");
	Animation* weapon_anim = AnimationManager::GetInstance().CreateAnimationFromFile("weapon", 256, 256, 256, 256, "pistol.png");

	// Create a spaceship and add it to the world
	// mGameWorld->AddObject(CreateSpaceship());

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	// High scores
	mHighScores.Load();

	// Create some decorative asteroids
	// No spaceship, no score, lives or legend visible
	CreateAsteroids(10);

	// Create the GUI
	CreateGUI();

	// Show the menu
	ChangeState(STATE_MENU);

	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	if (mState == STATE_MENU)
	{
		switch (key)
		{
		// Start Game
		case '1':
			StartNewGame();
			break;
		// Difficulty
		case '2':
			mPowerUpsEnabled = !mPowerUpsEnabled;
			{
				std::ostringstream oss;
				oss << "DIFFICULTY: " << (mPowerUpsEnabled ? "EASY (POWER-UPS ON)" : "HARD (POWER-UPS OFF)");
				mDifficultyLabel->SetText(oss.str());
			}
			break;
		// Instructions
		case '3':
			ChangeState(STATE_INSTRUCTIONS);
			break;
		// High Scores
		case '4':
			mHighScoresLabel->SetText(mHighScores.GetFormattedTable());
			ChangeState(STATE_HIGH_SCORES);
			break;
		}
		return;
	}

	if (mState == STATE_INSTRUCTIONS)
	{
		if (key == 'b' || key == 'B')
			ChangeState(STATE_MENU);
		return;
	}

	if (mState == STATE_HIGH_SCORES)
	{
		if (key == 'b' || key == 'B')
			ChangeState(STATE_MENU);
		return;
	}

	if (mState == STATE_GAME_OVER_ENTRY)
	{
		if (key == 13) // Enter
		{
			int final_score = mScoreKeeper.GetScore();
			mHighScores.AddScore(mPlayerNameInput.empty() ? "PLAYER" : mPlayerNameInput, final_score);
			mPlayerNameInput = "";
			ChangeState(STATE_MENU);
		}
		else if (key == 8) // Backspace
		{
			if (!mPlayerNameInput.empty())
				mPlayerNameInput.pop_back();
			mNameEntryLabel->SetText("ENTER NAME: " + mPlayerNameInput + "_");
		}
		else if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z') ||
				 (key >= '0' && key <= '9'))
		{
			if (mPlayerNameInput.size() < 10)
			{
				mPlayerNameInput += (char)key;
				mNameEntryLabel->SetText("ENTER NAME: " + mPlayerNameInput + "_");
			}
		}
		return;
	}

	if (mState == STATE_PLAYING)
	{
		switch (key)
		{
		case ' ':
			if (mSpaceship)
				mSpaceship->Shoot();
			break;
		default:
			break;
		}
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	// While playing, only the spaceship listens to arrow keys
	if (mState != STATE_PLAYING || !mSpaceship)
		return;

	switch (key)
	{
	// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP:
		mSpaceship->Thrust(10);
		break;
	// If left arrow key is pressed start rotating anti-clockwise
	case GLUT_KEY_LEFT:
		mSpaceship->Rotate(90);
		break;
	// If right arrow key is pressed start rotating clockwise
	case GLUT_KEY_RIGHT:
		mSpaceship->Rotate(-90);
		break;
	// Default case - do nothing
	default:
		break;
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	if (mState != STATE_PLAYING || !mSpaceship)
		return;

	switch (key)
	{
	// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP:
		mSpaceship->Thrust(0);
		break;
	// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT:
		mSpaceship->Rotate(0);
		break;
	// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT:
		mSpaceship->Rotate(0);
		break;
	// Default case - do nothing
	default:
		break;
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld *world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		if (mState == STATE_PLAYING && mAsteroidCount <= 0)
		{
			SetTimer(500, START_NEXT_LEVEL);
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
		// After 2 seconds go to name entry (only if is highscore)
		SetTimer(2000, GO_TO_NAME_ENTRY);
	}

	if (value == GO_TO_NAME_ENTRY)
	{
		int final_score = mScoreKeeper.GetScore();
		if (mHighScores.IsHighScore(final_score))
		{
			ChangeState(STATE_GAME_OVER_ENTRY);
		}
		else
		{
			// If not a highscore, we go back to main menu
			ChangeState(STATE_MENU);
		}
	}

	if (value == SPAWN_POWERUP)
	{
		if (mState == STATE_PLAYING && mPowerUpsEnabled)
		{
			SpawnPowerUp();
			// Schedule the next one in 15-25 seconds
			SetTimer(15000 + (rand() % 10000), SPAWN_POWERUP);
		}
	}

	if (value == CLEAR_POWERUP_LBL)
	{
		mPowerUpStatusLabel->SetText("");
	}
}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();

	// Make the spaceship the listener for powerups
	PowerUp::SetListener(this);

	// Track for cleanup when game resets
	mSpawnedObjects.push_back(weak_ptr<GameObject>(mSpaceship));

	// Return the spaceship so it can be added to the world
	return mSpaceship;
}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);

		// Track for cleanup when game resets
		mSpawnedObjects.push_back(weak_ptr<GameObject>(asteroid));
	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mScoreLabel->SetVisible(false);
	// Add the GUILabel to the GUIComponent
	shared_ptr<GUIComponent> score_component = static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	mLivesLabel->SetVisible(false);
	// Add the GUILabel to the GUIComponent
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer
	shared_ptr<GUIComponent> game_over_component = static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

	// Title (menu)
	mTitleLabel = make_shared<GUILabel>("ASTEROIDS");
	mTitleLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mTitleLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mTitleLabel->SetVisible(true);
	shared_ptr<GUIComponent> title_component = static_pointer_cast<GUIComponent>(mTitleLabel);
	mGameDisplay->GetContainer()->AddComponent(title_component, GLVector2f(0.5f, 0.85f));

	// Menu Options
	mMenuLabel = make_shared<GUILabel>("1: START GAME    2: DIFFICULTY    3: INSTRUCTIONS    4: HIGH SCORES");
	mMenuLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mMenuLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mMenuLabel->SetVisible(true);
	shared_ptr<GUIComponent> menu_component = static_pointer_cast<GUIComponent>(mMenuLabel);
	mGameDisplay->GetContainer()->AddComponent(menu_component, GLVector2f(0.5f, 0.6f));

	// Difficulty Status (menu)
	mDifficultyLabel = make_shared<GUILabel>("DIFFICULTY: EASY (POWER-UPS ON)");
	mDifficultyLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mDifficultyLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mDifficultyLabel->SetVisible(true);
	shared_ptr<GUIComponent> difficulty_component = static_pointer_cast<GUIComponent>(mDifficultyLabel);
	mGameDisplay->GetContainer()->AddComponent(difficulty_component, GLVector2f(0.5f, 0.5f));

	// Instructions
	mInstructionsLabel = make_shared<GUILabel>("INSTRUCTIONS    ARROW KEYS: MOVE/TURN    SPACE: SHOOT    COLLECT POWER-UPS    PRESS B TO RETURN TO MENU");
	mInstructionsLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mInstructionsLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mInstructionsLabel->SetVisible(false);
	shared_ptr<GUIComponent> instructions_component = static_pointer_cast<GUIComponent>(mInstructionsLabel);
	mGameDisplay->GetContainer()->AddComponent(instructions_component, GLVector2f(0.5f, 0.5f));

	// High Scores
	mHighScoresLabel = make_shared<GUILabel>("HIGH SCORES");
	mHighScoresLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mHighScoresLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mHighScoresLabel->SetVisible(false);
	shared_ptr<GUIComponent> high_scores_component = static_pointer_cast<GUIComponent>(mHighScoresLabel);
	mGameDisplay->GetContainer()->AddComponent(high_scores_component, GLVector2f(0.5f, 0.5f));

	// Name Entry
	mNameEntryLabel = make_shared<GUILabel>("ENTER NAME: _");
	mNameEntryLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mNameEntryLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mNameEntryLabel->SetVisible(false);
	shared_ptr<GUIComponent> name_entry_component = static_pointer_cast<GUIComponent>(mNameEntryLabel);
	mGameDisplay->GetContainer()->AddComponent(name_entry_component, GLVector2f(0.5f, 0.4f));

	// Power-Up Status
	mPowerUpStatusLabel = make_shared<GUILabel>("");
	mPowerUpStatusLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_RIGHT);
	mPowerUpStatusLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	mPowerUpStatusLabel->SetVisible(true);
	shared_ptr<GUIComponent> powerup_status_component = static_pointer_cast<GUIComponent>(mPowerUpStatusLabel);
	mGameDisplay->GetContainer()->AddComponent(powerup_status_component, GLVector2f(1.0f, 1.0f));

	// Legend
	mLegendLabel = make_shared<GUILabel>("POWER-UPS: HEART=+1 LIFE  SHIELD=INVULNERABLE  PISTOL=SPREAD-SHOT");
	mLegendLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_RIGHT);
	mLegendLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	mLegendLabel->SetVisible(false);
	shared_ptr<GUIComponent> legend_component = static_pointer_cast<GUIComponent>(mLegendLabel);
	mGameDisplay->GetContainer()->AddComponent(legend_component, GLVector2f(1.0f, 0.0f));
}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0) {
        SetTimer(1000, CREATE_NEW_PLAYER);
    } else {
        SetTimer(500, SHOW_GAME_OVER);
    }
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}

void Asteroids::ChangeState(GameState new_state)
{
	mState = new_state;

	// First hide everything
	mTitleLabel->SetVisible(false);
	mMenuLabel->SetVisible(false);
	mDifficultyLabel->SetVisible(false);
	mInstructionsLabel->SetVisible(false);
	mHighScoresLabel->SetVisible(false);
	mNameEntryLabel->SetVisible(false);
	mScoreLabel->SetVisible(false);
	mLivesLabel->SetVisible(false);
	mGameOverLabel->SetVisible(false);

	switch (new_state)
	{
	case STATE_MENU:
		mTitleLabel->SetVisible(true);
		mMenuLabel->SetVisible(true);
		mDifficultyLabel->SetVisible(true);
		break;
	case STATE_INSTRUCTIONS:
		mInstructionsLabel->SetVisible(true);
		break;
	case STATE_HIGH_SCORES:
		mHighScoresLabel->SetVisible(true);
		break;
	case STATE_PLAYING:
		mScoreLabel->SetVisible(true);
		mLivesLabel->SetVisible(true);
		mLegendLabel->SetVisible(true);
		break;
	case STATE_GAME_OVER_ENTRY:
		mGameOverLabel->SetVisible(true);
		mNameEntryLabel->SetText("ENTER NAME: _");
		mNameEntryLabel->SetVisible(true);
		break;
	}
}

void Asteroids::StartNewGame()
{
	ResetGame();

	// Spawn spaceship
	mGameWorld->AddObject(CreateSpaceship());
	// Spawn asteroids
	CreateAsteroids(10);
	// Switch state and set text
	ChangeState(STATE_PLAYING);

	// Schedule first powerup if enabled
	if (mPowerUpsEnabled)
	{
		SetTimer(1500, SPAWN_POWERUP);
	}
}

void Asteroids::ResetGame()
{
	// Flag and clear every object that was spawned
	for (std::list< weak_ptr<GameObject> >::iterator it = mSpawnedObjects.begin(); it != mSpawnedObjects.end(); ++it)
	{
		shared_ptr<GameObject> obj = it->lock();
		if (obj) 
		{
			mGameWorld->FlagForRemoval(obj);
		}
	}
	mSpawnedObjects.clear();

	mLevel = 0;
	mAsteroidCount = 0;
	mPlayer.Reset(); // Resets lives to 3
	mScoreKeeper.Reset(); // Resets score to 0
	mPowerUpStatusLabel->SetText("");
}

void Asteroids::SpawnPowerUp()
{
	// Pick a random type
	int choice = rand() % 3;
	shared_ptr<GameObject> pu;
	const char* anim_name = NULL;

	switch (choice)
	{
	case 0:
		pu = make_shared<ExtraLifePowerUp>();
		anim_name = "life";
		break;
	case 1:
		pu = make_shared<InvulnerabilityPowerUp>();
		anim_name = "invuln";
		break;
	case 2:
		pu = make_shared<WeaponUpgradePowerUp>();
		anim_name = "weapon";
		break;
	}

	pu->SetBoundingShape(make_shared<BoundingSphere>(pu->GetThisPtr(), 6.0f));

	// Use each power up's sprite
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName(anim_name);
	shared_ptr<Sprite> sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	sprite->SetLoopAnimation(true);
	pu->SetSprite(sprite);
	pu->SetScale(0.08f);

	// Random spawn position inside world bounds
	GLVector3f pos((float)((rand() % 200) - 100), (float)((rand() % 200) - 100), 0.0f);
	pu->SetPosition(pos);

	mGameWorld->AddObject(pu);

	// Track for cleanup when game resets
	mSpawnedObjects.push_back(weak_ptr<GameObject>(pu));
}

void Asteroids::OnPowerUpCollected(PowerUpType type)
{
	if (mSpaceship) mSpaceship->OnPowerUpCollected(type);

	switch (type)
	{
	case POWERUP_EXTRA_LIFE:
		mPlayer.AddLife();
		UpdatePowerUpLabel("+1 LIFE!", 2000);
		break;
	case POWERUP_INVULNERABILITY:
		UpdatePowerUpLabel("INVULNERABLE 5s", 5000);
		break;
	case POWERUP_WEAPON_UPGRADE:
		UpdatePowerUpLabel("SPREAD SHOT 10s", 10000);
		break;
	}
}

void Asteroids::UpdatePowerUpLabel(const std::string &text, int duration_ms)
{
	mPowerUpStatusLabel->SetText(text);
	SetTimer(duration_ms, CLEAR_POWERUP_LBL);
}

void Asteroids::OnLivesChanged(int lives_left)
{
    std::ostringstream msg_stream;
    msg_stream << "Lives: " << lives_left;
	std::string lives_msg = msg_stream.str();
    mLivesLabel->SetText(lives_msg);
}
