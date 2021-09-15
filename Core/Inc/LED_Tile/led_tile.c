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

/**
  * @brief  Initialize LED Tile
  * @note	Initialize the LED Tile's ports, SPI, buffers and R_ext programming
  *
  * @note	NOTE: See "led_tile.h" for configuration
  *
  * @param  void
  * @retval None
  */
LED_Tile Init_LED_Tile(){
	LED_Tile tile;

	//Configure PCA9745 driver
	p = Init_PCA9745(&TILE_SPI, TILE_CS_PORT, TILE_CS_PIN, TILE_OE_PORT, TILE_OE_PIN);
	_PCA9745_Configure(&p, R_EXT, NUM_TILES, instr_buffer, data_buffer, rx_buffer);
	tile.p = &p;

	//Configure update timer
	tile.update_timer.htim = &TILE_TIM;
	tile.update_timer.tim_mhz = TILE_TIM_MHZ;
	tile.update_timer.update_freq = 100;
	return tile;
}

/**
  * @brief  Set Relative Intensity of LED
  * @note	Using a lookup function, that uses Newton-Raphson first order approximation
  * 		to find the current at a specified intensity, a 8-bit value is returned and
  * 		set using the IREFx registers.
  *
  * @param  LED_Tile *tile, uint16_t dev, uint8_t LED, float intensity
  * @retval None
  */
void LED_Tile_Set_LED_Intensity(LED_Tile *tile, uint16_t dev, uint8_t LED, float intensity){
	if(LED == 5){	//Set IR Intensity
		float ir_Io = Get_Intensity(intensity, IR_A, IR_B);
		PCA9745_Set_IREFx(tile->p, dev, 15, ir_Io);
	}
	else{	//Set RGB Intensity
		float r_Io = Get_Intensity(intensity, R_A, R_B);
		float g_Io = Get_Intensity(intensity, G_A, G_B);
		float b_Io = Get_Intensity(intensity, B_A, B_B);
		PCA9745_Set_IREFx(tile->p, dev, LED * 3 - 0, r_Io);
		PCA9745_Set_IREFx(tile->p, dev, LED * 3 - 1, g_Io);
		PCA9745_Set_IREFx(tile->p, dev, LED * 3 - 2, b_Io);
	}
}

/**
  * @brief  Set Color of LED
  * @note	Set the respective PWM register values of the RGB LED channels
  *
  * @param  LED_Tile *tile, uint16_t dev, uint8_t LED, uint8 red, uint8_t green, uint8_t blue
  * @retval None
  */
void LED_Tile_Set_LED_Color(LED_Tile *tile, uint16_t dev, uint8_t LED, uint8_t red, uint8_t green, uint8_t blue){
	PCA9745_Set_PWMx(tile->p, dev, LED * 3 + 0, red);
	PCA9745_Set_PWMx(tile->p, dev, LED * 3 + 1, green);
	PCA9745_Set_PWMx(tile->p, dev, LED * 3 + 2, blue);
}

/**
  * @brief  Set Color of LEDs on Each Tile
  * @note	Set the respective PWM register values of the RGB LED channels
  *
  * @param  LED_Tile *tile, uint8 red, uint8_t green, uint8_t blue
  * @retval None
  */
void LED_Tile_Set_LED_Color_All(LED_Tile *tile, uint8_t r, uint8_t g, uint8_t b){
	for(uint8_t led = 0; led < 5; led++){
		for(uint16_t dev = 0; dev < NUM_TILES; dev++){
			LED_Tile_Set_LED_Color(tile, dev, led, r, g, b);
		}
	}
}

/**
  * @brief  Set Color of LED
  * @note	Set the respective IR register values of the IR LED channel
  *
  * @param  LED_Tile *tile, uint16_t dev, uint8_t value
  * @retval None
  */
void LED_Tile_Set_IR_LED(LED_Tile *tile, uint16_t dev, uint8_t value){
	PCA9745_Set_PWMx(tile->p, dev, 15, value);
}

/**
  * @brief  Clear All Color in Tile
  * @note	Set the respective PWM register to zero
  *
  * @param  LED_Tile *tile, uint16_t dev
  * @retval None
  */
void LED_Tile_Clear(LED_Tile *tile, uint16_t dev){
	for(uint8_t i = 0; i < 16; i++){
		PCA9745_Set_PWMx(tile->p, dev, i, 0);
	}
}

/**
  * @brief  Clear All Color in All Tiles
  * @note	Set the respective PWM register to zero
  *
  * @param  LED_Tile *tile
  * @retval None
  */
void LED_Tile_Clear_All(LED_Tile *tile){
	for(uint8_t i = 0; i < NUM_TILES; i++){
		LED_Tile_Clear(tile, i);
	}
}

/**
  * @brief  Tests All Color in All Tiles
  * @note	Tests red, green, blue values of all LEDs in all tiles
  *
  * @param  LED_Tile *tile
  * @retval None
  */
