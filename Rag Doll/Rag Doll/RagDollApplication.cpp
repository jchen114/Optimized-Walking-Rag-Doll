#include "stdafx.h"
#include "RagDollApplication.h"
#include "WalkingController.h"

#include "State.h"	
#include "Gains.h"	


#pragma region INITIALIZATION

static RagDollApplication *m_app;

enum ControlIDs { 
	TORSO_KP, TORSO_KD = 0, 
	UPPER_R_LEG_KP, UPPER_R_LEG_KD, 
	UPPER_L_LEG_KP, UPPER_L_LEG_KD, 
	LOWER_R_LEG_KP, LOWER_R_LEG_KD, 
	LOWER_L_LEG_KP, LOWER_L_LEG_KD, 
	R_FOOT_KP, R_FOOT_KD, 
	L_FOOT_KP, L_FOOT_KD };

enum StateIDs {
	TORSO_ANGLE = 13,
	ULL_ANGLE,
	URL_ANGLE,
	LLL_ANGLE,
	LRL_ANGLE,
	LF_ANGLE,
	RF_ANGLE
};

enum Button_IDs {
	RESET_BUTTON = 20,
	PAUSE_BUTTON,
	START_BUTTON,
	SAVESTATES_BUTTON,
	SAVEGAINS_BUTTON
};

#pragma region DEFINITIONS 

#define PI 3.14159265

// MASS
#define torso_mass 70
#define upper_leg_mass 5
#define lower_leg_mass 4
#define feet_mass 1

// DIMENSIONS
#define torso_width 1.0f
#define torso_height 3.0

#define upper_leg_height 3.0f
#define upper_leg_width 0.7f

#define lower_leg_height 3.0f
#define lower_leg_width 0.5f

#define foot_height 0.3f
#define foot_width 1.2f

// Gains
#define KP_LOWER 0.0f
#define KP_HIGHER 5.0f
#define KD_LOWER 0.0f
#define KD_HIGHER 5.0f

// Spinner limits
#define SPINNER_TORSO_LOW		0.0f
#define SPINNER_TORSO_HIGH		180.0f
#define SPINNER_UPPER_LEG_LOW	30.0f
#define SPINNER_UPPER_LEG_HIGH	180.0f
#define SPINNER_LOWER_LEG_LOW	0.0f
#define SPINNER_LOWER_LEG_HIGH	90.0f
#define SPINNER_FOOT_LOW		0.0f
#define SPINNER_FOOT_HIGH		90.0f

// Hinge limits
#define HINGE_TORSO_ULL_LOW		-90.0f
#define HINGE_TORSO_ULL_HIGH	90.0f

#define HINGE_TORSO_URL_LOW		-90.0f
#define HINGE_TORSO_URL_HIGH	90.0f

#define HINGE_ULL_LLL_LOW		90.0f
#define HINGE_ULL_LLL_HIGH		90.0f

#define HINGE_URL_LRL_LOW		90.0f
#define HINGE_URL_LRL_HIGH		90.0f

#define HINGE_LLL_LF_LOW		-90.0f
#define HINGE_LLL_LF_HIGH		0.0f

#define HINGE_LRL_RF_LOW		-90.0f
#define HINGE_LRL_RF_HIGH		0.0f

#define ORIGINAL_TORSO_POSITION btVector3(0, -9 + foot_height/2 + lower_leg_height + upper_leg_height + torso_height/2, 0.5)

#pragma endregion DEFINITIONS

RagDollApplication::RagDollApplication()
{
}

RagDollApplication::RagDollApplication(ProjectionMode mode) :BulletOpenGLApplication(mode){
	m_app = this;
}


RagDollApplication::~RagDollApplication()
{
}

void RagDollApplication::InitializePhysics() {

	// create the collision configuration
	m_pCollisionConfiguration = new btDefaultCollisionConfiguration();
	// create the dispatcher
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfiguration);

	// Adding for 2D collisions and solving
	m_pDispatcher->registerCollisionCreateFunc(BOX_2D_SHAPE_PROXYTYPE, BOX_2D_SHAPE_PROXYTYPE, new btBox2dBox2dCollisionAlgorithm::CreateFunc());

	// create the broadphase
	m_pBroadphase = new btDbvtBroadphase();
	// create the constraint solver
	m_pSolver = new btSequentialImpulseConstraintSolver();
	// create the world
	m_pWorld = new btDiscreteDynamicsWorld(m_pDispatcher, m_pBroadphase, m_pSolver, m_pCollisionConfiguration);

	// Create ground
	btVector3 ground(1.0f, 25.0f, 10.0f);
	float mass = 0.0f;
	btVector3 position(0.0f, -10.0f, 0.0f);
	Create3DBox(ground, mass, GetRandomColor(), position);

	CreateRagDoll(ORIGINAL_TORSO_POSITION);

	// Create Controller
	m_WalkingController = new WalkingController(this);

	// Read from a file for last state configuration
	m_states = m_WalkingController->ReadStateFile();

	// Read from a file for last gains configuration
	m_gains = m_WalkingController->ReadGainsFile();

	// Create GUI
	CreateRagDollGUI();

	// Setup GUI with configurations
	SetupGUIConfiguration(m_states, m_gains);

	Reset();
}

