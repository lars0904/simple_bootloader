#ifndef FLASH_H
#define FLASH_H

#include "stdint.h"
#include "stddef.h"

#include "error_code.h"

/** 
    @brief
        Writes data into flash

    @param data Pointer to the data buffer
    @param dataSize Size of the data buffer, needs to be a multiple of 2
    @param address Address of the flash memory, address needs to be page aligned (2 KiB)
**/
Error_code Write_flash(void* data, size_t dataSize, uint32_t address);

#endif