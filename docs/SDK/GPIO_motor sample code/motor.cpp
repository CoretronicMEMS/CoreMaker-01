#include "mbed.h"
#include "PinNames.h"
/*
4096 = 360
2048 = 180 
1024 = 90 
*/
BusOut motor_out(MIKOR_MOSI,MIKOR_MISO,MIKOR_SCK,MIKOR_SS);
int step = 0; 
int dir = 1; // direction 順
int dd = 0;  // direction 逆
int main()
{
    for (size_t i = 1; i <=1024; i++) // i 為變數(角度)
    
    
    {   printf("第 %d 次\n",i);
        switch(step)
        {
            case 0: motor_out = 0x1; break;  // 0001
            case 1: motor_out = 0x3; break;  // 0011
            case 2: motor_out = 0x2; break;  // 0010   
            case 3: motor_out = 0x6; break;  // 0110
            case 4: motor_out = 0x4; break;  // 0100
            case 5: motor_out = 0xC; break;  // 1100
            case 6: motor_out = 0x8; break;  // 1000
            case 7: motor_out = 0x9; break;  // 1001
            

            default: motor_out = 0x0; break; // 0000
        }
  
        if(dir) step++; 
        else step--; 
        if(step>7)step=0; 
        if(step<0)step=7; 
        thread_sleep_for(15);  // speed
    }

thread_sleep_for(1000);

    for (size_t i = 1; i <=2048; i++)
    
    
    {   printf("第 %d 次\n",i);
        switch(step)
        {
            case 0: motor_out = 0x1; break;  // 0001
            case 1: motor_out = 0x3; break;  // 0011
            case 2: motor_out = 0x2; break;  // 0010   
            case 3: motor_out = 0x6; break;  // 0110
            case 4: motor_out = 0x4; break;  // 0100
            case 5: motor_out = 0xC; break;  // 1100
            case 6: motor_out = 0x8; break;  // 1000
            case 7: motor_out = 0x9; break;  // 1001
            

            default: motor_out = 0x0; break; // 0000
        }
  
        if(dd) step++; 
        else step--; 
        if(step>7)step=0; 
        if(step<0)step=7; 
        thread_sleep_for(15);  // speed
    }





    
}