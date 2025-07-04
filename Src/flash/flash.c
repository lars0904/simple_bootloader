#include "../flash/flash.h"

#include "stm32f3xx.h"

Error_code Erase_page(uint32_t address)
{
    if(address < FLASH_BASE)
    {
        return ERROR_INVALID_PARAMETER;
    }

    if(FLASH->SR & FLASH_SR_BSY)
    {
        return ERROR_PERIPHERAL_FAULT;
    }

    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = address;
    FLASH->CR |= FLASH_CR_STRT;

    while (FLASH->SR & FLASH_SR_BSY);

    FLASH->CR &= ~FLASH_CR_PER;

    if(FLASH->SR & FLASH_SR_EOP)
    {
        FLASH->SR |= FLASH_SR_EOP;
    }
    
    else 
    {
        return ERROR_PERIPHERAL_FAULT;
    }

    FLASH->CR |= FLASH_CR_PG;

    return NO_ERROR;
}

void Unlock_flash()
{
    if (FLASH->CR & FLASH_CR_LOCK)
    {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
    FLASH->CR |= FLASH_CR_PG;
}

void Lock_flash()
{
    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->CR |= FLASH_CR_LOCK;
}

Error_code Write_flash(void* data, size_t dataSize, uint32_t address)
{   
    if(data == NULL || dataSize < 1 || (dataSize % 2) != 0, address < FLASH_BASE || (address % 2048) != 0 )
    {
        return ERROR_INVALID_PARAMETER;
    }
    
    Error_code error = NO_ERROR;

    uint16_t* src = (uint16_t*)data;
    uint16_t* dst = (uint16_t*)address;

    Unlock_flash();

    dataSize = dataSize / 2;
    
    for(uint32_t i = 0; i < dataSize; i++) 
    {
        if((i + address) % (2048 / 2) == 0)
        {
            error = Erase_page(address + (i*2));
            if(error) return error;
        }
        dst[i] = src[i];
        while (FLASH->SR & FLASH_SR_BSY);
    }

    Lock_flash();

    return NO_ERROR;
}