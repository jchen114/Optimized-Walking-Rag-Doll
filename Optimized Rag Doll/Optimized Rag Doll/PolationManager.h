#pragma once

#include<string>
#include "LinearMath\btVector3.h"
#include <functional>
#include <vector>
#include "LinearMath\btQuickprof.h"

class WalkingController;
class RagDollApplication;

using namespace std::placeholders;

class PolationManager
{
public:
	PolationManager(WalkingController *walkingController, RagDollApplication *ragDollApp);
	~PolationManager();

	virtual void Begin(std::string begin_gait, std::string end_gait, std::string gait_name);
	virtual void End();

	virtual void RecordStates(std::vector<btVector3> linVec, std::vector<btVector3> linPos, std::vector<float> angVec, std::vector<float> angPos);


protected:
	WalkingController *m_WalkingController;
	RagDollApplication *m_RagDollApp;

	std::string m_gait_name;

	
};

