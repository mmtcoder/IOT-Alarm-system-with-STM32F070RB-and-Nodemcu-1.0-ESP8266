# IOT-Alarm-system-with-STM32F070RB-and-Nodemcu-1.0-ESP8266

This project is used Mqtt Protocol for Iot project.I got my server from "HIVEMQ" company.
( They give you a free mqtt server.Of course, they have some limitations but it will work easily for our application:)
Main goal is to use STM32... with ESP8266 for Iot application. 
Communication protocol between microcontrollers : UART BaudRate : 9600
Rest of settings are default values.

(Note: You can only make this project by using Nodemcu Esp8266)

Used Materials :
1) STM32F070RB
2) Nodemcu 1.0 ESP8266
3) QAPASS 16 x 2 LED with I2C module
4) PIR sensor
5) Buzzer
6) 3.3v to 5v logic level converter( It is not necessary but GPIO output and input voltages aren't the same with each other)

IMPORTANT NOTE: For Nodemcu ESP8266 libraries, you need to download "EspMQTTClient" and "PubSubClient" libraries 

NODEMCU ESP8266 PINS:

D9 : Rx

D10 :Tx

GND

STM32F070RB PINS :

PA10 :USART1_RX

PA9 : USART1_TX

PC4 : Buzzer(GPIO_Output)

PC8 : PIR(GPIO_EXTI)

PB9 : I2C1_SDA

PB8 : I2C1_SCL


Beginning of the Microcontrollers

When ESP8266 start to run, It connects to wifi and then mqtt server.For testing to
connect to mqtt server, It sends "hello world" message and subscribe the topic
you defined.

When STM32 start to run, It sends to "Welcome" message and "ALARM STATUS IS DEACTIVE" to LED.
It waits receiving code from ESP8266.

You should check the code and need to remember which code is what does it mean..

//Receiving from ESP8266

 uint8_t ALARM_ACTIVATE_CODE [] = {'2','0'};
 
 uint8_t ALARM_DEACTIVATE_CODE [] = {'2','1'};
 
 uint8_t MSG_SENT_TO_CLT [] = {'3','0'};
 
 uint8_t NOMSG_SENT_TO_CLT [] = {'3','1'};

//Sending to ESP8266

 uint8_t ALARM_ACTIVATED_CODE [] ={'2','3'};
 
 uint8_t ALARM_DEACTIVATED_CODE [] ={'2','4'};
 
 uint8_t SYSTEM_ALARMED_CODE [] = {'2','5'};


**********ALARM SCENARIO*********


Firstly, we send a alarm_activate_code (20) code to esp from mqtt server
and then If ESP has mqtt connection, it send to alarm_activated_code
(23) to server like mobile phone, computer ( for this example hivemq 
websocket client) after that ESP sends alarm_activated_code to STM32

-If Stm32 reads this code correctly, system is activated but It can't receive
correct code, It write "WE COULD NOT GET TRUE RESPONSE" to LED

-If Stm32 receive 31 code(NOMSG_SENT_TO_CLT), It write 
"COMMUNICATION ERROR WITH ESP8266" to LED.


We assume that every step is succeeded, System will be activated.
When PIR sensor detects a motion, ALARM_STATUS will be alarmed,
Stm32 sends SYSTEM_ALARMED_CODE to ESP8266(esp8266 sends this
message to client) and
Stm32 writes "OBJECT DETECTED" to LED and buzzer is activated.


While Alarm status is SYSTEM_ALARMED, you can either deactivate the alarm
by sending ALARM_DEACTIVATE_CODE(21 code) or wait until alarm timeout 
duration is reached(System deactivate the alarm automatically).



As an extra,When ALARM_STATUS = ALARM_ACTIVATED(not SYSTEM_ALARMED)
you can deactivate the alarm by sending deactivate code.

If you send alarm_activate_code to Stm32 while ALARM_STATUS =
ALARM_ACTIVATED, stm32 write "ALARM IS ALREADY ACTIVE" to LED
and process doesn't affect.


