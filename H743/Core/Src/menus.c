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
	{"Set RPM", setRPM, NULL}
};
Menu StepMenu = {
		StepMenuItems,
		sizeof(StepMenuItems) / sizeof(StepMenuItems[0]),
		Action,
		&ControlMenu,
		&StepHelp
};


MenuItem SpeedMenuItems[] = {
	{"RPM", NULL, NULL}
};
Menu SpeedMenu = {
		SpeedMenuItems,
		sizeof(SpeedMenuItems) / sizeof(SpeedMenuItems[0]),
		Action,
		&ControlMenu,
		&SpeedHelp
};


MenuItem ManualMenuItems[] = {
	{"Speed", NULL, &SpeedMenu},
	{"Step", NULL, &StepMenu}
};
Menu ManualMenu = {
	ManualMenuItems,
	sizeof(ManualMenuItems) / sizeof(ManualMenuItems[0]),
	Node,
	&ControlMenu,
	&ManualHelp
};


MenuItem AutomaticMenuItems[] = {
	{"Speed", NULL, NULL}
};
Menu AutomaticMenuMenu = {
	AutomaticMenuItems,
	sizeof(AutomaticMenuItems) / sizeof(AutomaticMenuItems[0]),
	Unused,
	&ControlMenu,
	&AutomaticHelp
};


MenuItem ControlMenuItems[] = {
	{"Manual", NULL, &ManualMenu},
	{"Automatic", NULL, &StepMenu}
};
Menu ControlMenu = {
	ControlMenuItems,
	sizeof(ControlMenuItems) / sizeof(ControlMenuItems[0]),
	Node,
	&MainMenu,
	&ControlHelp
};


MenuItem MainMenuItems[] = {
	{"Control", NULL, &ControlMenu},
	{"Debug", NULL, NULL},
	{"Other", NULL, NULL}
};
Menu MainMenu = {
	MainMenuItems,
	sizeof(MainMenuItems) / sizeof(MainMenuItems[0]),
	Node,
	NULL,
	&MainHelp
};

MainHelpItems[] = {

};
Menu MainHelp = {
	MainMenuItems,
	sizeof(MainMenuItems) / sizeof(MainMenuItems[0]),
	Help,
	&MainMenu,
	NULL
};

MenuState menuState = {
	&MainMenu,
	0
};


void setRPM(void) {
	int32_t rpm = __HAL_TIM_GET_COUNTER(&htim5);
	for (int i = 0; i < 4; i++) {
		if (servo[i].enableFlag) {
			setMotorSpeed(rpm*ENCRPMGAIN, servo[0]);
		}
	}
}
