/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ST7735\st7735.h"
#include "string.h"
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
SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void mostrarMatriz(char matriz[9], bool vazios, uint16_t xColor, uint16_t oColor) {
  int x = 0;
  int y = 0;
  uint16_t color;
  for (int i = 0; i < 9; ++i) {
	  if (matriz[i] == '_') {
		  color = WHITE;
	  } else if (matriz[i] == 'X'){
		  color = xColor;
	  } else {
		  color = oColor;
	  }
    if (vazios || matriz[i] != '_') {
      ST7735_WriteCharE(x, y, matriz[i], Font_16x26, color, BLACK);
    }
    if (i == 2 || i == 5) {
      y += 25;
      x = 0;
    } else {
      x += 30;
    }
  }
}

void atualizarCursor(int direcao, int* posicaoCursor, char matriz[9], int* x, int* y, uint16_t xColor, uint16_t oColor) {
  int cursor = *posicaoCursor;
  do {
    cursor += direcao;
    if (cursor > 8) {
      cursor = 0;
    } else if (cursor < 0) {
      cursor = 8;
    }

	if (direcao > 0) {
	  	  if (cursor == 3 || cursor == 6 || cursor == 0) {
	  		  *y += (cursor == 0) ? -50 : 25; // ternary operator
	  		  *x = 0;
	  	  } else {
	  		  *x = (cursor % 3) * 30;
	  	  }
	    } else {
	  	  if (cursor == 2 || cursor == 5 || cursor == 8) {
	  		  *y += (cursor == 8) ? 50 : -25;
	  		  *x = 60;
	  	  } else {
	  		  *x = (cursor % 3) * 30;
	  	  }
	}
  } while (matriz[cursor] != '_');
  *posicaoCursor = cursor;
  mostrarMatriz(matriz, true, xColor, oColor);
  HAL_Delay(250);
}