void RagDollApplication::Idle() {
	BulletOpenGLApplication::Idle();

	switch (m_WalkingController->m_currentState)
	{
		case WALKING: {
			
		}
			break;
		case PAUSE:
			break;
		case RESET:
			break;
		default:
			break;
	}

}

void RagDollApplication::CreateRagDollGUI() {

	printf("Create Glui Window \n");

	// Setup
	GLUI_Master.set_glutIdleFunc(RagDollIdle);
	m_glui_window = GLUI_Master.create_glui("Rag Doll Controls", 5, 0);
	m_glui_window->set_main_gfx_window(m_main_window_id);

	// Controls
	/*===================================== GAINS =========================================*/
	GLUI_Panel *gains_panel = m_glui_window->add_panel("Gains");
	m_glui_window->add_statictext_to_panel(gains_panel, "Torso");
	m_torso_kp_spinner =  m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, &m_gains.at(0)->m_kp, TORSO_KP);
	m_torso_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, &m_gains.at(0)->m_kd, TORSO_KD);

	m_torso_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_torso_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Upper Left Leg");
	m_ull_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, &m_gains.at(1)->m_kp, UPPER_L_LEG_KP);
	m_ull_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, &m_gains.at(1)->m_kp, UPPER_L_LEG_KD);

	m_ull_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_ull_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Upper Right Leg");
	m_url_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, &m_gains.at(2)->m_kp, UPPER_R_LEG_KP);
	m_url_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, &m_gains.at(2)->m_kp, UPPER_R_LEG_KD);

	m_url_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_url_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Lower Left Leg");
	m_lll_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, &m_gains.at(3)->m_kp, LOWER_L_LEG_KP);
	m_lll_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, &m_gains.at(3)->m_kp, LOWER_L_LEG_KD);

	m_lll_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_lll_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Lower Right Leg");
	m_lrl_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, &m_gains.at(4)->m_kp, LOWER_R_LEG_KP);
	m_lrl_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, &m_gains.at(4)->m_kp, LOWER_R_LEG_KD);

	m_lrl_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_lrl_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Left Foot");
	m_lf_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, &m_gains.at(5)->m_kp, L_FOOT_KP);
	m_lf_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, &m_gains.at(5)->m_kp, L_FOOT_KD);

	m_lf_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_lf_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_separator_to_panel(gains_panel);

	m_glui_window->add_statictext_to_panel(gains_panel, "Right foot");
	m_rf_kp_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kp", GLUI_SPINNER_FLOAT, &m_gains.at(6)->m_kp, R_FOOT_KP);
	m_rf_kd_spinner = m_glui_window->add_spinner_to_panel(gains_panel, "kd", GLUI_SPINNER_FLOAT, &m_gains.at(6)->m_kp, R_FOOT_KD);

	m_rf_kp_spinner->set_float_limits(KP_LOWER, KP_HIGHER);
	m_rf_kd_spinner->set_float_limits(KD_LOWER, KD_HIGHER);

	m_glui_window->add_button_to_panel(gains_panel, "Save Gains", SAVEGAINS_BUTTON, (GLUI_Update_CB)SaveGainsButtonPressed);

	// Horizontal separation
	m_glui_window->add_column(true);

	/*===================================== STATES =========================================*/
	GLUI_Panel *states_panel = m_glui_window->add_panel("States");
	m_StatesRadioGroup = m_glui_window->add_radiogroup_to_panel(states_panel, &m_currentState, -1, (GLUI_Update_CB)StateChanged);

	m_glui_window->add_radiobutton_to_group(m_StatesRadioGroup, "State 0");
	m_glui_window->add_radiobutton_to_group(m_StatesRadioGroup, "State 1");
	m_glui_window->add_radiobutton_to_group(m_StatesRadioGroup, "State 2");
	m_glui_window->add_radiobutton_to_group(m_StatesRadioGroup, "State 3");
	m_glui_window->add_radiobutton_to_group(m_StatesRadioGroup, "State 4");

	m_glui_window->add_separator_to_panel(states_panel);

	m_glui_window->add_statictext_to_panel(states_panel, "Desired State Angles");

	m_torso_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Torso Angle", GLUI_SPINNER_FLOAT, NULL, TORSO_ANGLE, (GLUI_Update_CB)TorsoAngleChanged);
	m_ull_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Upper left leg Angle", GLUI_SPINNER_FLOAT, NULL, ULL_ANGLE, (GLUI_Update_CB)UpperLeftLegAngleChanged);
	m_url_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Upper right leg Angle", GLUI_SPINNER_FLOAT, NULL, URL_ANGLE, (GLUI_Update_CB)UpperRightLegAngleChanged);
	m_lll_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Lower left leg Angle", GLUI_SPINNER_FLOAT, NULL, LLL_ANGLE, (GLUI_Update_CB)LowerLeftLegAngleChanged);
	m_lrl_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Lower right leg Angle", GLUI_SPINNER_FLOAT, NULL, LRL_ANGLE, (GLUI_Update_CB)LowerRightLegAngleChanged);
	m_lf_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Left foot Angle", GLUI_SPINNER_FLOAT, NULL, LF_ANGLE, (GLUI_Update_CB)LeftFootAngleChanged);
	m_rf_state_spinner = m_glui_window->add_spinner_to_panel(states_panel, "Right foot Angle", GLUI_SPINNER_FLOAT, NULL, RF_ANGLE, (GLUI_Update_CB)RightFootAngleChanged);

	m_torso_state_spinner->set_float_limits(SPINNER_TORSO_LOW, SPINNER_TORSO_HIGH);
	m_ull_state_spinner->set_float_limits(SPINNER_UPPER_LEG_LOW, SPINNER_UPPER_LEG_HIGH);
	m_url_state_spinner->set_float_limits(SPINNER_UPPER_LEG_LOW, SPINNER_UPPER_LEG_HIGH);
	m_lll_state_spinner->set_float_limits(SPINNER_LOWER_LEG_LOW, SPINNER_LOWER_LEG_HIGH);
	m_lrl_state_spinner->set_float_limits(SPINNER_LOWER_LEG_LOW, SPINNER_LOWER_LEG_HIGH);
	m_lf_state_spinner->set_float_limits(SPINNER_FOOT_LOW, SPINNER_FOOT_HIGH);
	m_rf_state_spinner->set_float_limits(SPINNER_FOOT_LOW, SPINNER_FOOT_HIGH);

	m_glui_window->add_button_to_panel(states_panel, "Save States", SAVESTATES_BUTTON, (GLUI_Update_CB)SaveStatesButtonPressed);

	/*===================================== CONTROLS =========================================*/

	GLUI_Panel *control_panel = m_glui_window->add_panel("Controls");
	m_glui_window->add_button_to_panel(control_panel, "Reset", RESET_BUTTON, (GLUI_Update_CB)ResetButtonPressed);
	m_glui_window->add_separator_to_panel(control_panel);
	m_glui_window->add_button_to_panel(control_panel, "Pause", PAUSE_BUTTON, (GLUI_Update_CB)PauseButtonPressed);
	m_glui_window->add_separator_to_panel(control_panel);
	m_glui_window->add_button_to_panel(control_panel, "Start", START_BUTTON, (GLUI_Update_CB)StartButtonPressed);

}

