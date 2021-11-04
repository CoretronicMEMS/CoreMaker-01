
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define GET_BITS(reg, msk, pos)             (((reg) >> pos) & msk)
#define SET_BITS(reg, msk, val)             (reg) = (((reg) & ~(msk)) | (val))
#define MCUREG_GET_FIELD(reg, bitname)      (((reg) & bitname##_Msk) >> bitname##_Pos)
#define MCUREG_SET_FIELD(reg, bitname, val) (reg) = (((reg) & ~bitname##_Msk) | (((val)<<bitname##_Pos)&bitname##_Msk))

#define PINMUX_SET(reg, port, val)  SET_BITS(SYS->reg, SYS_##reg##_##port##MFP_Msk, SYS_##reg##_##port##MFP_##val)

#define ARRAY_SIZE(arr)     (sizeof(arr) / sizeof(arr[0]))

#endif
