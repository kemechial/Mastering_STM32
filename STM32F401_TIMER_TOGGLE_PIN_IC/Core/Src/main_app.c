/*
 * main.c
 *
 *  Created on: Sep 17, 2024
 *      Author: Lenovo
 */

#include <main_app.h>
#include "string.h"
#include "stdio.h"


void SystemClockConfig(uint8_t);
void Error_handler(void);
void TIM3_Init(void);
void UART1_Init(void);
void LSE_Configuration(void);

UART_HandleTypeDef huart1;
uint8_t msg[100];
uint16_t input_captures[2]={0};
uint8_t count=0;
uint8_t capture_flag = FALSE;
uint16_t capture_difference = 0;
TIM_HandleTypeDef htim3;

double timer3_cnt_res = 0;
double timer3_cnt_freq = 0;
double signal_time_period = 0;
double signal_freq = 0;
uint32_t PCLK_freq = 0;

int main(void)
{
	SystemClockConfig(CLOCK_FREQ_50MHz);
	LSE_Configuration(); // this must on top of other init functions which start port clocks, then it gives an error

	HAL_Init();
	UART1_Init();
	TIM3_Init();
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);


	while(1){

		if (capture_flag){
				if(input_captures[1]>input_captures[0]){
					capture_difference=input_captures[1]-input_captures[0];
				}else{

					capture_difference=(TIM3->ARR-input_captures[0])+input_captures[1];
				}
				PCLK_freq = HAL_RCC_GetPCLK1Freq();
				timer3_cnt_freq = PCLK_freq *2 / (htim3.Init.Prescaler+1);
				timer3_cnt_res = 1/timer3_cnt_freq;
				signal_time_period = capture_difference*timer3_cnt_res;
                signal_freq = 1/signal_time_period;
                count=0;
		        capture_flag = FALSE;
		}

					memset(msg,0,sizeof(msg));
				   sprintf((char *)msg,"capture difference: %d\r\n", capture_difference);
				   HAL_UART_Transmit(&huart1,  msg, (uint16_t)  strlen((char *)msg), HAL_MAX_DELAY);
		            memset(msg,0,sizeof(msg));
				   sprintf((char *)msg,"signal frequency: %.2lf\r\n", signal_freq);
				   HAL_UART_Transmit(&huart1,  msg, (uint16_t)  strlen((char *)msg), HAL_MAX_DELAY);
				   HAL_Delay(500);

	}




	return 0;
}

void TIM3_Init(void)
{
	TIM_IC_InitTypeDef tim3_ic_cfg;
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;//htim3.Init.Prescaler = 39; This is for 100 ms.
	htim3.Init.Period = 0xFFFF;
	if(HAL_TIM_IC_Init(&htim3) != HAL_OK){
		Error_handler();
	}
	tim3_ic_cfg.ICFilter = 0;
	tim3_ic_cfg.ICPolarity = TIM_ICPOLARITY_RISING;
	tim3_ic_cfg.ICPrescaler = TIM_ICPSC_DIV1;
	tim3_ic_cfg.ICSelection = TIM_ICSELECTION_DIRECTTI;

    if(HAL_TIM_IC_Init(&htim3)!= HAL_OK){
		Error_handler();
	};

    if(HAL_TIM_IC_ConfigChannel(&htim3, &tim3_ic_cfg, TIM_CHANNEL_2)!= HAL_OK){
 		Error_handler();
 	};

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

void SystemClockConfig(uint8_t clock_freq)
{
	RCC_OscInitTypeDef osc_init;
	RCC_ClkInitTypeDef clk_init;
	uint32_t FlashLatency;

	osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
	osc_init.HSEState = RCC_HSE_ON;
	osc_init.LSEState = RCC_LSE_ON;
	osc_init.PLL.PLLState = RCC_PLL_ON;
	osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    switch(clock_freq){
    	case CLOCK_FREQ_50MHz:
    		osc_init.PLL.PLLM = 25;
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

     		    osc_init.PLL.PLLM = 25;
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

	__HAL_RCC_HSI_DISABLE(); //Clock powered by HSE, now can disable HSI

    //SysTick Configuration for new clock setup
	//Input value is based on HCLK that SysTick would tick every 1 ms.
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000); // one count period is 1/HCLK so it must count HCLK /1000 to get 1 ms
    //Configure clk source as HCLK
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);


}

void LSE_Configuration(void)
{

	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_LSE, RCC_MCODIV_1);

}



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(!capture_flag){ // we need to block callback while the frequency is calculated in main function
		if(!count){
			input_captures[count++]=__HAL_TIM_GET_COMPARE(htim,TIM_CHANNEL_2);

		}else{

			input_captures[count]=__HAL_TIM_GET_COMPARE(htim,TIM_CHANNEL_2);
			capture_flag=TRUE;
		}
	}

}


