/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "max2870.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t cur_tick = 0;
uint32_t tmp_tick = 0;

uint8_t tmp[64];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define FROMCHAR(c) (c > 57 ? c - 87 : c - 48)

uint16_t to_int(uint8_t* tmp, uint8_t base)
{
  uint16_t ret = 0;
  ret  = FROMCHAR(tmp[0]) * base * base * base;
  ret += FROMCHAR(tmp[1]) * base * base;
  ret += FROMCHAR(tmp[2]) * base;
  ret += FROMCHAR(tmp[3]) ;
  return ret;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  uint8_t hallo[] = "\n\rHallo!\n\r";
  HAL_UART_Transmit(&huart2, hallo, sizeof(hallo), HAL_MAX_DELAY);

  max2870_init(MAX2870_PFD);
  max2870_dump();

  HAL_UART_Receive_IT(&huart2, &rx_data, 1);

  //max2870_init();
  //max2870_power_on(1);
  //max2870_set_pfd(32, 2);
  //max2870_set_rf_out_a(1200.0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


	if (answ_ready == 1)
	{
		//HAL_UART_Transmit(&huart2, rx_buff, rx_cnt, HAL_MAX_DELAY);
		if (rx_cnt == 2)
		{
			if ((rx_buff[0] == 'r')||(rx_buff[0] == 'R'))
			{
				sprintf(tmp,"reset:0\n\r");
				HAL_UART_Transmit(&huart2, tmp, 9, HAL_MAX_DELAY);

				max2870_init(MAX2870_PFD);
				//max2870_dump();

			}
			else if ((rx_buff[0] == 'o')||(rx_buff[0] == 'O'))
			{
				sprintf(tmp,"out off:0\n\r");
				HAL_UART_Transmit(&huart2, tmp, 11, HAL_MAX_DELAY);
				//max2870_power_on(0);
				//max2870_outA(3);
				max2870_outA(0);
				max2870_write_data(0);
			}
			else
			{
				HAL_UART_Transmit(&huart2, "E02\n\r", 5, HAL_MAX_DELAY);
			}
		}
		else if (rx_cnt == 3)
		{
			if ((rx_buff[0] == 'p')||(rx_buff[0] == 'p'))
			{
				if (rx_buff[1] == '0')
				{
					sprintf(tmp,"power:0\n\r");
					HAL_UART_Transmit(&huart2, tmp, 9, HAL_MAX_DELAY);
					max2870_power(0);
					max2870_write_data(0);
				}
				else
				{
					sprintf(tmp,"power:1\n\r");
					HAL_UART_Transmit(&huart2, tmp, 9, HAL_MAX_DELAY);
					max2870_power(1);
					max2870_write_data(0);
				}
			}
			else
			{
				HAL_UART_Transmit(&huart2, "E02\n\r", 5, HAL_MAX_DELAY);
			}
		}
		else if (rx_cnt == 6)
		{
			if ((rx_buff[0] == 'f')||(rx_buff[0] == 'F'))
			{
				//rx_buff[rx_cnt-1] = '\n';
				//rx_buff[rx_cnt] = '\r';
				//HAL_UART_Transmit(&huart2, &rx_buff[1], rx_cnt+1, HAL_MAX_DELAY);
				int freq = to_int(&rx_buff[1], 10);
				double fres = max2870_set_rf_out_a((float)freq);
				sprintf(tmp,"freq:%04d\n\r", (int)fres);
				HAL_UART_Transmit(&huart2, tmp, 11, HAL_MAX_DELAY);
				max2870_outA(1);
				max2870_write_data(0);
				//max2870_dump();
			}
			else
			{
				HAL_UART_Transmit(&huart2, "E02\n\r", 5, HAL_MAX_DELAY);
			}
		}
		else
		{
			HAL_UART_Transmit(&huart2, "E01\n\r", 5, HAL_MAX_DELAY);
		}
		answ_ready = 0;
		rx_cnt = 0;
	}

	cur_tick = HAL_GetTick();
	if (cur_tick - tmp_tick >= 200){
	  tmp_tick = cur_tick;

	  HAL_GPIO_TogglePin(LED_PORT, LED_PIN);  //toggle green led

	}

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
