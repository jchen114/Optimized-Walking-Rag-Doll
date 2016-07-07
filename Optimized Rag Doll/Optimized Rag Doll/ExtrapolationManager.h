#pragma once
#include "PolationManager.h"
class ExtrapolationManager :
	public PolationManager
{
public:
	ExtrapolationManager(WalkingController *m_WalkingController, RagDollApplication *ragDollApplication);
	~ExtrapolationManager();

	virtual void Begin(std::string begin_gait, std::string end_gait, std::string gait_name = "extrapolate") override;
	virtual void End() override;

};

