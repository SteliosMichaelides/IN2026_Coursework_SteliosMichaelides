#include "PowerUp.h"
#include "GameWorld.h"
#include "BoundingShape.h"
#include "GameObjectType.h"

// Initialise static listener 
IPowerUpListener* PowerUp::sListener = NULL;

PowerUp::PowerUp(const char* type_name, PowerUpType type)
    : GameObject(type_name), mPowerUpType(type), mTimeToLive(15000)
{
    // Move slowly
    float angle = (float)(rand() % 360);
    mVelocity.x = 3.0f * cos(DEG2RAD * angle);
    mVelocity.y = 3.0f * sin(DEG2RAD * angle);
    mRotation = 30.0f;
}

PowerUp::~PowerUp(void) {}

void PowerUp::Update(int t)
{
    GameObject::Update(t);

    mTimeToLive -= t;
    if (mTimeToLive <= 0 && mWorld) {
        mWorld->FlagForRemoval(GetThisPtr());
    }
}

bool PowerUp::CollisionTest(shared_ptr<GameObject> o)
{
    // Only collide with spaceship
    if (o->GetType() != GameObjectType("Spaceship")) return false;
    if (mBoundingShape.get() == NULL) return false;
    if (o->GetBoundingShape().get() == NULL) return false;
    return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void PowerUp::OnCollision(const GameObjectList& objects)
{
    bool ship_collided = false;
    for (GameObjectList::const_iterator it = objects.begin(); it != objects.end(); ++it)
    {
        if ((*it)->GetType() == GameObjectType("Spaceship")) {
            ship_collided = true;
            break;
        }
    }

    // Ignore the collision if its not with the spaceship
    if (!ship_collided) return;

    if (sListener != NULL) {
        sListener->OnPowerUpCollected(mPowerUpType);
    }
    // Remove powerup
    if (mWorld) mWorld->FlagForRemoval(GetThisPtr());
}