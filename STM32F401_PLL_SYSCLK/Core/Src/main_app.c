/*
 * main.c
 *
 *  Created on: Sep 17, 2024
 *      Author: Lenovo
 */

#include "main.h"
#include "string.h"
#include "stdio.h"

UART_HandleTypeDef huart1;



void SystemClockConfig(uint8_t);
void UART1_Init(void);
void Error_handler(void);
void printClocks(void);

uint8_t msg[100];
char* user_data = "The application is running.\r\n";



int main(void)
{

	HAL_Init();

	SystemClockConfig(CLOCK_FREQ_80MHz);

	//clocks are modified, uart has to be initialized again.
	UART1_Init();

	while(1){

		   memset(msg,0,sizeof(msg));
		   sprintf((char *) msg,"SYSCLK: %ld\r\n", HAL_RCC_GetSysClockFreq());
	    HAL_UART_Transmit(&huart1,  msg, (uint16_t) strlen((char *)msg), HAL_MAX_DELAY);
	    HAL_Delay(500);
	    memset(msg,0,sizeof(msg));
	    sprintf((char *)msg,"HCLK: %ld\r\n", HAL_RCC_GetHCLKFreq());
		   HAL_UART_Transmit(&huart1, msg, (uint16_t)  strlen((char *)msg), HAL_MAX_DELAY);
		   HAL_Delay(500);
		   memset(msg,0,sizeof(msg));
		   sprintf((char *)msg,"PCLK1: %ld\r\n", HAL_RCC_GetPCLK1Freq());
		   HAL_UART_Transmit(&huart1,  msg, (uint16_t)  strlen((char *)msg), HAL_MAX_DELAY);
		   HAL_Delay(500);
		   memset(msg,0,sizeof(msg));
		   sprintf((char *)msg,"PCLK2: %ld\r\n", HAL_RCC_GetPCLK2Freq());
		   HAL_UART_Transmit(&huart1,  msg, (uint16_t)  strlen((char *)msg), HAL_MAX_DELAY);
		   HAL_Delay(500);


//
//	   SystemClockConfig(CLOCK_FREQ_80MHz);
//
//	   		//clocks are modified, uart has to be initialized again.
//	   		UART1_Init();
//	   		printClocks();
//	   	    HAL_Delay(1000);


	}




	return 0;
}





void SystemClockConfig(uint8_t clock_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	uint32_t FlashLatency;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	osc_init.HSIState = RCC_HSI_ON;
	osc_init.HSICalibrationValue = 0x10; //Default value 16
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSI;

    switch(clock_freq){
    	case CLOCK_FREQ_50MHz:
    		osc_init.PLL.PLLM = 16;
    		osc_init.PLL.PLLN = 100;
    		osc_init.PLL.PLLP = 2;

    		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
    									RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    	    clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    	    clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
    	    clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
    	    clk_init.APB2CLKDivider = RCC_HCLK_DIV1;
    	    FlashLatency = FLASH_ACR_LATENCY_1WS;
         break;

     	case CLOCK_FREQ_80MHz:

     		    osc_init.PLL.PLLM = 16;
        		osc_init.PLL.PLLN = 160;
        		osc_init.PLL.PLLP = 2;

        		clk_init.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
        									RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        	    clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        	    clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
        	    clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
        	    clk_init.APB2CLKDivider = RCC_HCLK_DIV1;
        	    FlashLatency = FLASH_ACR_LATENCY_2WS;
             break;


     	default:
     		return;

    }

	if(HAL_RCC_OscConfig(&osc_init) != HAL_OK){
		Error_handler();
	}


	if(HAL_RCC_ClockConfig(&clk_init, FlashLatency) != HAL_OK){
			Error_handler();
		}



    //SysTick Configuration for new clock setup
	//Input value is based on HCLK that SysTick would tick every 1 ms.
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000); // one count period is 1/HCLK so it must count HCLK /1000 to get 1 ms
    //Configure clk source as HCLK
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);




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

