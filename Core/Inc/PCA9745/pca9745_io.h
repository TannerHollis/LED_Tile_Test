/*
 * pca9745_io.h
 *
 *  Created on: Jun 23, 2021
 *      Author: THollis
 */

#ifndef INC_PCA9745_PCA9745_IO_H_
#define INC_PCA9745_PCA9745_IO_H_

#include "main.h"

#define PCA9745_XFR_DELAY 10

typedef enum{
	NO_ERROR,
	SHORT_CIRCUIT,
	OPEN_CIRCUIT,
	DNE
} PCA9745_Error_TypeDef;

typedef struct {
	SPI_HandleTypeDef *hspi;
	GPIO_TypeDef *gpio_port_nCS;
	uint16_t gpio_pin_nCS;
	GPIO_TypeDef *gpio_port_nOE;
	uint16_t gpio_pin_nOE;

	//Must include for PCA9745.c to compile
	float r_ext;			//External Resistor value
	uint16_t num_dev;		//number of devices
	uint8_t *instr_buffer;
	uint8_t *data_buffer;
	uint8_t *rx_buffer;
} PCA9745;

PCA9745 Init_PCA9745(SPI_HandleTypeDef *hspi, GPIO_TypeDef *nCS_port, uint16_t nCS_pin, GPIO_TypeDef *nOE_port,	uint16_t nOE_pin);
void _PCA9745_Configure(PCA9745 *p, float r_ext, uint16_t num_dev, uint8_t *instr_buffer, uint8_t *data_buffer, uint8_t *rx_buffer);
void _PCA9745_CS(PCA9745 *p, uint8_t state);
void _PCA9745_OE(PCA9745 *p, uint8_t state);
void _PCA9745_Write(PCA9745 *p, uint8_t *instruction, uint8_t *data);
void _PCA9745_Read(PCA9745 *p, uint8_t instruction);
void _PCA9745_Format_Data(PCA9745 *p, uint16_t dev, uint8_t instruction, uint8_t data);
void _PCA9745_Set_SPI(PCA9745 *p, SPI_HandleTypeDef *hspi);
void _PCA9745_Set_CS(PCA9745 *p, GPIO_TypeDef *port, uint16_t pin);
void _PCA9745_Set_OE(PCA9745 *p, GPIO_TypeDef *port, uint16_t pin);

#endif /* INC_PCA9745_PCA9745_IO_H_ */
