#include "stdafx.h"
#include "InterpolationManager.h"
#include "Gait.h"

#include "WalkingController.h"
#include "RagDollApplication.h"

InterpolationManager::InterpolationManager(WalkingController *walkingController, RagDollApplication *ragDollApp):PolationManager(walkingController, ragDollApp)
{

}


InterpolationManager::~InterpolationManager()
{
}

void InterpolationManager::Begin(std::string begin_gait, std::string end_gait, std::string gait_name) {
	PolationManager::Begin(begin_gait, end_gait, gait_name);
	printf("Interpolation Begin \n");

	// Check here if the gait name already exists. If it does, then pop up some message.

	// Ask WalkingController for begin gaits and end gaits
	// Interpolation manager: Hello Walking controller, may I have the gait named "_____".
	// Walking Controller: Sure, here you go.

	Gait beginning_gait = m_WalkingController->GetGait(begin_gait);
	Gait ending_gait = m_WalkingController->GetGait(end_gait);

	// Interpolate between the gaits
	Gait interpolated_gait = Interpolate(beginning_gait, ending_gait);

	// Give new gait to Walking Controller
	m_WalkingController->AddGait(interpolated_gait, gait_name);
	// Register callback with Walking Controller to save body states
	m_WalkingController->SetCallbackFunction(std::bind(&InterpolationManager::RecordStates, this, _1, _2, _3, _4));
	m_WalkingController->ChangeGait(m_gaitName);
	// Save everything
	m_RagDollApp->SaveStates();
	m_RagDollApp->SaveGains();
	m_RagDollApp->SaveFeedback();
	m_RagDollApp->SaveTime();
	// Tell Rag Doll to start walking
	//m_RagDollApp->Start();

}

void InterpolationManager::End() {
	printf("Interpolation End \n");
	m_RagDollApp->Reset();
}

Gait InterpolationManager::Interpolate(const Gait &begin_gait, const Gait &end_gait) {
	std::vector<State> b_states = begin_gait.GetStates();
	std::vector<State> e_states = end_gait.GetStates();

	std::vector<State> i_states;

	for (int i = 0; i < b_states.size(); i++) {
		float i_torsoAngle = (e_states.at(i).m_torsoAngle + b_states.at(i).m_torsoAngle) / 2;
		float i_ullAngle = (e_states.at(i).m_upperLeftLegAngle + b_states.at(i).m_upperLeftLegAngle) / 2;
		float i_urlAngle = (e_states.at(i).m_upperRightLegAngle + b_states.at(i).m_upperRightLegAngle) / 2;
		float i_lllAngle = (e_states.at(i).m_lowerLeftLegAngle + b_states.at(i).m_lowerLeftLegAngle) / 2;
		float i_lrlAngle = (e_states.at(i).m_lowerRightLegAngle + b_states.at(i).m_lowerRightLegAngle) / 2;
		float i_lfAngle = (e_states.at(i).m_leftFootAngle + b_states.at(i).m_leftFootAngle) / 2;
		float i_rfAngle = (e_states.at(i).m_rightFootAngle + b_states.at(i).m_rightFootAngle) / 2;

		State i_state(i_torsoAngle, i_ullAngle, i_urlAngle, i_lllAngle, i_lrlAngle, i_lfAngle, i_rfAngle);
		i_states.push_back(i_state);
	}

	std::vector<Gains> b_gains = begin_gait.GetGains();
	std::vector<Gains> e_gains = end_gait.GetGains();

	std::vector<Gains> i_gains;

	for (int i = 0; i < b_gains.size(); i++) {
		float kp = (e_gains.at(i).m_kp + b_gains.at(i).m_kp) / 2;
		float kd = (e_gains.at(i).m_kd + b_gains.at(i).m_kd) / 2;
		Gains gains(kp, kd, (AssociatedBody)i);
		i_gains.push_back(gains);
	}

	std::vector<float> b_fdbk = begin_gait.GetFdbk();
	std::vector<float> e_fdbk = end_gait.GetFdbk();

	std::vector<float> i_fdbk;

	for (int i = 0; i < b_fdbk.size(); i++) {
		float fdbk = (e_fdbk.at(i) + b_fdbk.at(i))/2;
		i_fdbk.push_back(fdbk);
	}

	float i_time = (begin_gait.GetStateTime() + end_gait.GetStateTime());

	Gait i_gait(i_states, i_gains, i_fdbk, i_time);

	return i_gait;

}