#include "GameUtil.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "Spaceship.h"
#include "BoundingSphere.h"

using namespace std;

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/**  Default constructor. */
Spaceship::Spaceship()
	: GameObject("Spaceship"), mThrust(0), mInvulnerabilityTime(0), mWeaponUpgradeTime(0), mBlinkTimer(0), mBlinkVisible(true)
{
}

/** Construct a spaceship with given position, velocity, acceleration, angle, and rotation. */
Spaceship::Spaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r)
	: GameObject("Spaceship", p, v, a, h, r), mThrust(0), mInvulnerabilityTime(0), mWeaponUpgradeTime(0), mBlinkTimer(0), mBlinkVisible(true)
{
}

/** Copy constructor. */
Spaceship::Spaceship(const Spaceship& s)
	: GameObject(s), mThrust(0), mInvulnerabilityTime(0), mWeaponUpgradeTime(0), mBlinkTimer(0), mBlinkVisible(true)
{
}

/** Destructor. */
Spaceship::~Spaceship(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Update this spaceship. */
void Spaceship::Update(int t)
{
	// Call parent update function
	GameObject::Update(t);

	// Tick down power up timers
	if (mInvulnerabilityTime > 0) {
        mInvulnerabilityTime -= t;
        if (mInvulnerabilityTime < 0) mInvulnerabilityTime = 0;

        // Blink every 100ms while invulnerable
        mBlinkTimer += t;
        if (mBlinkTimer > 100) {
            mBlinkVisible = !mBlinkVisible;
            mBlinkTimer = 0;
        }

        // Reset visibility when invulnerability ends
        if (mInvulnerabilityTime == 0) mBlinkVisible = true;
    }

    if (mWeaponUpgradeTime > 0) {
        mWeaponUpgradeTime -= t;
        if (mWeaponUpgradeTime < 0) mWeaponUpgradeTime = 0;
    }
}

/** Render this spaceship. */
void Spaceship::Render(void)
{
	// Skip rendering on alternate frames while invulneerable for a blink effect
    if (IsInvulnerable() && !mBlinkVisible) {
        return;
    }

	if (mSpaceshipShape.get() != NULL) mSpaceshipShape->Render();

	// If ship is thrusting
	if ((mThrust > 0) && (mThrusterShape.get() != NULL)) {
		mThrusterShape->Render();
	}

	GameObject::Render();
}

/** Fire the rockets. */
void Spaceship::Thrust(float t)
{
	mThrust = t;
	// Increase acceleration in the direction of ship
	mAcceleration.x = mThrust*cos(DEG2RAD*mAngle);
	mAcceleration.y = mThrust*sin(DEG2RAD*mAngle);
}

/** Set the rotation. */
void Spaceship::Rotate(float r)
{
	mRotation = r;
}

/** Shoot a bullet (or 3 bullets when weapon upgrade is active). */
void Spaceship::Shoot(void)
{
	// Check the world exists
	if (!mWorld) return;

	// Calculate how fast the bullet should travel
	float bullet_speed = 30;

	// If weapon upgrade is active, fire 3 bullets (like a shotgun), else fire 1 straight
	int num_shots = HasWeaponUpgrade() ? 3 : 1;
    float angle_offsets[3] = { 0.0f, -15.0f, 15.0f };

	for (int i = 0; i < num_shots; ++i)
    {
        float fire_angle = mAngle + angle_offsets[i];
		// Construct a unit length vector in the direction the spaceship is headed
		GLVector3f bullet_heading(cos(DEG2RAD*fire_angle), sin(DEG2RAD*fire_angle), 0);
		bullet_heading.normalize();

		// Calculate the point at the node of the spaceship from position and heading
		GLVector3f bullet_position = mPosition + (bullet_heading * 4);
		// Construct a vector for the bullet's velocity
		GLVector3f bullet_velocity = mVelocity + bullet_heading * bullet_speed;

		// Construct a new bullet
		shared_ptr<GameObject> bullet
			(new Bullet(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 2000));
		bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
		bullet->SetShape(mBulletShape);
		// Add the new bullet to the game world
		mWorld->AddObject(bullet);
    }
}

bool Spaceship::CollisionTest(shared_ptr<GameObject> o)
{
	// Ignore asteroid collisions if invulnerabble
	if (IsInvulnerable()) return false;

	if (o->GetType() != GameObjectType("Asteroid")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Spaceship::OnCollision(const GameObjectList &objects)
{
	if (IsInvulnerable()) return;
	for (GameObjectList::const_iterator it = objects.begin(); it != objects.end(); ++it)
	{
		if ((*it)->GetType() == GameObjectType("Asteroid"))
		{
			mWorld->FlagForRemoval(GetThisPtr());
			return;
		}
	}
}

void Spaceship::OnPowerUpCollected(PowerUpType type)
{
    switch (type)
    {
    case POWERUP_INVULNERABILITY:
        mInvulnerabilityTime = 5000;
        mBlinkTimer = 0;
        mBlinkVisible = true;
        break;
    case POWERUP_WEAPON_UPGRADE:
        mWeaponUpgradeTime = 10000;
        break;
    case POWERUP_EXTRA_LIFE:
        // Handled by Playes object
        break;
    default:
        break;
    }
}