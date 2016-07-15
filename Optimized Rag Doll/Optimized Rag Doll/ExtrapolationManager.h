#pragma once
#include "PolationManager.h"

#define EXTRAPOLATION_FACTOR 0.1

class Gait;

class ExtrapolationManager :
	public PolationManager
{
public:
	ExtrapolationManager(WalkingController *walkingController, RagDollApplication *ragDollApplication);
	~ExtrapolationManager();

	virtual void Begin(std::string begin_gait, std::string end_gait, std::string gait_name = "extrapolate") override;
	virtual void End() override;

	virtual void RecordStates(std::vector<btVector3> linVec, std::vector<btVector3> linPos, std::vector<float> angVec, std::vector<float> angPos) override;

	Gait Extrapolate(const Gait &begin_gait, const Gait &end_gait);

private:

	btClock m_clock;

};

