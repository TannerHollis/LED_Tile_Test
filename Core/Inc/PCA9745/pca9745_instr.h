/*
 * pca9745_instr.h
 *
 *  Created on: Jun 23, 2021
 *      Author: THollis
 */

#ifndef INC_PCA9745_INSTR_H_
#define INC_PCA9745_INSTR_H_

#define MODE1 		0x00
#define MODE2 		0x01

#define LEDOUT0		0x02
#define LEDOUT1		0x03
#define LEDOUT2 	0x04
#define LEDOUT3		0x05

#define GRPPWM		0x06
#define GRPFREQ		0x07

#define PWM0		0x08
#define PWM1		0x09
#define PWM2		0x0A
#define PWM3		0x0B
#define PWM4		0x0C
#define PWM5		0x0D
#define PWM6		0x0E
#define PWM7		0x0F
#define PWM8		0x10
#define PWM9		0x11
#define PWM10		0x12
#define PWM11		0x13
#define PWM12		0x14
#define PWM13		0x15
#define PWM14		0x16
#define PWM15		0x17
#define PWMALL		0x40	//PWM all LEDs

#define IREF0		0x18
#define IREF1		0x19
#define IREF2		0x1A
#define IREF3		0x1B
#define IREF4		0x1C
#define IREF5		0x1D
#define IREF6		0x1E
#define IREF7		0x0F
#define IREF8		0x20
#define IREF9		0x21
#define IREF10		0x22
#define IREF11		0x23
#define IREF12		0x24
#define IREF13		0x25
#define IREF14		0x26
#define IREF15		0x27
#define IREFALL		0x41 //Output gain all LEDs

#define IREF_GRP0		0x2B
#define IREF_GRP1		0x2F
#define IREF_GRP2		0x33
#define IREF_GRP3		0x37

#define RAMP_RATE_GRP0	0x28
#define STEP_TIME_GRP0	0x29
#define HOLD_CNTL_GRP0	0x2A

#define RAMP_RATE_GRP1	0x2C
#define STEP_TIME_GRP1	0x2D
#define HOLD_CNTL_GRP1	0x2E

#define RAMP_RATE_GRP2	0x30
#define STEP_TIME_GRP2	0x31
#define HOLD_CNTL_GRP2	0x32

#define RAMP_RATE_GRP3	0x34
#define STEP_TIME_GRP3	0x35
#define HOLD_CNTL_GRP3	0x36

#define GRAD_MODE_SEL0	0x38
#define GRAD_MODE_SEL1	0x39

#define GRAD_GRP_SEL0	0x3A
#define GRAD_GRP_SEL1	0x3B
#define GRAD_GRP_SEL2	0x3C
#define GRAD_GRP_SEL3	0x3D

#define GRAD_CNTL	0x3E

#define OFFSET		0x3F

#define EFLAG0		0x42
#define EFLAG1		0x43
#define EFLAG2		0x44
#define EFLAG3		0x45

#endif /* INC_PCA9745_INSTR_H_ */
