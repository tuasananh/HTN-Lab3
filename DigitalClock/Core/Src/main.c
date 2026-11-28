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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "ILI9341_GFX.h"
#include "ILI9341_STM32_Driver.h"
#include "fonts.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CLOCK_RUN_MS       10000U
#define CLOCK_SLEEP_MS     10000U
#define CLOCK_POLL_MS      20U
#define CLOCK_TEXT_X       12U
#define CLOCK_WEEKDAY_Y    40U
#define CLOCK_DATE_Y       112U
#define CLOCK_TIME_Y       176U
#define CLOCK_FONT_SCALE   2U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi5;

/* USER CODE BEGIN PV */
static volatile uint32_t lastActivityTick;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI5_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void Clock_SetInitialDateTime(void)
{
  RTC_DateTypeDef date = {0};
  RTC_TimeTypeDef time = {0};

  /* RTC_FORMAT_BIN requires decimal month 11, not RTC_MONTH_NOVEMBER (0x11). */
  date.Year = 26;
  date.Month = 11;
  date.Date = 6;
  date.WeekDay = RTC_WEEKDAY_FRIDAY;
  if (HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  time.Hours = 7;
  time.Minutes = 37;
  time.Seconds = 16;
  time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  time.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
}

static void Clock_DrawDate(const RTC_DateTypeDef *date)
{
  static const char * const weekdays[] = {
    "Monday", "Tuesday", "Wednesday", "Thursday",
    "Friday", "Saturday", "Sunday"
  };
  char text[11];

  if ((date->WeekDay < RTC_WEEKDAY_MONDAY) || (date->WeekDay > RTC_WEEKDAY_SUNDAY))
  {
    Error_Handler();
    return;
  }

  int length = snprintf(text, sizeof(text), "%u/%u/%04u",
                        (unsigned int)date->Date,
                        (unsigned int)date->Month,
                        2000U + (unsigned int)date->Year);
  if ((length < 0) || ((size_t)length >= sizeof(text)))
  {
    Error_Handler();
    return;
  }

  uint16_t lineHeight = (uint16_t)FONT4[2] * CLOCK_FONT_SCALE;
  ILI9341_DrawRectangle(0, CLOCK_WEEKDAY_Y - 8U, LCD_WIDTH, lineHeight + 16U, LIGHTGREY);
  ILI9341_DrawTextScaled(weekdays[date->WeekDay - RTC_WEEKDAY_MONDAY], FONT4,
                         70, CLOCK_WEEKDAY_Y, BLACK, LIGHTGREY, CLOCK_FONT_SCALE);
  ILI9341_DrawRectangle(0, CLOCK_DATE_Y, LCD_WIDTH, lineHeight, BLACK);
  ILI9341_DrawTextScaled(text, FONT4, 40, CLOCK_DATE_Y,
                         WHITE, BLACK, CLOCK_FONT_SCALE);
}

static void Clock_DrawTime(const RTC_TimeTypeDef *time, char previousText[9], uint8_t fullRedraw)
{
  char text[9];
  int length = snprintf(text, sizeof(text), "%02u:%02u:%02u",
                        (unsigned int)time->Hours,
                        (unsigned int)time->Minutes,
                        (unsigned int)time->Seconds);
  if ((length < 0) || ((size_t)length >= sizeof(text)))
  {
    Error_Handler();
    return;
  }

  uint16_t x = 55;
  for (uint8_t i = 0; i < 8; i++)
  {
    if (fullRedraw || text[i] != previousText[i])
    {
      ILI9341_DrawCharScaled(text[i], FONT4, x, CLOCK_TIME_Y,
                             WHITE, BLACK, CLOCK_FONT_SCALE);
      previousText[i] = text[i];
    }
    x += ILI9341_GetCharAdvance(text[i], FONT4, CLOCK_FONT_SCALE);
  }
  previousText[8] = '\0';
}

static void Clock_UpdateDisplay(uint8_t fullRedraw)
{
  static RTC_DateTypeDef previousDate;
  static RTC_TimeTypeDef previousTime;
  static char previousText[9];
  RTC_TimeTypeDef time = {0};
  RTC_DateTypeDef date = {0};

  /* Reading the date releases the RTC shadow registers after a time read. */
  if (HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK ||
      HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  if (fullRedraw || date.Date != previousDate.Date || date.Month != previousDate.Month ||
      date.Year != previousDate.Year || date.WeekDay != previousDate.WeekDay)
  {
    Clock_DrawDate(&date);
    previousDate = date;
  }
  if (fullRedraw || time.Hours != previousTime.Hours ||
      time.Minutes != previousTime.Minutes || time.Seconds != previousTime.Seconds)
  {
    Clock_DrawTime(&time, previousText, fullRedraw);
    previousTime = time;
  }
}

static void Clock_TryStandby(uint32_t activityTick)
{
  ILI9341_DrawRectangle(0, 0, LCD_WIDTH, LCD_HEIGHT, BLACK);
  ILI9341_DrawTextScaled("Standby..", FONT4, CLOCK_TEXT_X, CLOCK_DATE_Y,
                         WHITE, BLACK, CLOCK_FONT_SCALE);
  ILI9341_DrawText("Press PA0 to wake up", FONT3, CLOCK_TEXT_X, CLOCK_TIME_Y, WHITE, BLACK);

  /* A press during the SPI transfer or while arming WKUP must still count. */
  __disable_irq();
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
  if (lastActivityTick == activityTick && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET &&
      __HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) == RESET)
  {
    HAL_SuspendTick();
    SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
    __DSB();
    HAL_PWR_EnterSTANDBYMode();
    /* A pending interrupt can prevent WFI from entering Standby. */
    CLEAR_BIT(SCB->SCR, SCB_SCR_SLEEPDEEP_Msk);
    HAL_ResumeTick();
  }
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  __enable_irq();
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
  MX_SPI5_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  if (__HAL_RTC_IS_CALENDAR_INITIALIZED(&hrtc) == 0U ||
      (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) && !__HAL_PWR_GET_FLAG(PWR_FLAG_SB)))
  {
    Clock_SetInitialDateTime();
  }
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
  __HAL_RCC_CLEAR_RESET_FLAGS();

  ILI9341_Init();
  ILI9341_DrawRectangle(0, 0, LCD_WIDTH, LCD_HEIGHT, BLACK);
  Clock_UpdateDisplay(1);
  lastActivityTick = HAL_GetTick();
  uint32_t lastPoll = lastActivityTick;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    uint32_t activityTick = lastActivityTick;
    uint32_t now = HAL_GetTick();
    uint32_t idle = now - activityTick;

    if (idle >= CLOCK_RUN_MS + CLOCK_SLEEP_MS && HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
    {
      Clock_TryStandby(activityTick);
      /* A successful Standby wake restarts main. A cancelled entry returns here. */
      ILI9341_DrawRectangle(0, 0, LCD_WIDTH, LCD_HEIGHT, BLACK);
      Clock_UpdateDisplay(1);
      lastPoll = HAL_GetTick();
      continue;
    }

    if (now - lastPoll >= CLOCK_POLL_MS)
    {
      Clock_UpdateDisplay(0);
      lastPoll = now;
    }

    if (idle >= CLOCK_RUN_MS)
    {
      HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    }
    else
    {
      HAL_Delay(CLOCK_POLL_MS);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
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
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 249;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER button on PA0/WKUP */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
  if (pin == GPIO_PIN_0)
  {
    lastActivityTick = HAL_GetTick();
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
