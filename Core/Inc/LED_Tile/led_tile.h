/*
 * led_tile.h
 *
 *  Created on: Jun 25, 2021
 *      Author: THollis
 */

#ifndef INC_LED_TILE_LED_TILE_H_
#define INC_LED_TILE_LED_TILE_H_

#include "main.h"
#include "PCA9745/pca9745.h"

extern SPI_HandleTypeDef hspi1;

#define TILE_SPI 		hspi1
#define TILE_CS_PORT 	nCS_GPIO_Port
#define TILE_CS_PIN		nCS_Pin
#define TILE_OE_PORT	nOE_GPIO_Port
#define TILE_OE_PIN		nOE_Pin

#define NUM_TILES 1
#define R_EXT 3600.0f
#define MAX_INTESITY 2.25f

#define R_A 0.0f
#define R_B 0.125f
#define G_A -0.00541666f
#define G_B 0.220833f
#define B_A -0.00625f
#define B_B 0.3125f

#define TWINKLE_NUM 	20
#define TWINKLE_CHANCE	1	//10 out of 1000 chance to spawn a new twinkle
#define TWINKLE_TIME_STEP 0.01f

typedef struct {
	uint8_t brightness;
	uint8_t instruction;
} UV_LED;

typedef struct {
	uint8_t r, g, b;
	float a_0, a_1;
	float scale;
	float t;
	float t_max;
	uint8_t active;
	uint16_t dev;
	uint8_t led;
} RGB_LED;

typedef struct {
	PCA9745 *p;
	RGB_LED twinkle[TWINKLE_NUM];
	uint8_t twinkle_en;
} LED_Tile;

LED_Tile Init_LED_Tile();
void LED_Tile_Set_LED_Intensity(LED_Tile *tile, uint16_t dev, uint8_t LED, float intensity);
void LED_Tile_Set_LED_Color(LED_Tile *tile, uint16_t dev, uint8_t LED, uint8_t red, uint8_t green, uint8_t blue);
void LED_Tile_Twinkle_Start(LED_Tile *tile);
void LED_Tile_Twinkle_Stop(LED_Tile *tile);
void LED_Tile_Twinkle_Update(LED_Tile *tile);
void LED_Tile_Twinkle_Add(LED_Tile *tile);
float f_brightness(float a, float b, float t);
float Get_Intensity(float intensity, float a, float b);
float f_x(float x, float a, float b, float c);
float f_dx(float x, float a, float b);

#endif /* INC_LED_TILE_LED_TILE_H_ */
