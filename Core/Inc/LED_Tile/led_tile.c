/*
 * led_tile.c
 *
 *  Created on: Jun 25, 2021
 *      Author: THollis
 */

#include "main.h"
#include "led_tile.h"
#include "PCA9745/pca9745.h"
#include "math.h"

uint8_t instr_buffer[NUM_TILES];
uint8_t rx_buffer[NUM_TILES];
uint8_t data_buffer[NUM_TILES];

PCA9745 p;

LED_Tile Init_LED_Tile(){
	LED_Tile tile;
	p = Init_PCA9745(&TILE_SPI, TILE_CS_PORT, TILE_CS_PIN, TILE_OE_PORT, TILE_OE_PIN);
	_PCA9745_Configure(&p, R_EXT, NUM_TILES, instr_buffer, data_buffer, rx_buffer);
	tile.p = &p;
	return tile;
}

void LED_Tile_Set_LED_Intensity(LED_Tile *tile, uint16_t dev, uint8_t LED, float intensity){
	if(LED == 5){
		//Do nothing
	}
	else{
		float r_Io = Get_Intensity(intensity, R_A, R_B);
		float g_Io = Get_Intensity(intensity, G_A, G_B);
		float b_Io = Get_Intensity(intensity, B_A, B_B);
		PCA9745_Set_IREFx(tile->p, dev, 15 - LED * 3 - 0, r_Io);
		PCA9745_Set_IREFx(tile->p, dev, 15 - LED * 3 - 1, g_Io);
		PCA9745_Set_IREFx(tile->p, dev, 15 - LED * 3 - 2, b_Io);
	}
}

void LED_Tile_Set_LED_Color(LED_Tile *tile, uint16_t dev, uint8_t LED, uint8_t red, uint8_t green, uint8_t blue){
	PCA9745_Set_PWMx(tile->p, dev, 15 - LED * 3 - 0, red);
	PCA9745_Set_PWMx(tile->p, dev, 15 - LED * 3 - 1, green);
	PCA9745_Set_PWMx(tile->p, dev, 15 - LED * 3 - 2, blue);
}

void LED_Tile_Twinkle_Start(LED_Tile *tile){
	tile->twinkle_en = 1;
	for(uint8_t i = 0; i < TWINKLE_NUM; i++){
		tile->twinkle[i].active = 0;
	}
}

void LED_Tile_Twinkle_Stop(LED_Tile *tile){
	tile->twinkle_en = 0;
}

void LED_Tile_Twinkle_Update(LED_Tile *tile){
	if(tile->twinkle_en == 1){
		for(uint8_t i = 0; i < TWINKLE_NUM; i++){
			if(tile->twinkle[i].active == 1){
				tile->twinkle[i].t += TWINKLE_TIME_STEP;
				float scale = f_brightness(tile->twinkle[i].a_0, tile->twinkle[i].a_1, tile->twinkle[i].t) * tile->twinkle[i].scale;
				uint8_t r = tile->twinkle[i].r * scale;
				uint8_t g = tile->twinkle[i].g * scale;
				uint8_t b = tile->twinkle[i].b * scale;
				LED_Tile_Set_LED_Color(tile, tile->twinkle[i].dev, tile->twinkle[i].led, r, g, b);
				if(r <= 1 && g <= 1 && b <= 1 && tile->twinkle[i].t > tile->twinkle[i].t_max){
					tile->twinkle[i].active = 0;
				}
			}
		}

		uint8_t chance = rand() % 1000;
		if(chance < TWINKLE_CHANCE){
			LED_Tile_Twinkle_Add(tile);
		}
	}
}

void LED_Tile_Twinkle_Add(LED_Tile *tile){
	uint16_t dev = rand() % NUM_TILES;
	uint8_t led = rand() % 5;
	uint8_t found = 0;
	for(uint8_t i = 0; i < TWINKLE_NUM; i++){
		if(tile->twinkle[i].dev == dev && tile->twinkle[i].led == led && tile->twinkle[i].active == 1){
			found = 1;
		}
	}
	if(found == 0){
		for(uint8_t i = 0; i < TWINKLE_NUM; i++){
			if(tile->twinkle[i].active == 0){
				tile->twinkle[i].active = 1;
				tile->twinkle[i].dev = dev;
				tile->twinkle[i].led = led;
				tile->twinkle[i].t = 0.0f;
				tile->twinkle[i].r = rand() % 255;
				tile->twinkle[i].g = rand() % 255;
				tile->twinkle[i].b = rand() % 255;
				tile->twinkle[i].a_0 = ((float)(rand() % 100)) / 100.0f;
				tile->twinkle[i].a_1 = tile->twinkle[i].a_0 + ((float)(rand() % 100)) / 100.0f;
				tile->twinkle[i].t_max = log(tile->twinkle[i].a_1 / tile->twinkle[i].a_0) / (tile->twinkle[i].a_1 - tile->twinkle[i].a_0);
				tile->twinkle[i].scale = 1.0f / f_brightness(tile->twinkle[i].a_0, tile->twinkle[i].a_1, tile->twinkle[i].t_max);
				break;
			}
		}
	}
}

float f_brightness(float a, float b, float t){
	return exp(-a*t) - exp(-b*t);
}

float Get_Intensity(float intensity, float a, float b){
	if(intensity > MAX_INTESITY){
		intensity = MAX_INTESITY;
	}
	if(intensity < 0){
		intensity = 0.0f;
	}

	float x = 0;
	for(uint8_t i = 0; i < 10; i++){
		x = x - f_x(x, a, b, intensity) / f_dx(x, a, b);
	}
	return x;
}

float f_x(float x, float a, float b, float c){
	return a*x*x + b*x - c;
}

float f_dx(float x, float a, float b){
	return 2*a*x + b;
}
