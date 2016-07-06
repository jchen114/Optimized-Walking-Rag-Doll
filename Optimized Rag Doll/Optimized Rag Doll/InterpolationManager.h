#pragma once
#include "PolationManager.h"
class InterpolationManager :
	public PolationManager
{
public:
	InterpolationManager();
	~InterpolationManager();

	virtual void Begin() override;
	virtual void End() override;

};

