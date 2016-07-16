#pragma once
#include "PolationManager.h"
#include "Gait.h"

#define EXTRAPOLATION_FACTOR 0.1f

class ExtrapolationManager :
	public PolationManager
{
public:
	ExtrapolationManager(WalkingController *walkingController, RagDollApplication *ragDollApplication);
	~ExtrapolationManager();

	virtual void Begin(std::string begin_gait, std::string end_gait, std::string gait_name = "extrapolate") override;
	virtual void End() override;

	virtual void RecordStates(std::vector<btVector3> linVec, std::vector<btVector3> linPos, std::vector<float> angVec, std::vector<float> angPos) override;

	Gait Extrapolate(const Gait &begin_gait, const Gait &end_gait, const bool &first_time = true);

private:

	btClock m_clock;
	std::string m_gait_dir;
	Gait m_beginning_gait;
	Gait m_ending_gait;

	std::vector<std::vector<float>> m_delta_states;
	std::vector<std::vector<float>> m_delta_gains;
	std::vector<float> m_delta_fdbks;
	float m_delta_time;

};

