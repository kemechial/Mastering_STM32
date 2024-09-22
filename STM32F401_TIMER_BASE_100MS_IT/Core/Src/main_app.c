/*
 * main.c
 *
 *  Created on: Sep 17, 2024
 *      Author: Lenovo
 */

#include <main_app.h>
#include "string.h"
#include "stdio.h"


void SystemClockConfig(void);
void Error_handler(void);
void TIM3_Init(void);
void UART1_Init(void);
UART_HandleTypeDef huart1;
uint8_t msg[100];

TIM_HandleTypeDef htim3;


int main(void)
{

	HAL_Init();
	SystemClockConfig();
	TIM3_Init();
	UART1_Init();
	HAL_TIM_Base_Start_IT(&htim3);

	while(1){

	}




	return 0;
}

void TIM3_Init(void)
{
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 39;//htim3.Init.Prescaler = 39; This is for 100 ms.
	htim3.Init.Period = 40000-1;
	HAL_TIM_Base_Init(&htim3);
}


void SystemClockConfig(void)
{


}

void UART1_Init()
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;

	if(HAL_UART_Init(&huart1) != HAL_OK){
		Error_handler();
	}

}

void Error_handler(void)
{

   while(1);

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);


}

