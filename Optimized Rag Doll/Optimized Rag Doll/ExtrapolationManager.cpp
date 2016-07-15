#include "stdafx.h"
#include "ExtrapolationManager.h"
#include "Gait.h"

#include "WalkingController.h"
#include "RagDollApplication.h"

ExtrapolationManager::ExtrapolationManager(WalkingController *walkingController, RagDollApplication *ragDollApplication) :PolationManager(walkingController, ragDollApplication)
{
}


ExtrapolationManager::~ExtrapolationManager()
{
}

void ExtrapolationManager::Begin(std::string begin_gait, std::string end_gait, std::string gait_name) {
	printf("Extrapolation Begin\n");
	PolationManager::Begin(begin_gait, end_gait, gait_name);

	// Get beginning and ending gaits.
	Gait beginning_gait = m_WalkingController->GetGait(begin_gait);
	Gait ending_gait = m_WalkingController->GetGait(end_gait);

	// Extrapolate from beginning to end
	Gait extrapolatedGait = Extrapolate(beginning_gait, ending_gait);

	// Add gait to WalkingController
	m_WalkingController->AddGait(extrapolatedGait, gait_name);

	// Change current gait
	m_WalkingController->SetCallbackFunction(std::bind(&ExtrapolationManager::RecordStates, this, _1, _2, _3, _4));
	m_RagDollApp->ChangeGait(m_gaitName);
	// Save gait
	m_RagDollApp->SaveStates();
	m_RagDollApp->SaveGains();
	m_RagDollApp->SaveFeedback();
	m_RagDollApp->SaveTime();

	// Start gait
	m_RagDollApp->Start();

	// Time the gait.
	m_clock.reset();

	// If gait is successful, extrapolate futher by using the same gait.

}

Gait ExtrapolationManager::Extrapolate(const Gait &begin_gait, const Gait &end_gait) {
	std::vector<State> b_states = begin_gait.GetStates();
	std::vector<State> e_states = end_gait.GetStates();

	std::vector<State> ex_states;

	for (int i = 0; i < b_states.size(); i++) {
		float ex_torsoAngle = e_states.at(i).m_torsoAngle + (e_states.at(i).m_torsoAngle - b_states.at(i).m_torsoAngle) * EXTRAPOLATION_FACTOR;
		float ex_ullAngle = e_states.at(i).m_upperLeftLegAngle + (e_states.at(i).m_upperLeftLegAngle - b_states.at(i).m_upperLeftLegAngle) * EXTRAPOLATION_FACTOR;
		float ex_urlAngle = e_states.at(i).m_upperRightLegAngle + (e_states.at(i).m_upperRightLegAngle - b_states.at(i).m_upperRightLegAngle) * EXTRAPOLATION_FACTOR;
		float ex_lllAngle = e_states.at(i).m_lowerLeftLegAngle + (e_states.at(i).m_lowerLeftLegAngle - b_states.at(i).m_lowerLeftLegAngle) * EXTRAPOLATION_FACTOR;
		float ex_lrlAngle = e_states.at(i).m_lowerRightLegAngle + (e_states.at(i).m_lowerRightLegAngle - b_states.at(i).m_lowerRightLegAngle) * EXTRAPOLATION_FACTOR;
		float ex_lfAngle = e_states.at(i).m_leftFootAngle + (e_states.at(i).m_leftFootAngle - b_states.at(i).m_leftFootAngle) * EXTRAPOLATION_FACTOR;
		float ex_rfAngle = e_states.at(i).m_rightFootAngle + (e_states.at(i).m_rightFootAngle - b_states.at(i).m_rightFootAngle) * EXTRAPOLATION_FACTOR;

		State ex_state(ex_torsoAngle, ex_ullAngle, ex_urlAngle, ex_lllAngle, ex_lrlAngle, ex_lfAngle, ex_rfAngle);
		ex_states.push_back(ex_state);
	}

	std::vector<Gains> b_gains = begin_gait.GetGains();
	std::vector<Gains> e_gains = end_gait.GetGains();

	std::vector<Gains> ex_gains;

	for (int i = 0; i < b_gains.size(); i++) {
		float kp = e_gains.at(i).m_kp + (e_gains.at(i).m_kp - b_gains.at(i).m_kp) * EXTRAPOLATION_FACTOR;
		float kd = e_gains.at(i).m_kd + (e_gains.at(i).m_kd - b_gains.at(i).m_kd) * EXTRAPOLATION_FACTOR;
		Gains gains(kp, kd, (AssociatedBody)i);
		ex_gains.push_back(gains);
	}

	std::vector<float> b_fdbk = begin_gait.GetFdbk();
	std::vector<float> e_fdbk = end_gait.GetFdbk();

	std::vector<float> ex_fdbk;

	for (int i = 0; i < b_fdbk.size(); i++) {
		float fdbk = e_fdbk.at(i) + (e_fdbk.at(i) - b_fdbk.at(i)) * EXTRAPOLATION_FACTOR;
		ex_fdbk.push_back(fdbk);
	}

	float ex_time = end_gait.GetStateTime() + (end_gait.GetStateTime() - begin_gait.GetStateTime()) * EXTRAPOLATION_FACTOR;

	Gait ex_gait(ex_states, ex_gains, ex_fdbk, ex_time);

	return ex_gait;
}

void ExtrapolationManager::RecordStates(std::vector<btVector3> linVec, std::vector<btVector3> linPos, std::vector<float> angVec, std::vector<float> angPos) {

	static const auto runOnce = [] { std::cout << "Received callback" << std::endl; return true; }();

	if (m_clock.getTimeSeconds() >= 60) {
		if (m_WalkingController->m_ragDollState == STATE_5) {
			// unsuccessful.
			static const auto runOnce = [] { std::cout << "Unsuccessful" << std::endl; return true; }();
		}
		else {
			static const auto runOnce = [] { std::cout << "Successful" << std::endl; return true; }();
		}
	}

}

void ExtrapolationManager::End() {
	printf("Extrapolation End\n");
}