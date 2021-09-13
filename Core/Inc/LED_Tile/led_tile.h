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
#define B_B 0.4f
#define IR_A 0.0f
#define IR_B 0.1125f

#define TWINKLE_NUM_MAX 	5
#define TWINKLE_CHANCE 		10000

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

	struct {
		RGB_LED twinkles[TWINKLE_NUM_MAX];
		uint8_t en;
		uint16_t chance;	// x / 1000 chance to spawn a twinkle
		float time_step;	// controls the decay rate
		uint8_t num;
	} twinkle;
} LED_Tile;

LED_Tile Init_LED_Tile();
void LED_Tile_Set_LED_Intensity(LED_Tile *tile, uint16_t dev, uint8_t LED, float intensity);
void LED_Tile_Set_LED_Color(LED_Tile *tile, uint16_t dev, uint8_t LED, uint8_t red, uint8_t green, uint8_t blue);
void LED_Tile_Set_IR_LED(LED_Tile *tile, uint16_t dev, uint8_t value);
void LED_Tile_Twinkle_Init(LED_Tile *tile, uint16_t chance, uint8_t num);
void LED_Tile_Twinkle_Start(LED_Tile *tile, float time_step);
void LED_Tile_Twinkle_Stop(LED_Tile *tile);
void LED_Tile_Twinkle_Update(LED_Tile *tile);
void LED_Tile_Twinkle_Add(LED_Tile *tile);
float f_brightness(float a, float b, float t);
float Get_Intensity(float intensity, float a, float b);
float f_x(float x, float a, float b, float c);
float f_dx(float x, float a, float b);

#endif /* INC_LED_TILE_LED_TILE_H_ */