void RagDollApplication::SetupGUIConfiguration(std::vector<State *>states, std::vector<Gains *> gains) {

	// Assume Currently Selected State is 0
	DisplayState(0);
	DisplayGains();

	DisableStateSpinner();

}

void RagDollApplication::ShutdownPhysics() {

}

void RagDollApplication::DisplayState(int state) {

	State *selected_state = m_states.at(state);
	m_torso_state_spinner->set_float_val(selected_state->m_torsoAngle);
	
	m_ull_state_spinner->set_float_val(selected_state->m_upperLeftLegAngle);
	m_url_state_spinner->set_float_val(selected_state->m_upperRightLegAngle);
	
	// Lower legs have to relative orientation to upper legs. Local coordinates
	m_lll_state_spinner->set_float_val(selected_state->m_lowerLeftLegAngle);
	m_lrl_state_spinner->set_float_val(selected_state->m_lowerRightLegAngle);
	
	m_lf_state_spinner->set_float_val(selected_state->m_leftFootAngle);
	m_rf_state_spinner->set_float_val(selected_state->m_rightFootAngle);

}

void RagDollApplication::DisplayGains() {

	for (std::vector<Gains *>::iterator it = m_gains.begin(); it != m_gains.end(); it++) {

		switch ((*it)->GetAssociatedBody()) {
		case TORSO: {
			m_torso_kp_spinner->set_float_val((*it)->m_kp);
			m_torso_kd_spinner->set_float_val((*it)->m_kd);
		}
			break;
		case UPPER_LEFT_LEG: {
			m_ull_kp_spinner->set_float_val((*it)->m_kp);
			m_ull_kd_spinner->set_float_val((*it)->m_kp);
		}
			break;
		case UPPER_RIGHT_LEG: {
			m_url_kp_spinner->set_float_val((*it)->m_kp);
			m_url_kd_spinner->set_float_val((*it)->m_kd);
		}
			break;
		case LOWER_LEFT_LEG: {
			m_lll_kp_spinner->set_float_val((*it)->m_kp);
			m_lll_kd_spinner->set_float_val((*it)->m_kd);

		}
			break;
		case LOWER_RIGHT_LEG: {
			m_lrl_kp_spinner->set_float_val((*it)->m_kp);
			m_lrl_kd_spinner->set_float_val((*it)->m_kd);
		}
			break;
		case LEFT_FOOT: {
			m_lf_kp_spinner->set_float_val((*it)->m_kp);
			m_lf_kd_spinner->set_float_val((*it)->m_kd);
		}
			break;
		case RIGHT_FOOT: {
			m_rf_kp_spinner->set_float_val((*it)->m_kp);
			m_rf_kd_spinner->set_float_val((*it)->m_kd);
		}
			break;
		default:
			break;
		}
	}
}

