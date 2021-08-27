/*
 * alarm.c
 *
 *  Created on: 11 Agust 2021
 *      Author: EVERYBODY
 */

#include "alarm.h"
#include "i2c-lcd.h"
#include "main.h"

//Receiving from ESP8266
 uint8_t ALARM_ACTIVATE_CODE [] = {'2','0'};
 uint8_t ALARM_DEACTIVATE_CODE [] = {'2','1'};
 uint8_t MSG_SENT_TO_CLT [] = {'3','0'};
 uint8_t NOMSG_SENT_TO_CLT [] = {'3','1'};

//Sending to ESP8266
 uint8_t ALARM_ACTIVATED_CODE [] ={'2','3'};
 uint8_t ALARM_DEACTIVATED_CODE [] ={'2','4'};
 uint8_t SYSTEM_ALARMED_CODE [] = {'2','5'};

//Sending to LED
 char ALARM_ACTIVATE_MESSAGE [] = "ALARM ACTIVATED";
 char ALARM_DEACT_SYS_MESSAGE [] = "ALARM DEACTIVATED BY SYSTEM";
 char ALARM_DEACTI_MANL_MESSAGE [] = "ALARM DEACTIVATED BY USER";
 char SYSTEM_ALARMED_MESSAGE [] = "OBJECT DETECTED ";
 char MQTT_CLIENT_ERROR [] = "ERROR CAUSE OF CLIENT";
 char RECEIVING_DATA_ERROR [] = "COMMUNICATION ERROR WITH ESP8266";
 char WRONG_DATA [] = "WE COULD NOT GET TRUE RESPONSE";
 char HELLO_MESSAGE [] = "WELCOME :)";
 char ALARM_BEGINNING [] = "ALARM STATUS IS DEACTIVE";
 char ALREADY_ALARMED [] = "ALARM IS ALREADY ACTIVE";

 //Alarm STATUS
const uint8_t ALARM_ACTIVE = 1;
const uint8_t ALARM_DEACTIVE =2;
const uint8_t SYSTEM_ALERMED =3;
uint8_t ALARM_STATUS = ALARM_DEACTIVE;	//first Initialize

//You should set the duration in multiples of 5. Unit of this variable is second.
const uint8_t ALARM_DEACTIVATE_TIME = 10;

extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c1;
extern uint8_t bufUsart[];

uint8_t secResUsart =0;
// Normally we need just 2 byte but we get wanted code message 5h or 6th turn into response
// or response2 buffer so we have to define 6 or 10 size
// I defined these variables in order to observe in the debugging mode into global variable
uint8_t response2 [10];
uint8_t response [6];

void TransmitDataToEsp(uint8_t * pData,uint8_t length)
{

	HAL_UART_Transmit(&huart1, pData, length, 30);

}

void TransmitDataToLED(char * pData)
{
	lcd_clear();

	HAL_Delay(500);

	lcd_send_string ( pData);

	HAL_Delay(200);
}


