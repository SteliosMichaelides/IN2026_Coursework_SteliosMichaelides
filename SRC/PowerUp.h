#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "GameUtil.h"
#include "GameObject.h"
#include "IPowerUpListener.h"

class PowerUp : public GameObject
{
public:
    PowerUp(const char * type_name, PowerUpType type);
    virtual ~PowerUp(void);

    virtual void Update(int t);

    bool CollisionTest(shared_ptr<GameObject> o);
    void OnCollision(const GameObjectList & objects);

    PowerUpType GetPowerUpType() const { return mPowerUpType; }

    // Listener notified
    static void SetListener(IPowerUpListener * listener) { sListener = listener; }

protected:
    PowerUpType mPowerUpType;
    int mTimeToLive;

    static IPowerUpListener *sListener;
};

#endif