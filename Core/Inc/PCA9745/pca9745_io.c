/*
 * pca9745_io.c
 *
 *  Created on: Jun 23, 2021
 *      Author: THollis
 */

#include "pca9745_io.h"
#include "main.h"

PCA9745 Init_PCA9745(SPI_HandleTypeDef *hspi, GPIO_TypeDef *nCS_port, uint16_t nCS_pin, GPIO_TypeDef *nOE_port,	uint16_t nOE_pin){
	PCA9745 p;

	_PCA9745_Set_SPI(&p, hspi);
	_PCA9745_Set_CS(&p, nCS_port, nCS_pin);
	_PCA9745_Set_OE(&p, nOE_port, nOE_pin);
	_PCA9745_OE(&p, 0);

	return p;
}

void _PCA9745_Configure(PCA9745 *p, float r_ext, uint16_t num_dev, uint8_t *instr_buffer, uint8_t *data_buffer, uint8_t *rx_buffer){
	p->instr_buffer = instr_buffer;
	p->data_buffer = data_buffer;
	p->rx_buffer = rx_buffer;

	p->r_ext = r_ext;
	p->num_dev = num_dev;
}

void _PCA9745_CS(PCA9745 *p, uint8_t state){
	HAL_GPIO_WritePin(p->gpio_port_nCS, p->gpio_pin_nCS, state);
}

void _PCA9745_OE(PCA9745 *p, uint8_t state){
	HAL_GPIO_WritePin(p->gpio_port_nOE, p->gpio_pin_nOE, state);
}

void _PCA9745_Write(PCA9745 *p, uint8_t *instruction, uint8_t *data){
	uint8_t transfer_buffer[2];
	_PCA9745_CS(p, 0);
	for(uint16_t i = 0; i < p->num_dev;i++){
		transfer_buffer[0] = (instruction[i] << 1) | 0x00;
		transfer_buffer[1] = data[i];
		HAL_SPI_Transmit(p->hspi, transfer_buffer, sizeof(transfer_buffer), PCA9745_XFR_DELAY);
	}
	_PCA9745_CS(p, 1);
}

void _PCA9745_Read(PCA9745 *p, uint8_t instruction){
	uint8_t transfer_buffer[] = {(instruction << 1) | 0x01, 0xFF};
	_PCA9745_CS(p, 0);
	for(uint16_t i = 0; i < p->num_dev; i++){
		HAL_SPI_Transmit(p->hspi, transfer_buffer, sizeof(transfer_buffer), PCA9745_XFR_DELAY);
	}
	_PCA9745_CS(p, 1);
	_PCA9745_CS(p, 0);
	uint8_t receive_buffer[2 * p->num_dev];
	HAL_SPI_Receive(p->hspi, receive_buffer, sizeof(receive_buffer), PCA9745_XFR_DELAY);
	_PCA9745_CS(p, 1);
	for(uint16_t i = 0; i < p->num_dev; i++){
		p->rx_buffer[i] = receive_buffer[i * 2 + 1];
	}
}

void _PCA9745_Format_Data(PCA9745 *p, uint16_t dev, uint8_t instruction, uint8_t data){
	for(uint16_t i = 0; i < p->num_dev; i++){
		if(i == dev){
			p->instr_buffer[i] = instruction;
			p->data_buffer[i] = data;
		}
		else{
			p->instr_buffer[i] = 0xFF;
			p->data_buffer[i] = 0xFF;
		}
	}
}

void _PCA9745_Set_SPI(PCA9745 *p, SPI_HandleTypeDef *hspi){
	p->hspi = hspi;
}

void _PCA9745_Set_CS(PCA9745 *p, GPIO_TypeDef *port, uint16_t pin){
	p->gpio_port_nCS = port;
	p->gpio_pin_nCS = pin;
}

void _PCA9745_Set_OE(PCA9745 *p, GPIO_TypeDef *port, uint16_t pin){
	p->gpio_port_nOE = port;
	p->gpio_pin_nOE = pin;
}