#pragma endregion INITIALIZATION

#pragma region RAG_DOLL

void RagDollApplication::CreateRagDoll(const btVector3 &position) {

	// Create a torso centered at the position
	btVector3 halfSize(torso_height/2, torso_width/2, 0.0);
	
	m_torso = Create2DBox(halfSize, torso_mass, btVector3(0, 51 / 256.0, 102 / 256.0), position); // Blue

	// test box
	//Create2DBox(halfSize, mass, GetRandomColor(), btVector3(-4, 5, 0.7));
	//Create2DBox(halfSize, mass, GetRandomColor(), btVector3(4, 7, 0.5));

	// Create Upper legs
	halfSize = btVector3(upper_leg_height / 2, upper_leg_width / 2, 0.0f);
	m_upperLeftLeg = Create2DBox(halfSize, upper_leg_mass, btVector3(255 / 256.0, 102 / 256.0, 255 / 256.0), position + btVector3(0,0, -0.1)); // Pink
	m_upperRightLeg = Create2DBox(halfSize, upper_leg_mass, btVector3(0 / 256.0, 153 / 256.0, 0 / 256.0), position + btVector3(0,0,0.1)); // Green

	// Create lower legs
	halfSize = btVector3(lower_leg_height / 2, lower_leg_width / 2, 0.0f);
	m_lowerLeftLeg = Create2DBox(halfSize, lower_leg_mass, btVector3(250 / 256.0, 250 / 256.0, 10 / 256.0), position + btVector3(0, 0, -0.11)); // Yellow
	m_lowerRightLeg = Create2DBox(halfSize, lower_leg_mass, btVector3(255 / 256.0, 102 / 256.0, 0 / 256.0), position + btVector3(0, 0, 0.11)); // Orange

	// Create feet
	halfSize = btVector3(foot_width / 2, foot_height / 2, 0.0f); 
	m_leftFoot = Create2DBox(halfSize, feet_mass, btVector3(0 / 256.0, 255 / 256.0, 255 / 256.0), position + btVector3(0,0,-0.12)); // aqua blue
	halfSize = btVector3(foot_width / 2, foot_height / 2, 0.0f);
	m_rightFoot = Create2DBox(halfSize, feet_mass, btVector3(153 / 256.0, 0 / 256.0, 153 / 256.0), position + btVector3(0, 0, 0.12)); // purple

	AddHinges();

}

