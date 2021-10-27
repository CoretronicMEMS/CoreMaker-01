#ifndef NVTRS485_H
#define NVTRS485_H
 
#include "mbed.h"
#if MBED_MAJOR_VERSION >= 6
#include "MyUnbufferedSerial.h"
class NvtRS485 : public MyUnbufferedSerial {
#else
class NvtRS485 : public Serial {
#endif
public:
    NvtRS485(PinName tx, PinName rx, PinName dir);
    virtual ~NvtRS485(void);
    int set_rs485_mode(PinName dir);
};

#endif //NVTRS485_H