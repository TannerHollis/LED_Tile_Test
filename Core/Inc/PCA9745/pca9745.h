/*
 * pca9745.h
 *
 *  Created on: Jun 23, 2021
 *      Author: THollis
 */

#ifndef INC_PCA9745_H_
#define INC_PCA9745_H_

#include "pca9745_io.h"
#include "pca9745_instr.h"

void PCA9745_Set_PWMx(PCA9745 *p, uint16_t dev, uint8_t channel, uint8_t data);
void PCA9745_Set_IREFx(PCA9745 *p, uint16_t dev, uint8_t channel, float current);
void PCA9745_Set_Sleep(PCA9745 *p, uint16_t dev, uint8_t state);
void PCA9745_Set_LEDOUTx(PCA9745 *p, uint16_t dev, uint8_t channel, uint8_t state);
uint8_t PCA9745_Check_Temperature(PCA9745 *p, uint16_t dev);
void PCA9745_Check_Errors(PCA9745 *p, uint16_t dev, PCA9745_Error_TypeDef *e);

#endif /* INC_PCA9745_H_ */
