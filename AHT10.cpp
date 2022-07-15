#include "mbed.h"
I2C i2c_aht10( MIKOR_SDA, MIKOR_SCL);
const int addres = 0x70;
char data[3]={0xE1, 0x08, 0x00};
char data1[3]={0xAC, 0x33, 0x00};
char data2[6];
int main()
{
//start snsor    
  
    i2c_aht10.write(addres , data, 3);
    

    thread_sleep_for(500);

    while(1){
        //measure

        i2c_aht10.write(addres, data1, 3);
        
        thread_sleep_for(100);
        //read measure
    
        i2c_aht10.read(addres, data2, 6);

        //calculator

        float hum = (float(((((data2[1] << 16) | (data2[2] << 8) | data2[3]) >> 4) / 1048576.0) *100.0));

        float tmp =(float((((((data2[3] & 0x0F) << 16) | (data2[4] << 8) | data2[5]) *200) /1048576.0) - 50.0));

        //print
        printf("sensor_data:%.2f degc,  %.2f %%\n",tmp,hum);
        thread_sleep_for(500);

    }

}