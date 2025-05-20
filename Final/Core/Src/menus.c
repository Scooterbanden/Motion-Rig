/*
 * menus.c
 *
 *  Created on: Apr 9, 2025
 *      Author: jakob
 */
#include "menus.h"

// Menus
Menu MainMenu;
	Menu ControlMenu;
		Menu ManualMenu;
			Menu SpeedMenu;
			Menu StepMenu;
		Menu AutomaticMenu;
			Menu SequencesMenu;
	Menu DebugMenu;
	Menu OtherMenu;

// Help Menus
	Menu MainHelp;
		Menu ControlHelp;
			Menu ManualHelp;
				Menu SpeedHelp;
				Menu StepHelp;
			Menu AutomaticHelp;
		Menu DebugHelp;
		Menu OtherHelp;

MenuItem StepMenuItems[] = {
	{"Set RPM", setStepFlag, NULL}
};
Menu StepMenu = {
		StepMenuItems,
		sizeof(StepMenuItems) / sizeof(StepMenuItems[0]),
		Action,
		&ManualMenu,
		&StepHelp
};


MenuItem SpeedMenuItems[] = {
	{"RPM", NULL, NULL}
};
Menu SpeedMenu = {
	SpeedMenuItems,
	sizeof(SpeedMenuItems) / sizeof(SpeedMenuItems[0]),
	Action,
	&ManualMenu,
	&SpeedHelp
};

MenuItem SequencesMenuItems[] = {
		{"Chirp1", startSeq1, NULL},
		{"Chirp2", startSeq2, NULL},
		{"Chirp3", startSeq3, NULL}
};

Menu SequencesMenu = {
	SequencesMenuItems,
	sizeof(SequencesMenuItems) / sizeof(SequencesMenuItems[0]),
	Motion,
	&AutomaticMenu,
	NULL
};

MenuItem ManualMenuItems[] = {
	{"Speed", enterMenu, &SpeedMenu},
	{"Step", enterMenu, &StepMenu},
	{"Sequences", enterMenu, &SequencesMenu}
};

Menu ManualMenu = {
	ManualMenuItems,
	sizeof(ManualMenuItems) / sizeof(ManualMenuItems[0]),
	Idle,
	&MainMenu,
	&ManualHelp
};


MenuItem AutomaticMenuItems[] = {
	{"Validation", startValidation, NULL},
	{"Game", startGame, NULL},
	{"Calibrate", startCalibration, NULL}
};
Menu AutomaticMenu = {
	AutomaticMenuItems,
	sizeof(AutomaticMenuItems) / sizeof(AutomaticMenuItems[0]),
	Idle,
	&MainMenu,
	&AutomaticHelp
};


MenuItem MainMenuItems[] = {
	{"Manual", enterMenu, &ManualMenu},
	{"Automatic", enterMenu, &AutomaticMenu}
};
Menu MainMenu = {
	MainMenuItems,
	sizeof(MainMenuItems) / sizeof(MainMenuItems[0]),
	Node,
	NULL,
	&MainHelp
};
/*
MainHelpItems[] = {

};
Menu MainHelp = {
	MainMenuItems,
	sizeof(MainMenuItems) / sizeof(MainMenuItems[0]),
	Help,
	&MainMenu,
	NULL
};
*/
MenuState menuState = {
	&MainMenu,
	0
};

void startGame(void) {
	controlCounter = 0;
	controlMode = GAME;
}

void startValidation(void) {
	controlCounter = 0;
	validationMode = ALL_CONSTANT;
	controlMode = VALIDATION;
}

void startCalibration(void) {
	calibrated = false;
	controlCounter = 0;
	controlMode = CALIBRATE;
}



void setRPM(void) {
	int32_t rpm = __HAL_TIM_GET_COUNTER(&htim5);
	for (int i = 0; i < 4; i++) {
		if (servo[i].enableFlag) {
			setMotorSpeed(rpm*ENCRPMGAIN, &servo[i]);
		}
	}
}

void enterMenu(void) {
	menuState.current_menu = menuState.current_menu->items[menuState.selected_index].submenu;
	menuState.selected_index = 0;
	encoderValue = 0;
	__HAL_TIM_SET_COUNTER(&htim5, 0);

	switch (menuState.current_menu->type) {
	case Idle:
		servoInit();
		controlInit();
		break;
	case Motion:
	case Action:
		servoInit();
		controlInit();
		break;
	default:
		break;
	}

}

void setStepFlag(void) {
	controlCounter = 0;
	int32_t encoderValue = __HAL_TIM_GET_COUNTER(&htim5);
	stepRef = encoderValue*20;
	controlMode = STEP;
}

void startSeq1(void) {
	freqScale = 0.00005;
	rpmAmplitude = 3000;
	setSeqFlag();
}

void startSeq2(void) {
	freqScale = 0.00005;
	rpmAmplitude = 1000;
	setSeqFlag();
}

void startSeq3(void) {
	freqScale = 0.00002;
	rpmAmplitude = 3000;
	setSeqFlag();
}

void setSeqFlag(void) {
	controlCounter = 0;
	controlMode = SEQUENCE;
}
