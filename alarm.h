/*
 * alarm.h
 *
 *  Created on: 11 Ağu 2021
 *      Author: H3RK3S
 */

#ifndef INC_ALARM_H_
#define INC_ALARM_H_



#include "stdint.h"


void TransmitDataToEsp(uint8_t * pData,uint8_t length);


void TransmitDataToLED(char * pData);

void ReceiveDataFromESP(uint8_t * buffer);


uint8_t CompareBufferInfo(uint8_t buffer [],uint8_t bufLength,uint8_t message []);
#endif /* INC_ALARM_H_ */
