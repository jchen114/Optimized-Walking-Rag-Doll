#include "stdafx.h"
#include "ExtrapolationManager.h"
#include "Gait.h"

#include "WalkingController.h"
#include "RagDollApplication.h"

#include <iostream>
#include <fstream>


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
	m_RagDollApp->ChangeGait(m_gait_name);

	// Save gait
	m_RagDollApp->SaveStates();
	m_RagDollApp->SaveGains();
	m_RagDollApp->SaveFeedback();
	m_RagDollApp->SaveTime();

	// Start gait
	m_RagDollApp->Start();

	// Time the gait.
	m_clock.reset();

}

Gait ExtrapolationManager::Extrapolate(const Gait &begin_gait, const Gait &end_gait, const bool &first_time) {
	std::vector<State> b_states = begin_gait.GetStates();
	std::vector<State> e_states = end_gait.GetStates();

	m_beginning_gait = begin_gait;
	m_ending_gait = end_gait;

	std::vector<State> ex_states;

	if (first_time) {

		for (int i = 0; i < b_states.size(); i++) {

			float d_torso = (e_states.at(i).m_torsoAngle - b_states.at(i).m_torsoAngle) * EXTRAPOLATION_FACTOR;
			float d_ull = (e_states.at(i).m_upperLeftLegAngle - b_states.at(i).m_upperLeftLegAngle) * EXTRAPOLATION_FACTOR;
			float d_url = (e_states.at(i).m_upperRightLegAngle - b_states.at(i).m_upperRightLegAngle) * EXTRAPOLATION_FACTOR;
			float d_lll = (e_states.at(i).m_lowerLeftLegAngle - b_states.at(i).m_lowerLeftLegAngle) * EXTRAPOLATION_FACTOR;
			float d_lrl = (e_states.at(i).m_lowerRightLegAngle - b_states.at(i).m_lowerRightLegAngle) * EXTRAPOLATION_FACTOR;
			float d_lf = (e_states.at(i).m_leftFootAngle - b_states.at(i).m_leftFootAngle) * EXTRAPOLATION_FACTOR;
			float d_rf = (e_states.at(i).m_rightFootAngle - b_states.at(i).m_rightFootAngle) * EXTRAPOLATION_FACTOR;

			float ex_torsoAngle = e_states.at(i).m_torsoAngle + d_torso;
			float ex_ullAngle = e_states.at(i).m_upperLeftLegAngle + d_ull;
			float ex_urlAngle = e_states.at(i).m_upperRightLegAngle + d_url;
			float ex_lllAngle = e_states.at(i).m_lowerLeftLegAngle + d_lll;
			float ex_lrlAngle = e_states.at(i).m_lowerRightLegAngle + d_lrl;
			float ex_lfAngle = e_states.at(i).m_leftFootAngle + d_lf;
			float ex_rfAngle = e_states.at(i).m_rightFootAngle + d_rf;

			State ex_state(ex_torsoAngle, ex_ullAngle, ex_urlAngle, ex_lllAngle, ex_lrlAngle, ex_lfAngle, ex_rfAngle);

			std::vector<float> deltas = { d_torso, d_ull, d_url, d_lll, d_lrl, d_lf, d_rf };
			m_delta_states.push_back(deltas);

			ex_states.push_back(ex_state);
		}
	}
	else {
		for (int i = 0; i < b_states.size(); i++) {

			float d_torso = m_delta_states.at(i).at(0);
			float d_ull = m_delta_states.at(i).at(1);
			float d_url = m_delta_states.at(i).at(2);
			float d_lll = m_delta_states.at(i).at(3);
			float d_lrl = m_delta_states.at(i).at(4);
			float d_lf = m_delta_states.at(i).at(5);
			float d_rf = m_delta_states.at(i).at(6);

			float ex_torsoAngle = e_states.at(i).m_torsoAngle + d_torso;
			float ex_ullAngle = e_states.at(i).m_upperLeftLegAngle + d_ull;
			float ex_urlAngle = e_states.at(i).m_upperRightLegAngle + d_url;
			float ex_lllAngle = e_states.at(i).m_lowerLeftLegAngle + d_lll;
			float ex_lrlAngle = e_states.at(i).m_lowerRightLegAngle + d_lrl;
			float ex_lfAngle = e_states.at(i).m_leftFootAngle + d_lf;
			float ex_rfAngle = e_states.at(i).m_rightFootAngle + d_rf;

			State ex_state(ex_torsoAngle, ex_ullAngle, ex_urlAngle, ex_lllAngle, ex_lrlAngle, ex_lfAngle, ex_rfAngle);
			ex_states.push_back(ex_state);
		}
	}

	std::vector<Gains> b_gains = begin_gait.GetGains();
	std::vector<Gains> e_gains = end_gait.GetGains();

	std::vector<Gains> ex_gains;

	if (first_time) {
		for (int i = 0; i < b_gains.size(); i++) {
			float d_kp = (e_gains.at(i).m_kp - b_gains.at(i).m_kp) * EXTRAPOLATION_FACTOR;
			float d_kd = (e_gains.at(i).m_kd - b_gains.at(i).m_kd) * EXTRAPOLATION_FACTOR;
			float kp = e_gains.at(i).m_kp + d_kp;
			float kd = e_gains.at(i).m_kd + d_kd;
			
			std::vector<float> deltas = { d_kp, d_kd };
			m_delta_gains.push_back(deltas);

			Gains gains(kp, kd, (AssociatedBody)i);
			ex_gains.push_back(gains);
		}
	}
	else {
		for (int i = 0; i < b_gains.size(); i++) {
			float d_kp = m_delta_gains.at(i).at(0);
			float d_kd = m_delta_gains.at(i).at(1);

			float kp = e_gains.at(i).m_kp + d_kp;
			float kd = e_gains.at(i).m_kd + d_kd;

			Gains gains(kp, kd, (AssociatedBody)i);
			ex_gains.push_back(gains);
		}
	}

	std::vector<float> b_fdbk = begin_gait.GetFdbk();
	std::vector<float> e_fdbk = end_gait.GetFdbk();

	std::vector<float> ex_fdbk;

	if (first_time) {
		for (int i = 0; i < b_fdbk.size(); i++) {
			float d_fdbk = (e_fdbk.at(i) - b_fdbk.at(i)) * EXTRAPOLATION_FACTOR;

			float fdbk = e_fdbk.at(i) + d_fdbk;

			m_delta_fdbks.push_back(d_fdbk);

			ex_fdbk.push_back(fdbk);

		}
	}
	else {
		for (int i = 0; i < b_fdbk.size(); i++) {
			float d_fdbk = m_delta_fdbks.at(i);

			float fdbk = e_fdbk.at(i) + d_fdbk;
			ex_fdbk.push_back(fdbk);
		}
	}

	float ex_time;

	if (first_time) {
		float d_time = (end_gait.GetStateTime() - begin_gait.GetStateTime()) * EXTRAPOLATION_FACTOR;

		m_delta_time = d_time;
		//printf("m_delta_time = %f\n", d_time);
		ex_time = end_gait.GetStateTime() + d_time;
	}
	else {
		float d_time = m_delta_time;
		//printf("m_delta_time = %f\n", d_time);
		ex_time = end_gait.GetStateTime() + d_time;
	}

	Gait ex_gait(ex_states, ex_gains, ex_fdbk, ex_time);
	
	m_ending_gait = ex_gait;

	return ex_gait;
}

