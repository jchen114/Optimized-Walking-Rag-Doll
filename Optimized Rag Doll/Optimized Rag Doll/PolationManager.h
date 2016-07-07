#pragma once

#include<string>
class WalkingController;
class RagDollApplication;

class PolationManager
{
public:
	PolationManager(WalkingController *walkingController, RagDollApplication *ragDollApp);
	~PolationManager();

	virtual void Begin(std::string begin_gait, std::string end_gait, std::string gait_name);
	virtual void End();


protected:
	WalkingController *m_WalkingController;
	RagDollApplication *m_RagDollApp;

	std::string m_gaitName;
};

