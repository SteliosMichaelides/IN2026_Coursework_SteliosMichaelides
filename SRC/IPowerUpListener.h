#ifndef __IPOWERUPLISTENER_H__
#define __IPOWERUPLISTENER_H__

// The listener uses this to know which one to activate
enum PowerUpType
{
    POWERUP_EXTRA_LIFE,
    POWERUP_INVULNERABILITY,
    POWERUP_WEAPON_UPGRADE
};

class IPowerUpListener
{
public:
    virtual ~IPowerUpListener() {}
    virtual void OnPowerUpCollected(PowerUpType type) = 0;
};

#endif