void RagDollApplication::AddHinges() {

	printf("Adding hinges. \n");

	// Connect torso to upper legs
	m_torso_ulLeg = AddHingeConstraint(m_torso, m_upperLeftLeg, btVector3(-torso_height / 2, 0, 0), btVector3(upper_leg_height / 2, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_TORSO_ULL_LOW), Constants::GetInstance().DegreesToRadians(HINGE_TORSO_ULL_HIGH));
	m_torso_urLeg = AddHingeConstraint(m_torso, m_upperRightLeg, btVector3(-torso_height / 2, 0, 0), btVector3(upper_leg_height / 2, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_TORSO_ULL_LOW), Constants::GetInstance().DegreesToRadians(HINGE_TORSO_URL_HIGH));

	// Connect upper legs to lower legs
	m_ulLeg_llLeg = AddHingeConstraint(m_upperLeftLeg, m_lowerLeftLeg, btVector3(-upper_leg_height / 2, 0, 0), btVector3(lower_leg_height / 2, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_ULL_LLL_LOW), Constants::GetInstance().DegreesToRadians(HINGE_ULL_LLL_HIGH));
	m_urLeg_lrLeg = AddHingeConstraint(m_upperRightLeg, m_lowerRightLeg, btVector3(-upper_leg_height / 2, 0, 0), btVector3(lower_leg_height / 2, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_URL_LRL_LOW), Constants::GetInstance().DegreesToRadians(HINGE_URL_LRL_HIGH));

	// Connect feet to lower legs
	m_llLeg_lFoot = AddHingeConstraint(m_lowerLeftLeg, m_leftFoot, btVector3(-lower_leg_height / 2, 0, 0), btVector3((foot_width) / 4, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_LLL_LF_LOW), Constants::GetInstance().DegreesToRadians(HINGE_LLL_LF_HIGH));
	m_lrLeg_rFoot = AddHingeConstraint(m_lowerRightLeg, m_rightFoot, btVector3(-lower_leg_height / 2, 0, 0), btVector3((foot_width) / 4, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), Constants::GetInstance().DegreesToRadians(HINGE_LRL_RF_LOW), Constants::GetInstance().DegreesToRadians(HINGE_LRL_RF_HIGH));
}

void RagDollApplication::SaveStates() {
	ChangeState(-1);
	// Save States into file
	m_WalkingController->SaveStates();
}

void RagDollApplication::SaveGains(){
	// Save gains into file
	m_WalkingController->SaveGains();
}

void RagDollApplication::Reset() {

	printf("Reset button pressed \n");

	m_WalkingController->Reset();

	std::vector<GameObject *> bodies{ m_torso, m_upperRightLeg, m_upperLeftLeg, m_lowerRightLeg, m_lowerLeftLeg, m_rightFoot, m_leftFoot };

	// Clear Everything
	//GameObject::ClearForces(bodies);
	GameObject::ClearVelocities(bodies);

	DisableStateSpinner();
	EnableGainSpinners();
	m_StatesRadioGroup->enable();

	GameObject::DisableObjects(bodies);
	
	m_StatesRadioGroup->set_int_val(0); // State 0
	m_currentState = 0;
	UpdateRagDoll();

}

void RagDollApplication::Start() {
	
	DisableAllSpinners();

	printf("Start button Pressed\n INITIATE WALKING!!!\n");

	std::vector<GameObject *> bodies{ m_torso, m_upperRightLeg, m_upperLeftLeg, m_lowerRightLeg, m_lowerLeftLeg, m_rightFoot, m_leftFoot };
	GameObject::EnableObjects(bodies);

	m_WalkingController->Walk();
}

void RagDollApplication::Pause() {

	printf("Pause button pressed \n");

	std::vector<GameObject *> bodies{ m_torso, m_upperRightLeg, m_upperLeftLeg, m_lowerRightLeg, m_lowerLeftLeg, m_rightFoot, m_leftFoot };
	GameObject::DisableObjects(bodies);

	m_WalkingController->PauseWalking();
}

void RagDollApplication::ChangeState(int id) {

	printf("previous state = %d\n", m_previousState);
	
	// Update previous state
	m_states.at(m_previousState)->m_torsoAngle = m_torso_state_spinner->get_float_val();
	m_states.at(m_previousState)->m_upperLeftLegAngle = m_ull_state_spinner->get_float_val();
	m_states.at(m_previousState)->m_upperRightLegAngle = m_url_state_spinner->get_float_val();
	// Relative Store local coordinates
	m_states.at(m_previousState)->m_lowerLeftLegAngle = m_lll_state_spinner->get_float_val();
	m_states.at(m_previousState)->m_lowerRightLegAngle = m_lrl_state_spinner->get_float_val();
	m_states.at(m_previousState)->m_leftFootAngle = m_lf_state_spinner->get_float_val();
	m_states.at(m_previousState)->m_rightFootAngle = m_rf_state_spinner->get_float_val();
	// Change previous to current state
	DisplayState(m_currentState);
	m_previousState = m_currentState;
	printf("next state = %d \n", m_currentState);

	DisableStateSpinner();

	UpdateRagDoll();

}

