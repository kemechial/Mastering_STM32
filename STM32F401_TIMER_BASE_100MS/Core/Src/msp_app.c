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

	  GPIO_InitTypeDef gpio_led;
	__HAL_RCC_GPIOC_CLK_ENABLE();

	    gpio_led.Pin = GPIO_PIN_13;
		gpio_led.Mode = GPIO_MODE_OUTPUT_PP;
		gpio_led.Pull = GPIO_NOPULL;

		HAL_GPIO_Init(GPIOC, &gpio_led);


}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{

	__HAL_RCC_TIM3_CLK_ENABLE();
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);

}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart;
	//Enable the clock for uart and GPIO pins
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//Do the pin mux configurations
	gpio_uart.Pin = GPIO_PIN_9;
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Pull = GPIO_PULLUP;
	gpio_uart.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_uart.Alternate = GPIO_AF7_USART1; //USART1 TX
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_10; //USART1 RX
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	//Enable IRQ and set up the priority
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);

}

