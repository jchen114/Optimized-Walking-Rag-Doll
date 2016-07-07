#include "stdafx.h"
#include "PolationManager.h"
#include "Gait.h"

#include "WalkingController.h"
#include "RagDollApplication.h"


PolationManager::PolationManager(WalkingController *walkingController, RagDollApplication *ragDollApp)
{

	m_WalkingController = walkingController;
	m_RagDollApp = ragDollApp;

}


PolationManager::~PolationManager()
{
}

void PolationManager::Begin(std::string begin_gait, std::string end_gait, std::string gait_name) {
	
	m_gaitName = gait_name;

}

void PolationManager::End() {

}

void PolationManager::RecordStates(std::vector<btVector3> linVec, std::vector<btVector3> linPos, std::vector<float> angVec, std::vector<float> angPos) {

	printf("Recording state\n");

}
