/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define IS_SET(byte,bit)    (((byte) &  (1 << (bit))) >> (bit))
#define SET_BIT(byte, bit)  ((byte)  |= (1 << (bit)))
#define CLEAR_BIT(byte,bit) ((byte)  &= ~(1 << (bit)))
#define DATA_SAMPLE 32

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif


uint8_t testValue=0, interValue=0;
uint8_t receiving = 0, databyte = 0, connectionCount = 0;
volatile uint32_t samples = 0;
volatile uint8_t  samplesReady = 0;
volatile int8_t   sampleCount = DATA_SAMPLE - 1;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void delay_us(uint16_t us);
uint8_t DiffManchester_sendByte( uint8_t byte, uint8_t lastEnd);
uint32_t getSample(void);
uint8_t  interpretSample(uint32_t samps);
uint8_t  DiffManchester_GetData( uint8_t **data, uint8_t dataLens );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM7_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  uint8_t databyte = 0xE9;
  uint8_t lastEnd = 1;
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, GPIO_PIN_RESET);
  //samplesReady = 1;
  //HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim6);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
    /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	//HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_0);
	//delay_us(500);
	//HAL_Delay(1000);
	//lastEnd = DiffManchester_sendByte( databyte, lastEnd);
	//if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)) testValue=1;
	//else                                    testValue=0;
	while( 0 == samplesReady );

	uint8_t data[2]; uint8_t dataLens = 25;
	DiffManchester_GetData( &data, dataLens );
	//printf("%x\r\n", data[0]);
	//lastEnd = DiffManchester_sendByte( *data, lastEnd);
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

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 83;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 125;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 83;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 65535;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PE0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
void delay_us(uint16_t us)
{
	uint16_t differ=0xffff-us-5;


	HAL_TIM_Base_Start(&htim7);
	__HAL_TIM_SetCounter(&htim7,differ);
	while(differ < 0xffff-5)
	{
		differ = __HAL_TIM_GetCounter(&htim7);
	}
	HAL_TIM_Base_Stop(&htim7);
}

uint8_t DiffManchester_sendByte( uint8_t byte, uint8_t lastEnd)
{
    int8_t bit = 7;

    for (; bit >= 0; --bit)
    {
      if ( IS_SET(byte,bit) )
        {
            if( 1 == lastEnd )
            {
              //no jump, 10
              lastEnd = 0;

              delay_us(250);//while(TCNT0 < HALF_TIME);
              HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);

              delay_us(250);//while(TCNT0 < HALF_TIME);
              HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
             }
            else
            {
              //no jump, 01
              lastEnd = 1;

              delay_us(250);//while(TCNT0 < HALF_TIME);
              HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);

              delay_us(250);//while(TCNT0 < HALF_TIME);
              HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
            }
        }
        else
        {
	   if( 1 == lastEnd )
		{
		  //jump, 01
		  lastEnd = 1;

		  delay_us(250);//while(TCNT0 < HALF_TIME);
		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);

		  delay_us(250);//while(TCNT0 < HALF_TIME);
		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);
		 }
		else
		{
		  //no jump, 10
		  delay_us(250);//while(TCNT0 < HALF_TIME);
		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_SET);

		  delay_us(250);//while(TCNT0 < HALF_TIME);
		  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_0, GPIO_PIN_RESET);
		}
	  }
     }

   return lastEnd;
}

uint32_t getSample(void)
{
    /* Reset flag */
    samplesReady = 0;

    return samples;
}
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart2, (uint8_t*)&ch,1,HAL_MAX_DELAY);
    return ch;
}