void RagDollApplication::UpdateRagDoll() {

	State *state = m_states.at(m_currentState);

	float torsoAngle = Constants::GetInstance().DegreesToRadians(state->m_torsoAngle);
	float ullAngle = Constants::GetInstance().DegreesToRadians(state->m_upperLeftLegAngle);
	float urlAngle = Constants::GetInstance().DegreesToRadians(state->m_upperRightLegAngle);
	// Local coordinates convert to world coordinates
	float lllAngle = Constants::GetInstance().DegreesToRadians(state->m_upperLeftLegAngle - state->m_lowerLeftLegAngle);
	float lrlAngle = Constants::GetInstance().DegreesToRadians(state->m_upperRightLegAngle - state->m_lowerRightLegAngle);

	//printf("lll angle = %f, lfAngle = %f\n", Constants::GetInstance().RadiansToDegrees(lllAngle), Constants::GetInstance().RadiansToDegrees(lllAngle) - state->m_leftFootAngle - 90);

	float lfAngle = lllAngle - Constants::GetInstance().DegreesToRadians(state->m_leftFootAngle - 90);
	float rfAngle = lrlAngle - Constants::GetInstance().DegreesToRadians(state->m_rightFootAngle - 90);

	//printf("Updating Rag doll:\n %f, %f, %f, %f, %f, %f, %f\n", torsoAngle, ullAngle, urlAngle, lllAngle, lrlAngle, lfAngle, rfAngle);
	// Blue
	m_torso->Reposition(ORIGINAL_TORSO_POSITION + btVector3(-(torso_height/2) * cos(PI - torsoAngle), sin(PI - torsoAngle)*(torso_height/2) - (torso_height/2),0), btQuaternion(btVector3(0, 0, 1), torsoAngle));
	// Pink Green
	m_upperLeftLeg->Reposition(ORIGINAL_TORSO_POSITION + btVector3(0.0f, -(torso_height / 2 + upper_leg_height / 2), 0.1) + btVector3(-cos(ullAngle) * (upper_leg_height/2), (upper_leg_height/2) - sin(ullAngle)*upper_leg_height/2, 0), btQuaternion(btVector3(0,0,1), ullAngle));
	m_upperRightLeg->Reposition(ORIGINAL_TORSO_POSITION + btVector3(0.0f, -(torso_height / 2 + upper_leg_height / 2),- 0.1) + btVector3(-cos(urlAngle) * (upper_leg_height / 2), (upper_leg_height / 2) - sin(urlAngle)*upper_leg_height / 2, 0), btQuaternion(btVector3(0, 0, 1), urlAngle));
	btVector3 upperLeftLegBottomPoint = m_upperLeftLeg->GetCOMPosition() + btVector3(cos(PI - ullAngle) * upper_leg_height/2, -sin(PI - ullAngle) * upper_leg_height/2, 0);
	btVector3 upperRightLegBottomPoint = m_upperRightLeg->GetCOMPosition() + btVector3(cos(PI - urlAngle) * upper_leg_height / 2, -sin(PI - urlAngle) * upper_leg_height / 2, 0);
	//printf("ULL BP position: %f, %f\n", upperLeftLegBottomPoint.x(), upperLeftLegBottomPoint.y());
	// Yellow Orange
	//m_lowerLeftLeg->Reposition(upperLeftLegBottomPoint + btVector3(0.0f, -lower_leg_height / 2, 0.1), btQuaternion(btVector3(0, 0, 1), lllAngle));
	m_lowerLeftLeg->Reposition(upperLeftLegBottomPoint + btVector3(-(cos(lllAngle)*lower_leg_height / 2), -(sin(lllAngle)*lower_leg_height / 2), 0.1), btQuaternion(btVector3(0, 0, 1), lllAngle));
	m_lowerRightLeg->Reposition(upperRightLegBottomPoint + btVector3(-(cos(lrlAngle) * lower_leg_height / 2), -(sin(lrlAngle)*lower_leg_height / 2), -0.1), btQuaternion(btVector3(0, 0, 1), lrlAngle));

	btVector3 lowerLeftLegBottomPoint = m_lowerLeftLeg->GetCOMPosition() + btVector3(cos(PI - lllAngle) * lower_leg_height / 2, -sin(PI - lllAngle) * lower_leg_height / 2, 0);
	btVector3 lowerRightLegBottomPoint = m_lowerRightLeg->GetCOMPosition() + btVector3(cos(PI - lrlAngle) * lower_leg_height / 2, -sin(PI - lrlAngle) * lower_leg_height / 2, 0);

	m_leftFoot->Reposition(lowerLeftLegBottomPoint + btVector3(-cos(lfAngle) * foot_width/4, -sin(lfAngle) * foot_width/4, 0.1), btQuaternion(btVector3(0, 0, 1), lfAngle));
	m_rightFoot->Reposition(lowerRightLegBottomPoint + btVector3(-cos(rfAngle) * foot_width/4, -sin(rfAngle) * foot_width/4, -0.1), btQuaternion(btVector3(0, 0, 1), rfAngle));

}

