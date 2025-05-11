/*
 * invaders.c
 *
 * TOP SECRET PROJECT
 * Unauthorized access will result in extermination.
 *
 */

#include "main.h"
#include "stm32h7xx_hal_tim.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "invaders.h"

extern TIM_HandleTypeDef htim2;
extern uint32_t timer;
extern const unsigned char gunship [];

//ssd1306_DrawPixel(x + i, y, color);

uint8_t frames = 0;
uint8_t row = 0;
uint8_t col = 0;
SSD1306_COLOR color = White;

// 60 Hz timer interrupt


void invadersDrawScreen(void) {
	frames += 1;
	ssd1306_DrawBitmap(0,0,gunship,8,8,Black);
	/*
	ssd1306_DrawPixel(col, row, color);

	row += 1;
	if (row >= SSD1306_HEIGHT) {row = 0; col += 1;}
	if (col >= SSD1306_WIDTH) {col = 0;}

	uint32_t now = HAL_GetTick();
	if (now - timer >= 1000) {
		float fps = (frames * 1000.0f) / (now - timer);  // Use float correctly
		timer = now;
		frames = 0;  // Reset frame count
		ssd1306_SetCursor(2, 0);
		int len = snprintf(NULL, 0, "FPS: %.2f", fps);
		char *line1 = malloc(len + 1);
		snprintf(line1, len + 1, "FPS: %.2f", fps);
		ssd1306_WriteString(line1, Font_7x10, Black);
		free(line1);


		frames = 0;

	}
	*/
	ssd1306_UpdateScreen();
	//HAL_GPIO_TogglePin(GPIOB,LD2_Pin);
}

//void ship_init(shiptype_t type, )
