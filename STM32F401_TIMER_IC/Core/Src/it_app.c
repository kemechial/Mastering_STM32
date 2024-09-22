/*
 * it.c
 *
 *  Created on: Sep 17, 2024
 *      Author: Lenovo
 */

#include <main_app.h>

extern TIM_HandleTypeDef htim3;

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();


}



void TIM3_IRQHandler(void)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	HAL_TIM_IRQHandler(&htim3);
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

}
