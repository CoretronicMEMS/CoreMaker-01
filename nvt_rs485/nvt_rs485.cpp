#include "nvt_rs485.h"
#include "PeripheralPins.h"
#if MBED_MAJOR_VERSION >= 6
NvtRS485::NvtRS485(PinName tx, PinName rx, PinName dir)
    : MyUnbufferedSerial(tx, rx)
#else
NvtRS485::NvtRS485(PinName tx, PinName rx, PinName dir)
    : Serial(tx, rx)
#endif
{
    this->set_rs485_mode(dir);
}

NvtRS485::~NvtRS485()
{
    
}

int NvtRS485::set_rs485_mode(PinName dir)
{
    int ret=0;
    serial_t *obj = &_serial;  // Get pointer from grandfather-SerialBase.

    lock();
    
    //Implement RS485-AUD mode for various platform of Nuvoton
#if defined(TARGET_NUMAKER_PFM_NUC472) || defined(TARGET_NUMAKER_PFM_M453) || defined(TARGET_NUMAKER_PFM_M487) || defined(TARGET_NUMAKER_IOT_M487)
    /*
    Program Sequence example:
     1. Program FUNCSEL in UART_FUNCSEL to select RS-485 function.
     2. Program the RXOFF bit (UART_FIFO[8]) to determine enable or disable RS-485 receiver
     3. Program the RS-485_NMM or RS-485_AAD mode.
     4. If the RS-485_AAD mode is selected, the ADDRMV (UART_ALTCTL[31:24]) is programmed for auto address match value.
     5. Determine auto direction control by programming RS-485_AUD.
    */

    UART_T *uart_base = (UART_T *) NU_MODBASE(obj->serial.uart);

    // First, disable flow control completely.
    uart_base->INTEN &= ~(UART_INTEN_ATORTSEN_Msk | UART_INTEN_ATOCTSEN_Msk);

    // Check if RTS pin matches.
    uint32_t uart_rts = pinmap_peripheral(dir, PinMap_UART_RTS);
    MBED_ASSERT(uart_rts == obj->serial.uart);

    // Enable the pin for RTS function
    pinmap_pinout(dir, PinMap_UART_RTS);

    uart_base->FUNCSEL = (uart_base->FUNCSEL & ~UART_FUNCSEL_FUNCSEL_Msk) | 0x3;
    uart_base->ALTCTL  = (uart_base->ALTCTL & ~UART_ALTCTL_RS485AUD_Msk) | UART_ALTCTL_RS485AUD_Msk;
    uart_base->MODEM = (uart_base->MODEM & ~UART_MODEM_RTSACTLV_Msk);
    uart_base->FIFO = (uart_base->FIFO & ~UART_FIFO_RTSTRGLV_Msk) | (0x2 << UART_FIFO_RTSTRGLV_Pos);
 
#else
    #error "Unknown platform."
#endif

    unlock();    
    return ret;
}