void RagDollApplication::ChangeTorsoAngle() {

	State *state = m_states.at(m_currentState);
	state->m_torsoAngle = m_torso_state_spinner->get_float_val();
	UpdateRagDoll();

}

void RagDollApplication::ChangeUpperLeftLegAngle() {
	State *state = m_states.at(m_currentState);
	state->m_upperLeftLegAngle = m_ull_state_spinner->get_float_val();
	UpdateRagDoll();
}

void RagDollApplication::ChangeUpperRightLegAngle() {
	State *state = m_states.at(m_currentState);
	state->m_upperRightLegAngle = m_url_state_spinner->get_float_val();
	UpdateRagDoll();
}

void RagDollApplication::ChangeLowerLeftLegAngle() {
	State *state = m_states.at(m_currentState);
	state->m_lowerLeftLegAngle = m_lll_state_spinner->get_float_val(); // Assume relative orientation
	UpdateRagDoll();
}

void RagDollApplication::ChangeLowerRightLegAngle() {
	State *state = m_states.at(m_currentState);
	state->m_lowerRightLegAngle = m_lrl_state_spinner->get_float_val(); // Assume relative orientation
	UpdateRagDoll();
}

void RagDollApplication::ChangeLeftFootAngle() {
	
	State *state = m_states.at(m_currentState);
	state->m_leftFootAngle = m_lf_state_spinner->get_float_val(); // Assume relative orientation
	UpdateRagDoll();

}

void RagDollApplication::ChangeRightFootAngle() {

	State *state = m_states.at(m_currentState);
	state->m_rightFootAngle = m_rf_state_spinner->get_float_val(); // Assume relative orientation
	UpdateRagDoll();


}

void RagDollApplication::DisableStateSpinner() {

	switch (m_currentState)
	{
		case 0: {
			// Deactivate
			m_url_state_spinner->disable();
			m_lrl_state_spinner->disable();
			m_rf_state_spinner->disable();
			m_ull_state_spinner->disable();
			m_lll_state_spinner->disable();
			m_lf_state_spinner->disable();
			m_torso_state_spinner->disable();
		}
		break;
		case 1: 
		case 2: {
			// Deactivate
			m_url_state_spinner->disable();
			m_lrl_state_spinner->disable();
			m_rf_state_spinner->disable();

			// Activate
			m_ull_state_spinner->enable();
			m_lll_state_spinner->enable();
			m_lf_state_spinner->enable();
			m_torso_state_spinner->enable();
			break;
		}
		case 3:
		case 4:{
			  // Deactivate
			  m_ull_state_spinner->disable();
			  m_lll_state_spinner->disable();
			  m_lf_state_spinner->disable();

			  // Activate
			  m_url_state_spinner->enable();
			  m_lrl_state_spinner->enable();
			  m_rf_state_spinner->enable();
			  m_torso_state_spinner->enable();
		}
		break;
	default:
		break;
	}

}

void RagDollApplication::DisableAllSpinners() {

	std::vector<GLUI_Spinner*>spinners = {
		m_torso_kp_spinner, m_torso_kd_spinner,
		m_ull_kp_spinner, m_ull_kd_spinner,
		m_url_kp_spinner, m_url_kd_spinner,
		m_lll_kp_spinner, m_lll_kd_spinner,
		m_lrl_kp_spinner, m_lrl_kd_spinner,
		m_lf_kp_spinner, m_lf_kd_spinner,
		m_rf_kp_spinner, m_rf_kd_spinner,
		m_torso_state_spinner, m_ull_state_spinner,
		m_url_state_spinner, m_lll_state_spinner,
		m_lrl_state_spinner, m_lf_state_spinner,
		m_rf_state_spinner
	};

	for (std::vector<GLUI_Spinner*>::iterator it = spinners.begin(); it != spinners.end(); it++) {
		(*it)->disable();
	}

	m_StatesRadioGroup->disable();

}

