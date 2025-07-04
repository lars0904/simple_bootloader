#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "firmware.h"
#include "flash/flash.h"

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

void system_reset()
{
  LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_ALL);
  LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_ALL);
  LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_ALL);

  LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_ALL);
  LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_ALL);
  LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_ALL);
}

void jump_to_firmware()
{
  __disable_irq();

  uint32_t firmware_stack = *(volatile uint32_t*)FIRMWARE_ADDRESS;
  uint32_t firmware_reset_handler = *(volatile uint32_t*)(FIRMWARE_ADDRESS + 4); 
  
  typedef void (*pFunction)(void);
  pFunction app_entry = (pFunction)firmware_reset_handler;

  system_reset();
  __enable_irq();

  SCB->VTOR = FIRMWARE_ADDRESS;
  __set_MSP(firmware_stack);

  app_entry();
}

int main(void)
{
  Error_code error = NO_ERROR;

  system_setup();
  
  while (1)
  {
    if((GPIOC->IDR & GPIO_IDR_13) == 0)
    {
      error = Write_flash((void*)blink_test, 11808, FIRMWARE_ADDRESS);

      if(error == NO_ERROR)
      {
        jump_to_firmware();
      }
    }
  }
}
