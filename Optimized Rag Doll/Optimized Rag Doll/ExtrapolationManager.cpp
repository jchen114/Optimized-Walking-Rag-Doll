#include "stdafx.h"
#include "ExtrapolationManager.h"
#include "Gait.h"

ExtrapolationManager::ExtrapolationManager(WalkingController *walkingController, RagDollApplication *ragDollApplication) :PolationManager(walkingController, ragDollApplication)
{
}


ExtrapolationManager::~ExtrapolationManager()
{
}

void ExtrapolationManager::Begin(std::string begin_gait, std::string end_gait, std::string gait_name) {
	printf("Extrapolation Begin\n");
	PolationManager::Begin(begin_gait, end_gait, gait_name);
}

void ExtrapolationManager::End() {
	printf("Extrapolation End\n");
}