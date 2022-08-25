#include "mbed.h"
#include"PwmOut.h"



PwmOut LED_PWM(MIKOR_PWM);

int main()
{
    float fade;
    fade =0;
    LED_PWM.period(0.01f);//10ms
       while(1)
        {     
            thread_sleep_for(20); // 20 ms
            LED_PWM.write(fade);
            fade=fade+0.01;
            if (fade>1) 
                {
                fade=0;
                LED_PWM.write(fade);
                thread_sleep_for(1000);
                }
         
        }
}
//int main() 
//{
//    LED_PWM.period(0.020);          // servo requires a 20ms period
//    while (1) 
//   {
//        for(float offset=0.0; offset<0.001; offset+=0.0001)
//         {
//            LED_PWM.pulsewidth(0.001 + offset); // servo position determined by a pulsewidth between 1-2ms
//            thread_sleep_for(250);//0.25 s
//        }
//
//   }
//}


