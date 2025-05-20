/*
 * menus.h
 *
 *  Created on: Apr 9, 2025
 *      Author: jakob
 */

#ifndef INC_MENUS_H_
#define INC_MENUS_H_

#include <customMain.h>

typedef enum {
	Node = 0U,
	Idle,
	Action,
	Motion,
	Help,
	Unused
} MenuType;

typedef struct Menu Menu;

typedef struct {
    const char *label;
    void (*action)(void);  // Action when selected (optional)
    Menu *submenu;         // Submenu when selected (optional)
} MenuItem;

struct Menu {
    MenuItem *items;
    uint8_t num_items;
    MenuType type;
    Menu *parent;          // Pointer to parent menu for navigation
    Menu *help_menu;
};

typedef struct {
    Menu *current_menu;
    uint8_t selected_index;
} MenuState;

extern MenuState menuState;

void startGame(void);
void startValidation(void);
void startCalibration(void);
void startSeq1(void);
void startSeq2(void);
void startSeq3(void);
void setStepFlag(void);
void setSeqFlag(void);
void setRPM(void);
void enterMenu(void);

#endif /* INC_MENUS_H_ */
