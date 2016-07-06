#pragma once
#include "PolationManager.h"
class ExtrapolationManager :
	public PolationManager
{
public:
	ExtrapolationManager();
	~ExtrapolationManager();

	virtual void Begin() override;
	virtual void End() override;

};

