#include "stdafx.h"
#include "Gait.h"

#include "State.h"
#include "Gains.h"

Gait::Gait() {}

Gait::Gait(
	State s0, State s1, State s2, State s3, State s4,
	Gains torso_gains, Gains ull_gains, Gains url_gains, Gains lll_gains, Gains lrl_gains, Gains lf_gains, Gains rf_gains,
	float cd_1, float cv_1, float cd_2, float cv_2,
	float time)
{
	m_s0 = s0;
	m_s1 = s1;
	m_s2 = s2;
	m_s3 = s3;
	m_s4 = s4;

	m_torso_gains = torso_gains;
	m_ull_gains = ull_gains;
	m_url_gains = url_gains;
	m_lll_gains = lll_gains;
	m_lrl_gains = lrl_gains;
	m_lf_gains = lf_gains;
	m_rf_gains = rf_gains;

	m_cd_1 = cd_1;
	m_cv_1 = cv_1;
	
	m_cd_2 = cd_2;
	m_cv_2 = cv_2;

	m_state_time = time;

}

Gait::Gait(std::vector<State> states, std::vector<Gains> gains, std::vector<float> feedbacks, float time) {
	m_s0 = states.at(0);
	m_s1 = states.at(1);
	m_s2 = states.at(2);
	m_s3 = states.at(3);
	m_s4 = states.at(4);

	m_torso_gains = gains.at(0);
	m_ull_gains = gains.at(1);
	m_url_gains = gains.at(2);
	m_lll_gains = gains.at(3);
	m_lrl_gains = gains.at(4);
	m_lf_gains = gains.at(5);
	m_rf_gains = gains.at(6);

	m_cd_1 = feedbacks.at(0);
	m_cv_1 = feedbacks.at(1);
	m_cd_2 = feedbacks.at(2);
	m_cv_2 = feedbacks.at(3);

	m_state_time = time;
}

std::vector<State> Gait::GetStates() const {
	return std::vector<State> {m_s0, m_s1, m_s2, m_s3, m_s4};
}

std::vector<Gains> Gait::GetGains() const {
	return std::vector <Gains> {m_torso_gains, m_ull_gains, m_url_gains, m_lll_gains, m_lrl_gains, m_lf_gains, m_rf_gains};
}

std::vector<float> Gait::GetFdbk() const {
	return std::vector < float > {m_cd_1, m_cv_1, m_cd_2, m_cv_2};
}

float Gait::GetStateTime() const {
	return m_state_time; 
}

Gait::~Gait()
{
}
