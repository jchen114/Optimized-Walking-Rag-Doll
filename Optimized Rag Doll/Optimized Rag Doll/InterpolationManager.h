#pragma once
#include "PolationManager.h"

class Gait;

class InterpolationManager :
	public PolationManager
{
public:
	InterpolationManager(WalkingController *walkingController, RagDollApplication *ragDollApp);
	~InterpolationManager();

	virtual void Begin(std::string begin_gait, std::string end_gait, std::string gait_name = "Interpolate") override;
	virtual void End() override;

	Gait Interpolate(const Gait &begin_gait, const Gait &end_gait);

};

