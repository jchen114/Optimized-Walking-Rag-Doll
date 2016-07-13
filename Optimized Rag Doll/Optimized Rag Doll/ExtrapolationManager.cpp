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

	// Get beginning and ending gaits.

	// Extrapolate from beginning to end

	// Change current gait

	// Save gait

	// Start gait

	// Time the gait.

	// Save the state information before entering the first state

	// If gait is successful, extrapolate futher by using the same gait.

}

void ExtrapolationManager::End() {
	printf("Extrapolation End\n");
}