void selecionarCor(char letra, uint16_t* color) {
	int selec = 0;
	uint16_t colors[] = { WHITE, BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW };
	ST7735_FillScreen(BLACK);
	ST7735_WriteString(0, 0, "Selecione uma cor para", Font_11x18, WHITE, BLACK);
	while (1) {
		ST7735_WriteCharE(100, 20, letra, Font_11x18, colors[selec], BLACK);
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == 0) {
			if (selec < 6) {
				selec++;
			}
			HAL_Delay(250);
		} else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == 0) {
			if (selec > 0) {
				selec--;
			}
			HAL_Delay(250);
		}

		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == 0) {
			*color = colors[selec];
			break;
		}
	  }
	  selec = 0;
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  ST7735_Init();

  char matriz[9] = { '_', '_', '_', '_', '_', '_', '_', '_', '_' };
  int jogadorRodada = 1;
  int posicaoCursor = 0;
  int placar1 = 0;
  int placar2 = 0;
  int jogadas = 1;
  int x = 0;
  int y = 0;
  uint32_t last_time = 0;
  uint16_t xColor = WHITE;
  uint16_t oColor = WHITE;

  selecionarCor('X', &xColor);
  selecionarCor('O', &oColor);

  ST7735_FillScreen(BLACK);
  ST7735_WriteCharE(120, 20, 'x', Font_16x26, WHITE, BLACK);
  mostrarMatriz(matriz, true, xColor, oColor);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
	  if (HAL_GetTick() - last_time > 1000) {
		  last_time = HAL_GetTick();
		  ST7735_WriteCharE(x, y, jogadorRodada == 1 ? 'X' : 'O', Font_16x26, jogadorRodada == 1 ? xColor : oColor, BLACK);
	   } else if (HAL_GetTick() - last_time > 500) {
		ST7735_WriteCharE(x, y, ' ', Font_16x26, WHITE, BLACK);
	   }

    ST7735_WriteCharE(100, 20, placar1 + '0', Font_16x26, WHITE, BLACK);
    ST7735_WriteCharE(140, 20, placar2 + '0', Font_16x26, WHITE, BLACK);
    mostrarMatriz(matriz, false, xColor, oColor);

    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == 0) {
      atualizarCursor(1, &posicaoCursor, matriz, &x, &y, xColor, oColor);
    } else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == 0) {
      atualizarCursor(-1, &posicaoCursor, matriz, &x, &y, xColor, oColor);
    }

    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == 0) {
      if (matriz[posicaoCursor] == '_') {
        if (jogadorRodada == 1) {
          matriz[posicaoCursor] = 'X';
          jogadorRodada = 2;
		} else {
			matriz[posicaoCursor] = 'O';
			jogadorRodada = 1;
		}
        if (jogadas < 9) {
        	atualizarCursor(1, &posicaoCursor, matriz, &x, &y, xColor, oColor);
        }
        jogadas++;
      }

      if (jogadas >= 5) {
        mostrarMatriz(matriz, false, xColor, oColor);
        HAL_Delay(1000);
        int vencedor = 0;
        for (int i = 0; i < 9; i += 3) {
          if (matriz[i] != '_' && matriz[i] == matriz[i + 1] && matriz[i] == matriz[i + 2]) {
            vencedor = matriz[i] == 'X' ? 1 : 2;
            break;
          }
        }

        for (int i = 0; i < 3; ++i) {
          if (matriz[i] != '_' && matriz[i] == matriz[i + 3] && matriz[i] == matriz[i + 6]) {
            vencedor = matriz[i] == 'X' ? 1 : 2;
            break;
          }
        }


        if (matriz[0] != '_' && matriz[0] == matriz[4] && matriz[0] == matriz[8]) {
          vencedor = matriz[0] == 'X' ? 1 : 2;
        }
        if (matriz[2] != '_' && matriz[2] == matriz[4] && matriz[2] == matriz[6]) {
          vencedor = matriz[2] == 'X' ? 1 : 2;
        }

        if (vencedor) {
        	(vencedor == 1) ? placar1++ : placar2++;
        	if (placar1 == 2 || placar2 == 2) {
        		for (int i = GPIO_PIN_3; i < GPIO_PIN_7; i = i << 1) {
        			HAL_GPIO_WritePin(GPIOB, i, 1);
        			HAL_Delay(75);
        			HAL_GPIO_WritePin(GPIOB, i, 0);
        			HAL_Delay(75);
        		}
        		for (int i = 0; i < 3; ++i) {
        			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 + GPIO_PIN_4 + GPIO_PIN_5 + GPIO_PIN_6, 1);
        			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 0);
        			HAL_Delay(500);
        			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 + GPIO_PIN_4 + GPIO_PIN_5 + GPIO_PIN_6, 0);
        			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, 1);
        			HAL_Delay(500);
        		}
        		placar1 = 0;
        		placar2 = 0;
        	}
            memset(matriz, '_', sizeof(matriz));
            mostrarMatriz(matriz, true, xColor, oColor);
            jogadorRodada = (jogadorRodada == 1) ? 1 : 2;
            posicaoCursor = 0;
            jogadas = 1;
            x = 0;
            y = 0;
        } else if (!vencedor && jogadas > 9) {
            memset(matriz, '_', sizeof(matriz));
            mostrarMatriz(matriz, true, xColor, oColor);
            jogadorRodada = (jogadorRodada == 1) ? 1 : 2;
            posicaoCursor = 0;
            jogadas = 1;
            x = 0;
            y = 0;
        }
      }
    }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV16;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_1LINE;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ST7735_DC_Pin|ST7735_RES_Pin|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);

  /*Configure GPIO pin : ST7735_CS_Pin */
  GPIO_InitStruct.Pin = ST7735_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ST7735_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ST7735_DC_Pin ST7735_RES_Pin PB3 PB4
                           PB5 PB6 PB9 */
  GPIO_InitStruct.Pin = ST7735_DC_Pin|ST7735_RES_Pin|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA9 PA10 PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
