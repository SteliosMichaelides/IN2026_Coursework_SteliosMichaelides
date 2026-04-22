#ifndef __IPLAYERLISTENER_H__
#define __IPLAYERLISTENER_H__

class IPlayerListener
{
public:
	virtual ~IPlayerListener() {}

	virtual void OnPlayerKilled(int lives_left) = 0;

	virtual void OnLivesChanged(int lives_left) = 0;
};

#endif
