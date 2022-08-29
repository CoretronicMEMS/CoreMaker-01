#include "mbed.h"
I2C i2c_aht10( MIKOR_SDA, MIKOR_SCL);
const int addres = 0x70;
char data[3]={0xE1, 0x08, 0x00};
char data1[3]={0xAC, 0x33, 0x00};
char data2[6];
int main()
{
//start sensor    
  
    i2c_aht10.write(addres , data, 3);
    

    thread_sleep_for(500);
    printf("\033[r\033[2J\033[?25l"); 
    fflush(stdout);
    while(1){
        //measure

        i2c_aht10.write(addres, data1, 3);
        
        thread_sleep_for(100);
        //read measure
    
        i2c_aht10.read(addres, data2, 6);

        //calculator
        
        float hum = (float(((((data2[1] << 16) | (data2[2] << 8) | data2[3]) >> 4) / 1048576.0) *100.0));

        float tmp =(float((((((data2[3] & 0x0F) << 16) | (data2[4] << 8) | data2[5]) *200) /1048576.0) - 50.0));
        float all[]={hum,tmp};


        //print
        printf("\r\033[temperature:%.2f deg \r\n",tmp);
        fflush(stdout);
        printf("\r\033[humidity:%.2f %% \r\n",hum);
        fflush(stdout);
        thread_sleep_for(100);
        printf("\033[%uA", sizeof(all)); 
        fflush(stdout);
    }

}
