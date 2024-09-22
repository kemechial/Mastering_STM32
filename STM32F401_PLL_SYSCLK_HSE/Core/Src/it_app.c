/*
 * it.c
 *
 *  Created on: Sep 17, 2024
 *      Author: Lenovo
 */

#include "main.h"



void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();


}



