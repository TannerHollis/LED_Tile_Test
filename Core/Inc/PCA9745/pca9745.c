/*
 * pca9745.c
 *
 *  Created on: Jun 23, 2021
 *      Author: THollis
 */

#include "main.h"
#include "pca9745.h"
#include "pca9745_instr.h"
#include "pca9745_io.h"

/**
  * @brief  Set PWM of Channel x
  * @note	The selected channel is set to a specific current value in mA (current).
  * 		See pg. 23 of the datasheet for equation.
  * 		Register values:
  *
  * @param  PCA9745 *p, uint16_t dev, uint8_t x, float current
  * @retval None
  */
void PCA9745_Set_PWMx(PCA9745 *p, uint16_t dev, uint8_t channel, uint8_t data){
	_PCA9745_Format_Data(p, dev, PWM0 + channel, data);
	_PCA9745_Write(p, p->instr_buffer, p->data_buffer);
}

/**
  * @brief  Set Io_LED of Channel x
  * @note	The selected channel is set to a specific current value in mA (current).
  * 		Register values:
  * 	   *0x00 - 0 (mA)
  * 		0x11 - 255 * 900 / R_ext * 0.25 (mA)
  *
  * @param  PCA9745 *p, uint16_t dev, uint8_t channel, float current
  * @retval None
  */
void PCA9745_Set_IREFx(PCA9745 *p, uint16_t dev, uint8_t channel, float current){
	uint8_t data = (uint8_t)((4 * p->r_ext * current) / 900);
	_PCA9745_Format_Data(p, dev, IREF0 + channel, data);
	_PCA9745_Write(p, p->instr_buffer, p->data_buffer);
}

/**
  * @brief  Sleep the PCA9745
  * @note	Sleep the PCA9745 LED driver by writing to MODE1 register with bit mask.
  * 	   *0 - Normal Mode
  * 		1 - Sleep
  * @param  PCA9745 *p, uint16_t dev, uint8_t state
  * @retval None
  */
void PCA9745_Set_Sleep(PCA9745 *p, uint16_t dev, uint8_t state){
	uint8_t data = 0x00 | (state << 4);
	_PCA9745_Format_Data(p, dev, MODE1, data);
	_PCA9745_Write(p, p->instr_buffer, p->data_buffer);
}

/**
  * @brief  Set the LED driver mode
  * @note	Set the LED driver mode for a given channel, states:
  * 		00 - LED driver is off.
  * 		01 - LED driver is fully on (individual brightness and group dimming/blinking not controlled)
  * 	   *10 - LED driver individual brightness can be controlled through its PWMx or PWMALL register
  * 		11 - LED driver individual brightness and group dimming/blinking can be controlled through
  * 			its PWM and GRPPWM registers
  * 		NOTE: This will not work unless MISO is enabled in SPI
  * @param  PCA9745 *p, uint16_t dev, uint8_t channel, uint8_t state
  * @retval None
  */
void PCA9745_Set_LEDOUTx(PCA9745 *p, uint16_t dev, uint8_t channel, uint8_t state){
	uint8_t instruction = LEDOUT0 + channel / 4;
	_PCA9745_Read(p, instruction);
	uint8_t reg = p->rx_buffer[dev];
	uint8_t data = (state << ((channel % 4) * 2));
	_PCA9745_Format_Data(p, dev, instruction, (reg & ~data) | data);
	_PCA9745_Write(p, p->instr_buffer, p->data_buffer);
}

/**
  * @brief  Check if the Temperature is OK
  * @note	Check for the TEMPOK bit is flagged in MODE2 register on a given device.
  * 		NOTE: This will not work unless MISO is enabled in SPI
  * @param  PCA9745 *p, uint16_t dev
  * @retval None
  */
uint8_t PCA9745_Check_Temperature(PCA9745 *p, uint16_t dev){
	_PCA9745_Read(p, MODE2);
	uint8_t reg = p->rx_buffer[dev];
	return ~(reg & (0x01 << 7));
}

/**
  * @brief  Check for LED Driver Errors
  * @note	Check for LED driver errors on a given device. If the error bit is flagged,
  * 		check on subsequent EFLAGx registers for that error. Then store the error
  * 		information in an PAC9745_Error_TypeDef array, e.
  * 		NOTE: This will not work unless MISO is enabled in SPI
  * @param  PCA9745 *p, uint16_t dev, PCA9745_Error_TypeDef *e
  * @retval None
  */
void PCA9745_Check_Errors(PCA9745 *p, uint16_t dev, PCA9745_Error_TypeDef *e){
	_PCA9745_Read(p, MODE2);
	uint8_t reg = p->rx_buffer[dev];
	uint8_t is_error = (reg & 0x01 << 6) >> 6;
	if (is_error){
		uint8_t flags[4 * p->num_dev];
		for(uint8_t i = 0; i < 4 * p->num_dev; i++){
			_PCA9745_Read(p, EFLAG0 + i);
			flags[i] = p->rx_buffer[dev];
			for(uint8_t j = 0; j < 4; j++){
				e[i * 4 + j] = (flags[i] & (0x03 << j)) >> j;
			}
		}
	}
	else{
		for(uint8_t i = 0; i < 16 * p->num_dev; i++){
			e[i] = NO_ERROR;
		}
	}
}
