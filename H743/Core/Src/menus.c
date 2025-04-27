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


MenuItem ManualMenuItems[] = {
	{"Speed", NULL, &SpeedMenu},
	{"Step", NULL, &StepMenu}
};
Menu ManualMenu = {
	ManualMenuItems,
	sizeof(ManualMenuItems) / sizeof(ManualMenuItems[0]),
	Node,
	&MainMenu,
	&ManualHelp
};


MenuItem AutomaticMenuItems[] = {
	{"Speed", NULL, NULL}
};
Menu AutomaticMenu = {
	AutomaticMenuItems,
	sizeof(AutomaticMenuItems) / sizeof(AutomaticMenuItems[0]),
	Unused,
	&MainMenu,
	&AutomaticHelp
};


MenuItem MainMenuItems[] = {
	{"Manual", NULL, &ManualMenu},
	{"Automatic", NULL, &AutomaticMenu},
	{"Other", NULL, NULL}
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


void setRPM(void) {
	int32_t rpm = __HAL_TIM_GET_COUNTER(&htim5);
	for (int i = 0; i < 4; i++) {
		if (servo[i].enableFlag) {
			setMotorSpeed(rpm*ENCRPMGAIN, servo[i]);
		}
	}
}
