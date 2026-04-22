#ifndef __ISCORELISTENER_H__
#define __ISCORELISTENER_H__

class IScoreListener
{
public:
	virtual ~IScoreListener() {}
	virtual void OnScoreChanged(int score) = 0;
};

#endif
