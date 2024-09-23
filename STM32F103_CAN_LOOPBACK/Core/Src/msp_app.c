/*
 * msp.c
 *
 *  Created on: Sep 17, 2024
 *      Author: Lenovo
 */
#include <main_app.h>


void HAL_MspInit(void)
{

	// there would be a HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup) call here.
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
	// On F401 Black pill blue led is connected to PC13

}



void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart;
	//Enable the clock for uart and GPIO pins
	__HAL_RCC_USART3_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	memset(&gpio_uart,0,sizeof(gpio_uart)); //there might be garbage values
	//Do the pin mux configurations
	gpio_uart.Pin = GPIO_PIN_10; //USART3 TX
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOB, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_11; //USART3 RX
	gpio_uart.Mode = GPIO_MODE_INPUT;
	gpio_uart.Pull =  GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &gpio_uart);


}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
	GPIO_InitTypeDef gpio_can;
	__HAL_RCC_CAN1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	memset(&gpio_can,0,sizeof(gpio_can)); //there might be garbage values
	//Do the pin mux configurations
	gpio_can.Pin = GPIO_PIN_12; //CAN TX
	gpio_can.Mode = GPIO_MODE_AF_PP;
	gpio_can.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOA, &gpio_can);

	gpio_can.Pin = GPIO_PIN_11; //CAN RX
	gpio_can.Mode = GPIO_MODE_INPUT;
	gpio_can.Pull =  GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &gpio_can);

}