void ReceiveDataFromESP(uint8_t * buffer)
{
	uint8_t * temp = buffer;


    if(ALARM_STATUS != ALARM_ACTIVE && ALARM_STATUS != SYSTEM_ALERMED)
    {
    	//if(temp[0] == ALARM_ACTIVATE_CODE[0] && temp[1] == ALARM_ACTIVATE_CODE[1])
    		if(CompareBufferInfo(temp, 6, ALARM_ACTIVATE_CODE))
    		{

    				//You don't need make count down like this :)
    				TransmitDataToLED("Alarm activating 3 second");
    				//TransmitData function takes 0.7 second.
    				HAL_Delay(300);
    				TransmitDataToLED("Alarm activating 2 second");
    				HAL_Delay(300);
    				TransmitDataToLED("Alarm activating 1 second");
    				HAL_Delay(300);
    				//Send Alarm activate code to ESP8266
    				TransmitDataToEsp(ALARM_ACTIVATED_CODE, 2);

    				//In this part, you must receive a response from ESP8266 in order to
    				//transmit your code to administive device( like mobil phones, Computer etc.)
    				//successfully  or not therefore I'm using this function in the "while" loop
    				// 1) If buffer is received true response from ESP, alarm will be activated
    				// 2) If buffer is received "NOMSG_SENT_TO_CLT", ESP lost its connection with HiveMq broker
    				// 3) If buffer is received another data, you will get "WRONG_DATA" message
    				while(HAL_UART_Receive(&huart1, (uint8_t *)response, 6, HAL_MAX_DELAY) != HAL_OK){};

    			if(response != 0)
    			{
					if(CompareBufferInfo(response, 6, MSG_SENT_TO_CLT))
					{
					TransmitDataToLED(ALARM_ACTIVATE_MESSAGE);
						ALARM_STATUS = ALARM_ACTIVE;


					}
					else if(CompareBufferInfo(response, 6, NOMSG_SENT_TO_CLT))
					{
						TransmitDataToLED(MQTT_CLIENT_ERROR);
						ALARM_STATUS = ALARM_DEACTIVE;

					}
					else
					{
						TransmitDataToLED(WRONG_DATA);
						ALARM_STATUS = ALARM_DEACTIVE;

					}

    			}

    		}
    		 else
    				{

    					TransmitDataToLED(RECEIVING_DATA_ERROR);
    					ALARM_STATUS = ALARM_DEACTIVE;


    				}

    }
    //This statement send to deactivate code manually while Alarm was Activated
    else if(ALARM_STATUS != ALARM_DEACTIVE && ALARM_STATUS != SYSTEM_ALERMED)
    {
    	  	if(CompareBufferInfo(temp, 6, ALARM_DEACTIVATE_CODE))
    	     {
			  TransmitDataToLED(ALARM_DEACTI_MANL_MESSAGE);
			  ALARM_STATUS = ALARM_DEACTIVE;

    	     }
    	  	//You don't need use this statement
    	else if(CompareBufferInfo(temp, 6, ALARM_ACTIVATE_CODE))
    	  {
    		  TransmitDataToLED(ALREADY_ALARMED);

    	  }

    }
    else if(ALARM_STATUS == SYSTEM_ALERMED)
    {
    	uint16_t curMiliTime = HAL_GetTick();
    	uint8_t  secondTime =0;
    	TransmitDataToLED(SYSTEM_ALARMED_MESSAGE);
    	//Activate Buzzer
    	HAL_GPIO_WritePin(GPIOC, Buzzer_Pin, SET);
    	TransmitDataToEsp(SYSTEM_ALARMED_CODE, 2);
    	/*
    	 * As soon as System is alarmed, while loop wait a receive from user.
    	 * If User don't transmit any data to STM32, "secondTime" variable holds passed time
    	 * When "secondTime " reached "ALARM_DEACTIVATE_TIME", system deactivates the alarm automatically
    	 * 5000 ms is good for getting response from user at a time
    	 */
    	while(secondTime < ALARM_DEACTIVATE_TIME  && (HAL_UART_Receive(&huart1, (uint8_t *)response2, 10, 5000) != HAL_OK)    )
    	{

		if((curMiliTime + 5000) < HAL_GetTick())
			{
				curMiliTime += 5000;
				secondTime = secondTime +5;
			}

    	}
    	if(secondTime < ALARM_DEACTIVATE_TIME)
    	{
    		if(response2 != 0)
			{
			   if(CompareBufferInfo(response2, 10, ALARM_DEACTIVATE_CODE))
				{

				TransmitDataToLED(ALARM_DEACTI_MANL_MESSAGE);
				HAL_GPIO_WritePin(GPIOC, Buzzer_Pin, RESET);
				ALARM_STATUS = ALARM_DEACTIVE;


				}
				else
				{
					TransmitDataToLED(WRONG_DATA);

				}
			}
    	}else
    	{
    		//You need to give information about alarm status to admin device
    		TransmitDataToEsp(ALARM_DEACTIVATED_CODE, 2);
    		TransmitDataToLED(ALARM_DEACT_SYS_MESSAGE);
    		HAL_GPIO_WritePin(GPIOC, Buzzer_Pin, RESET);
    		ALARM_STATUS = ALARM_DEACTIVE;

    	}


    }




}


uint8_t CompareBufferInfo(uint8_t buffer [],uint8_t bufLength,uint8_t message [])
{
	for(uint8_t k =0; k<bufLength ; k++)
	{
		if(buffer[k] == message[0])
		{
			if(k != (bufLength -1))
			{
				if(buffer[k+1] == message[1])
				{
					return 1;
				}
			}
			else{
				return 0;
			}
		}
	}
	return 0;

}