void RagDollApplication::EnableGainSpinners() {

	std::vector<GLUI_Spinner*>spinners = {
		m_torso_kp_spinner, m_torso_kd_spinner,
		m_ull_kp_spinner, m_ull_kd_spinner,
		m_url_kp_spinner, m_url_kd_spinner,
		m_lll_kp_spinner, m_lll_kd_spinner,
		m_lrl_kp_spinner, m_lrl_kd_spinner,
		m_lf_kp_spinner, m_lf_kd_spinner,
		m_rf_kp_spinner, m_rf_kd_spinner
	};

	for (std::vector<GLUI_Spinner*>::iterator it = spinners.begin(); it != spinners.end(); it++) {
		(*it)->enable();
	}

}

void RagDollApplication::ApplyTorqueOnTorso(float torqueForce) {
	ApplyTorqueOnGameBody(m_torso, torqueForce);
}

void RagDollApplication::ApplyTorqueOnUpperRightLeg(float torqueForce) {
	ApplyTorqueOnGameBody(m_upperRightLeg, torqueForce);
}

void RagDollApplication::ApplyTorqueOnUpperLeftLeg(float torqueForce) {
	ApplyTorqueOnGameBody(m_upperLeftLeg, torqueForce);
}

void RagDollApplication::ApplyTorqueOnLowerRightLeg(float torqueForce) {
	ApplyTorqueOnGameBody(m_lowerRightLeg, torqueForce);
}

void RagDollApplication::ApplyTorqueOnLowerLeftLeg(float torqueForce) {
	ApplyTorqueOnGameBody(m_lowerLeftLeg, torqueForce);
}

void RagDollApplication::ApplyTorqueOnRightFoot(float torqueForce) {
	ApplyTorqueOnGameBody(m_rightFoot, torqueForce);
}

void RagDollApplication::ApplyTorqueOnLeftFoot(float torqueForce) {
	ApplyTorqueOnGameBody(m_leftFoot, torqueForce);
}

void RagDollApplication::ApplyTorqueOnGameBody(GameObject *body, float torqueForce) {
	// make btVector3
	// apply torque to body
}

#pragma endregion RAG_DOLL

#pragma region DRAWING

GameObject *RagDollApplication::Create2DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position) {

	GameObject *aBox = CreateGameObject(new btBox2dShape(halfSize), mass, color, position);
	return aBox;
}

GameObject *RagDollApplication::Create3DBox(const btVector3 &halfSize, float mass, const btVector3 &color, const btVector3 &position) {
	GameObject *aBox = CreateGameObject(new btBoxShape(halfSize), mass, color, position);
	return aBox;
}


btVector3 RagDollApplication::GetRandomColor() {
	return btVector3(((double)rand() / RAND_MAX), ((double)rand() / RAND_MAX), ((double)rand() / RAND_MAX));
}

#pragma endregion DRAWING

#pragma region GLUI_CALLBACKS

static void RagDollIdle() {
	m_app->Idle();
}

static void SaveGainsButtonPressed(int id) {
	m_app->SaveGains();
}

static void SaveStatesButtonPressed(int id) {
	m_app->SaveStates();
}

static void ResetButtonPressed(int id) {
	m_app->Reset();
}

static void StartButtonPressed(int id) {
	m_app->Start();
}

static void PauseButtonPressed(int id) {
	m_app->Pause();
}

static void StateChanged(int id)	 {
	m_app->ChangeState(id);
}

// State Callbacks

static void TorsoAngleChanged(int id) {
	m_app->ChangeTorsoAngle();
}

static void UpperLeftLegAngleChanged(int id) {
	m_app->ChangeUpperLeftLegAngle();
}

static void UpperRightLegAngleChanged(int id) {
	m_app->ChangeUpperRightLegAngle();
}

static void LowerLeftLegAngleChanged(int id) {
	m_app->ChangeLowerLeftLegAngle();
}

static void LowerRightLegAngleChanged(int id) {
	m_app->ChangeLowerRightLegAngle();
}

static void LeftFootAngleChanged(int id) {
	m_app->ChangeLeftFootAngle();
}

static void RightFootAngleChanged(int id) {
	m_app->ChangeRightFootAngle();
}

#pragma endregion GLUI_CALLBACKS