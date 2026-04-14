#include "GameWorld.h"
#include "ShieldPowerup.h"
#include "Asteroids.h"
#include "BoundingSphere.h"
#include <stdlib.h>

ShieldPowerup::ShieldPowerup(Asteroids* game)
    : GameObject("ShieldPowerUp"),
        mGame(game),
        mTimeToLive(10000)
{
    mPosition.x = (rand() % 300) - 150.0f;
	mPosition.y = (rand() % 300) - 150.0f;
	mPosition.z = 0.0f;

    float angle = static_cast<float>(rand() % 360);
    mVelocity.x = 5.0f * cos(DEG2RAD * angle);
	mVelocity.y = 5.0f * sin(DEG2RAD * angle);
	mVelocity.z = 0.0f;
}

ShieldPowerUp::~ShieldPowerUp(void) {}

void ShieldPowerUp::Update(int t)
{
    GameObject::Update(t);
	mTimeToLive -= t;
	if (mTimeToLive <= 0 && mWorld)
		mWorld->FlagForRemoval(GetThisPtr());
}

bool ShieldPowerup::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Spaceship")) return false;
	if (!mBoundingShape || !o->GetBoundingShape()) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void ShieldPowerup::OnCollision(const GameObjectList& objects)
{
	if (mGame) mGame->ActivateShield();
	if (mWorld) mWorld->FlagForRemoval(GetThisPtr());
}