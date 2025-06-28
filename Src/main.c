#include "main.h"

#define BOOTLOADER_SIZE 0x8000u
#define FIRMWARE_ADDRESS FLASH_BASE + BOOTLOADER_SIZE

void button_setup()
{
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  GPIOC->PUPDR |= 0b01 << GPIO_PUPDR_PUPDR13_Pos; //pull-up 
}

void system_setup()
{
  SystemClock_Config();
  button_setup();
}

void jump_to_firmware()
{
  uint32_t firmware_stack = *(volatile uint32_t*)FIRMWARE_ADDRESS;
  uint32_t firmware_reset_handler = *(volatile uint32_t*)(FIRMWARE_ADDRESS + 4); 
  
  typedef void (*pFunction)(void);
  pFunction app_entry = (pFunction)firmware_reset_handler;

  SCB->VTOR = FIRMWARE_ADDRESS;

  LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_ALL);
  LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_ALL);
  LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_ALL);
  
  __set_MSP(firmware_stack);

  app_entry();
}

int main(void)
{
  system_setup();

  while (1)
  {
    if(GPIOC->IDR & GPIO_IDR_13)
    {
    }

    else
    {
      jump_to_firmware();
    }
  }
}

void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {

  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_9, LL_RCC_PREDIV_DIV_1);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(72000000);
  LL_SetSystemCoreClock(72000000);
  LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);
}

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
