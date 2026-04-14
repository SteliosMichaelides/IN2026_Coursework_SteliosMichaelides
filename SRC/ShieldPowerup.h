#ifndef __SHIELDPOWERUP_H__
#define __SHIELDPOWERUP_H__

#include "GameObject.h"

class Asteroids;

// When the spaceship collides with this pickup, a 5 second invulnerability shield is activated
class ShieldPowerup : public GameObject
{
public:
	ShieldPowerup(Asteroids* game);
	virtual ~ShieldPowerup(void);

	virtual void Update(int t);
	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);

private:
    Asteroids* mGame;
    int mTimeToLive;
};

#endif