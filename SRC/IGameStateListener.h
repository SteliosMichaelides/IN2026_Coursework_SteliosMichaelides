#ifndef __IGAMESTATELISTENER_H__
#define __IGAMESTATELISTENER_H__

#include "GameState.h"

class IGameStateListener
{
public:
    virtual ~IGameStateListener() {}
    virtual void OnGameStateChanged(GameState new_state) = 0;
};

#endif