void LED_Tile_Test_All(LED_Tile *tile){
	for(uint8_t r = 0; r < 255; r++){
		LED_Tile_Set_LED_Color_All(tile, r, 0, 0);
		HAL_Delay(1);
	}
	for(uint8_t g = 0; g < 255; g++){
		LED_Tile_Set_LED_Color_All(tile, 0, g, 0);
		HAL_Delay(1);
	}
	for(uint8_t b = 0; b < 255; b++){
		LED_Tile_Set_LED_Color_All(tile, 0, 0, b);
		HAL_Delay(1);
	}
}

/**
  * @brief  Initialize twinkle mode
  * @note	Set the twinkle chance frequency based on psuedo-random values
  * 		and set the maximum number of twinkles allowed at once.
  *
  * @note	WARNING: Increasing the twinkle number will greatly reduce maximum
  * 		update speed. Keep this number relatively low and keep it below the following limit:
  * 			0 < num <= NUM_TILES * 5
  *
  * @param  LED_Tile *tile, uint16_t chance, uint8_t num
  * @retval None
  */
void LED_Tile_Twinkle_Init(LED_Tile *tile, uint16_t chance, uint8_t num){
	tile->twinkle.chance = chance;
	tile->twinkle.num = num;
}

/**
  * @brief  Start twinkle mode
  * @note	Start the twinkle mode with a fixed time step.
  *
  * @note	This also sets all twinkles to inactive
  *
  * @param  LED_Tile *tile, uint16_t chance, float freq
  * @retval None
  */
void LED_Tile_Twinkle_Start(LED_Tile *tile, float freq){
	tile->twinkle.en = 1;
	tile->twinkle.time_step = 1.0f / freq;
	for(uint8_t i = 0; i < TWINKLE_NUM_MAX; i++){
		tile->twinkle.twinkles[i].active = 0;
	}
	Start_Update_Timer(tile, freq);
}

/**
  * @brief  Stop twinkle mode
  * @note	Stop the twinkle mode.
  *
  * @note	This also sets all of the currently active twinkle LED's to zero.
  *
  * @param  LED_Tile *tile
  * @retval None
  */
void LED_Tile_Twinkle_Stop(LED_Tile *tile){
	Stop_Update_Timer(tile);
	tile->twinkle.en = 0;
	for(uint8_t i = 0; i < tile->twinkle.num; i++){
		if(tile->twinkle.twinkles[i].active == 1){
			LED_Tile_Set_LED_Color(tile, tile->twinkle.twinkles[i].dev, tile->twinkle.twinkles[i].led, 0, 0, 0);
		}
	}
}

/**
  * @brief  Update twinkle mode
  * @note	Updates the twinkle mode. Iterate through the active twinkles and increment the time in the
  * 		exponential decay function. Each RGB value is determined using the following function:
  * 			scale = scale_init * exp(-a_0 * t) - exp(-a_1 * t);
  * 		Where scale_init is the unity gain factor for the decay function.
  *
  * @note	Create a random twinkle chance and if it is lower than the programmable twinkle chance, then
  * 		spawn a new twinkle.
  *
  * @param  LED_Tile *tile
  * @retval None
  */
void LED_Tile_Twinkle_Update(LED_Tile *tile){
	if(tile->twinkle.en == 1){
		for(uint8_t i = 0; i < tile->twinkle.num; i++){
			if(tile->twinkle.twinkles[i].active == 1){
				tile->twinkle.twinkles[i].t += tile->twinkle.time_step;
				float scale = f_brightness(tile->twinkle.twinkles[i].a_0, tile->twinkle.twinkles[i].a_1, tile->twinkle.twinkles[i].t) * tile->twinkle.twinkles[i].scale;
				uint8_t r = tile->twinkle.twinkles[i].r * scale;
				uint8_t g = tile->twinkle.twinkles[i].g * scale;
				uint8_t b = tile->twinkle.twinkles[i].b * scale;

				//If twinkle has reached a minimum value, set twinkle to inactive
				if(r <= 1 && g <= 1 && b <= 1 && tile->twinkle.twinkles[i].t > tile->twinkle.twinkles[i].t_max){
					LED_Tile_Set_LED_Color(tile, tile->twinkle.twinkles[i].dev, tile->twinkle.twinkles[i].led, 0, 0, 0);
					tile->twinkle.twinkles[i].active = 0;
				}
				else{
					LED_Tile_Set_LED_Color(tile, tile->twinkle.twinkles[i].dev, tile->twinkle.twinkles[i].led, r, g, b);
				}
			}
		}

		//If random value is less than programmable chance, spawn a twinkle
		if(rand() % TWINKLE_CHANCE < tile->twinkle.chance){
			LED_Tile_Twinkle_Add(tile);
		}
	}
}