void ExtrapolationManager::RecordStates(std::vector<btVector3> linVec, std::vector<btVector3> linPos, std::vector<float> angVec, std::vector<float> angPos) {

	static const auto runOnce = [] { std::cout << "Received callback" << std::endl; return true; }();

	if (m_clock.getTimeSeconds() >= 20) {
		if (m_WalkingController->m_ragDollState == STATE_5) {
			// unsuccessful.
			printf("Unsuccessful gait. \n");
		}
		else {
			printf("Successful gait. \n");

			// save all the data for the doll before returning to state 1
			std::ofstream dat_file;
			std::string file_name = m_gait_dir + "\\info.dat";

			printf("save data to %s\n", file_name.c_str());
			dat_file.open(file_name);

			for (int body = 0; body < linVec.size(); body ++) {
				btVector3 linV = linVec.at(body);
				btVector3 linP = linPos.at(body);
				float angV = angVec.at(body);
				float angP = angPos.at(body);
				char buffer[200];
				sprintf_s(buffer, "%f, %f | %f, %f | %f | %f \n", linV.x(), linV.y(), linP.x(), linP.y(), angV, angP);
				//std::cout << buffer;
				dat_file << buffer;
			}
			dat_file.close();

			// After writing, Extrapolate further. 			
			Gait ex_gait = Extrapolate(m_beginning_gait, m_ending_gait, false);

			m_gait_name = m_gait_name + "-e";

			// Add gait
			m_WalkingController->AddGait(ex_gait, m_gait_name);

			// Change gait
			printf("Change to next extrapolated gait\n");
			m_RagDollApp->ChangeGait(m_gait_name);

			// Save Previous gait
			m_RagDollApp->SaveStates();
			m_RagDollApp->SaveGains();
			m_RagDollApp->SaveFeedback();
			m_RagDollApp->SaveTime();

		}

		m_clock.reset();
	}

}

void ExtrapolationManager::End() {
	printf("Extrapolation End\n");
}