uint8_t DiffManchester_GetData( uint8_t **data, uint8_t dataLens )
{
  uint8_t  error=1;
  uint8_t  byte[25];
  uint32_t rawSamps;
  if( samplesReady ) //receiving
  {
	error=0;
	for( uint8_t i=0; i<dataLens; i++ )
	{
	  while( 0 == samplesReady );//need to wait in here
	  {
		rawSamps = getSample();
		if( interpretSample( rawSamps ))
		{
		  byte[i] = databyte;
		  //printf("%x\r\n", databyte);
		}
		else
		{
		  error=1; break;
		}
	   }
	 }
  }

  //HAL_TIM_Base_Stop_IT(&htim6);
  //clear the buffer and ready for next run
  HAL_Delay(1);
  receiving = 0;
  sampleCount = DATA_SAMPLE - 1;
  rawSamps = getSample();

  if( 1 == error )
  {
	printf("error \r\n");
  }
  {
	for( uint8_t j=0;j<dataLens; j++ )
	{
	  printf("%x ", byte[j]);
	}
	printf("\r\n");
  }

  return error;
/*

  uint8_t error=0;
  if( samplesReady ) //receiving
  {
	uint8_t  byte[2];
	uint8_t  i=0;
	uint32_t rawSamps = getSample();

	//HAL_TIM_Base_Stop_IT(&htim6);
	//the DiffManchester beginning/first bit
	uint32_t  bit = (rawSamps >> (30));
    bit = bit & 0x0F;
    if (bit == 0x00) bit = 0x11;
    //else if (bit == 0x00) bit = 0x03;
    //else
    //	return 0 ;

    uint32_t reSamps = (rawSamps >> (2)) | (bit << (30));
    uint32_t lastEnd;
    if( interpretSample( reSamps ) && 0 == error )
	{
    	byte[i] = databyte;

    	for( i=1; i<dataLens; i++ )
		{
		  //if (receiving ) //need to wait in here
		  {
			while( 0 == samplesReady );
			lastEnd = rawSamps & 0x00000003;
			rawSamps = getSample();
			reSamps  = (rawSamps >> (2)) | (lastEnd << (30));
			if( interpretSample( reSamps ))
			{
			  byte[i] = databyte;
			  //printf("%x\r\n", databyte);
			}
			else
			{
				error=1; break;
			}
		  }
		 }
	}
    else
    {
    	error=1;
    }

    HAL_TIM_Base_Stop_IT(&htim6);
    receiving = 0;
    if( 1 == error ) printf("error \r\n");
    else             printf("%x, %x\r\n", byte[0], byte[1]);
  }

  return error;
  */
}

uint8_t interpretSample(uint32_t samps)
{
    int8_t i = 7;
	uint8_t bit;

    for (; i >= 0; --i)
    {
        /* Grab the current bit */
        bit = (samps >> (i*4)) & 0x0F;

        if ((bit == 0x03) || ( (bit == 0x0c) ))   CLEAR_BIT(databyte,i);
        else if ((bit == 0x00) || (bit == 0x0F) ) SET_BIT  (databyte,i);
        else return 0;
    }
    //need to check the mid Fliping
    return 1;
}

//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim6.Instance == htim->Instance)
	{
		//if((TIM6->SR & TIM_FLAG_UPDATE) != RESET)
		if( 1 == receiving )
		{
			if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5))
			{
			  //TCNT0 = 1; //Writing to the TCNT0 Register blocks (removes) the Compare Match on the following timer clock.
			  SET_BIT(samples,sampleCount);
			}
			else
			{
			  //TCNT0 = 1;//Writing to the TCNT0 Register blocks (removes) the Compare Match on the following timer clock.
			  CLEAR_BIT(samples,sampleCount);
			}

			//If the bit is finished, set the samplesReady flag /
			if (! sampleCount--)
			{
			  samplesReady = 1;
			  //sampleCount = TIM6->CNT;
			  sampleCount = DATA_SAMPLE - 1;
			  //receiving = 0;
			}
			/* clean flag */
			//TIM6->SR  = ~ TIM_FLAG_UPDATE;
			//TIM6->CNT = 0;
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == GPIO_PIN_5)
  {
	  //if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)) interValue=1;
	  //else                                    interValue=0;

	  //if( 0 == receiving )
	  {
	     receiving = 1;
	     //HAL_TIM_Base_Start_IT(&htim6); // time not sync
	  }
	  TIM6->CNT = 65;
   }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
