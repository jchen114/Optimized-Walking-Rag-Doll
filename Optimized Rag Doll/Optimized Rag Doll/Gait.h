#pragma once

#include "State.h"
#include "Gains.h"
#include <vector>

class Gait
{
public:
	Gait(
		State s0, State s1, State s2, State s3, State s4, 
		Gains torso_gains, Gains ull_gains, Gains url_gains, Gains lll_gains, Gains lrl_gains, Gains lf_gains, Gains rf_gains,
		float cd_1, float cv_1, float cd_2, float cv_2,
		float time
		);

	Gait(std::vector<State> states, std::vector<Gains> gains, std::vector<float> feedbacks, float time);

	~Gait();

	std::vector<State> GetStates() const;
	std::vector<Gains> GetGains() const;
	std::vector<float> GetFdbk() const;
	float GetStateTime() const;

private:
	State m_s0;
	State m_s1;
	State m_s2;
	State m_s3;
	State m_s4;

	Gains m_torso_gains;
	Gains m_ull_gains;
	Gains m_url_gains;
	Gains m_lll_gains;
	Gains m_lrl_gains;
	Gains m_lf_gains;
	Gains m_rf_gains;

	float m_cd_1, m_cv_1, m_cd_2, m_cv_2;

	float m_state_time;

};