/**
  * @brief  Add a twinkle
  * @note	Add a twinkle to the twinkles array. This function creates a twinkle and checks if there is already
  * 		a twinkle at that location, then it will skip creating a twinkle.
  *
  * @note	If there is a space for a new twinkle available, create necessary decay values, RGB values and scale.
  * 		In order to find the scale, a unity gain is found by using the maximum value of the decay function.
  *
  * @note	The decay values a_0 and a_1 are very important to determine the decay rate and shape of the twinkle.
  * 		A couple of limitations to these two variables will determine the outcome of the decay function:
  * 			a_1 != a_0, to guarantee exponential decay.
  * 		The smaller the difference between a_0 and a_1, the slower the decay rate. Some decays may last up to
  * 		30 seconds in the current configuration.
  *
  * @param  LED_Tile *tile
  * @retval None
  */
void LED_Tile_Twinkle_Add(LED_Tile *tile){
	uint16_t dev = rand() % NUM_TILES;
	uint8_t led = rand() % 5;
	uint8_t found = 0;
	for(uint8_t i = 0; i < tile->twinkle.num; i++){
		if(tile->twinkle.twinkles[i].dev == dev && tile->twinkle.twinkles[i].led == led && tile->twinkle.twinkles[i].active == 1){
			found = 1;
		}
	}
	if(found == 0){
		for(uint8_t i = 0; i < tile->twinkle.num; i++){
			if(tile->twinkle.twinkles[i].active == 0){	//If inactive twinkle within num of twinkles is found
				tile->twinkle.twinkles[i].active = 1;
				tile->twinkle.twinkles[i].dev = dev;
				tile->twinkle.twinkles[i].led = led;
				tile->twinkle.twinkles[i].t = 0.0f;
				tile->twinkle.twinkles[i].r = rand() % 255;
				tile->twinkle.twinkles[i].g = rand() % 255;
				tile->twinkle.twinkles[i].b = rand() % 255;
				tile->twinkle.twinkles[i].a_0 = (float)(rand() % 95 + 5) / 20.0f;
				tile->twinkle.twinkles[i].a_1 = tile->twinkle.twinkles[i].a_0 + (float)(rand() % 99 + 1) / 20.0f;
				tile->twinkle.twinkles[i].t_max = log(tile->twinkle.twinkles[i].a_1 / tile->twinkle.twinkles[i].a_0) / (tile->twinkle.twinkles[i].a_1 - tile->twinkle.twinkles[i].a_0);
				tile->twinkle.twinkles[i].scale = 1.0f / f_brightness(tile->twinkle.twinkles[i].a_0, tile->twinkle.twinkles[i].a_1, tile->twinkle.twinkles[i].t_max);
				break;
			}
		}
	}
}

/**
  * @brief  Calculate decay function
  * @note	Calculate the decay function using two constants.
  *
  * @param  float a, float b, float t
  * @retval float
  */
float f_brightness(float a, float b, float t){
	return exp(-a*t) - exp(-b*t);
}

/**
  * @brief  Calculate LED Current with Relative Intensity
  * @note	Calculate the current with given relative intensity using Newton-Raphson method of a simple
  * 		quadratic function. Using only 10 iterations of the Newton-Raphson method, the current
  * 		value can be approximated using the first derivative of the quadratic function and the previous
  * 		value.
  *
  * @param  float intensity, float a, float b
  * @retval float
  */
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

/**
  * @brief  Quadratic Formula
  *
  * @param  float x, float a, float b, float c
  * @retval float
  */
float f_x(float x, float a, float b, float c){
	return a*x*x + b*x - c;
}

/**
  * @brief  First Derivative of Quadratic Formula
  *
  * @param  float x, float a, float b
  * @retval float
  */
float f_dx(float x, float a, float b){
	return 2*a*x + b;
}

/**
  * @brief  Starts the Update Timer
  * @note	Configures the timer prescaler (PSC) and autoreload (ARR) registers for a given frequency.
  * 		Based on the timer MHz value, the timer prescaler is configured so that the autoreload
  * 		register is based on a 1MHz input.
  * 			freq = 50.0f
  * 			ARR = 1MHz / 50.0f = 20000 - 1 = 19999
  * 			freq_max = 1 MHz
  * 			freq_min = 15.26 Hz
  *
  * @param  LED_Tile *tile
  * @retval None
  */
void Start_Update_Timer(LED_Tile *tile, float freq){
	tile->update_timer.htim->Instance->PSC = (uint16_t) (tile->update_timer.tim_mhz - 1);
	tile->update_timer.htim->Instance->ARR = (uint16_t) (tile->update_timer.tim_mhz * 1000000 / tile->update_timer.htim->Instance->PSC / freq - 1);
	tile->update_timer.update_freq = freq;
	HAL_TIM_Base_Start_IT(tile->update_timer.htim);
}

/**
  * @brief  Stops the Update Timer
  *
  * @param  LED_Tile *tile
  * @retval None
  */
void Stop_Update_Timer(LED_Tile *tile){
	HAL_TIM_Base_Stop_IT(tile->update_timer.htim);
}

