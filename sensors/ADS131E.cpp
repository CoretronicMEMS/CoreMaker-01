/* 
 * MIT License
 *
 * Copyright (c) 2022 CoretronicMEMS
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/


#include "ADS131E.h"
#include "global.h"


// IO pin configuration
#define ADS131E_START_PORT  PB
#define ADS131E_START_PIN   0
#define ADS131E_PWDN_PORT   PF
#define ADS131E_PWDN_PIN    4
#define ADS131E_DRDY_PORT   PF
#define ADS131E_DRDY_PIN    5

// #define ADS131E_START       PB0
// #define ADS131E_PWDN        PF4
// #define ADS131E_DRDY        PF5
// #define ADS131E_DRDY_IRQ    GPF_IRQn
// #define ADS131E_DRDY_ISR    GPF_IRQHandler

#define ADS131E_PID         0xD1

// ADS131E command
#define CMD_WAKEUP          0x02
#define CMD_STANDBY         0x04
#define CMD_RESET           0x06
#define CMD_START           0x08
#define CMD_STOP            0x0A
#define CMD_OFFSETCAL       0x1A
#define CMD_RDATAC          0x10
#define CMD_SDATAC          0x11
#define CMD_RDATA           0x12
#define CMD_RREG            0x20
#define CMD_WREG            0x40

#define REG_ID              0x00
#define REG_CONFIG1         0x01
#define REG_CONFIG2         0x02
#define REG_CONFIG3         0x03
#define REG_FAULT           0x04
#define REG_CH1SET          0x05
#define REG_CH2SET          0x06
#define REG_CH3SET          0x07
#define REG_CH4SET          0x08
#define REG_CH5SET          0x09
#define REG_CH6SET          0x0A
#define REG_CH7SET          0x0B
#define REG_CH8SET          0x0C


namespace CMC {

struct Config_to_Reg
{
    uint32_t config;
    uint8_t reg_value;
};

const static Config_to_Reg ads131e_odr_list[] = 
{
  {  1000, 6},
  {  2000, 5},
  {  4000, 4},
  {  8000, 3},
  { 16000, 2},
  { 32000, 1},
  { 64000, 0},
};

const static Config_to_Reg ads131e_gain_list[] = 
{
  {  1, 1},
  {  2, 2},
  {  4, 4},
  {  8, 5},
  { 12, 6},
};



#define MAX_DEVICE_COUNT    1


ADS131E::ADS131E(SPI *spi, PinName start_pin, PinName pwdn_pin, PinName drdy_pin, int odr):
        m_SPIDev(spi),
        m_ODR(odr),
        START_PIN(start_pin),
        PWDN_PIN(pwdn_pin),
        DRDY_PIN(drdy_pin),
        m_SPI_done("spi_done")
{
    
}

ADS131E::~ADS131E()
{

}

void ADS131E::DRDY_ISR()
{
    //ReadData(m_ADCData, ADS131E_ADC_CHANNELS);
    SetDataReady();
}

void ADS131E::SPI_Done(int val)
{
    m_SPI_done.set(0x01);
}

int32_t ADS131E::Initialize()
{
    DRDY_PIN.fall(callback(this, &ADS131E::DRDY_ISR));

    PWDN_PIN = 1;
    START_PIN = 0;
    ThisThread::sleep_for(std::chrono::milliseconds(1));

    m_SPIDev->frequency(20000000);
    m_SPIDev->format(8, 1);
    m_SPIDev->set_default_write_value(0x00);
    m_SPIDev->set_dma_usage(DMA_USAGE_ALWAYS);

    WriteCommand(CMD_RESET);
    wait_us(10);

    WriteCommand(CMD_SDATAC);
	uint8_t pid = ReadRegisterByte(REG_ID);
    DBG_MSG("Probe ads131e: %02X\n", pid);

    // WriteCommand(CMD_OFFSETCAL);
    // ThisThread::sleep_for(std::chrono::milliseconds(200));

    SetODR(m_ODR);
    WriteRegisterByte(REG_CONFIG3, 0xE0); // Enable internal reference and set to 4V.
	WriteCommand(CMD_RDATAC);

    START_PIN = 1;

    return 0;
}

int32_t ADS131E::Uninitialize()
{
    return 0;
}

int32_t ADS131E::Write(const void *data, size_t num)
{
    return 0;
}

int32_t ADS131E::Read(void *data, size_t num)
{
    ReadData((int32_t*)data, num);

    return 0;
}

int32_t ADS131E::Control(uint32_t control, uint32_t arg)
{
    if(control == SENSOR_CTRL_START)
    {
        START_PIN = 1;
        m_isOn = TRUE;
        WriteCommand(CMD_RDATAC);
    }
    else if(control == SENSOR_CTRL_STOP)
    {
        START_PIN = 0;
        m_isOn = FALSE;
        WriteCommand(CMD_SDATAC);
    }
    else if(control == SENSOR_CTRL_SET_ODR)
    {
        return SetODR(arg);
    }
    else if(control == SENSOR_CTRL_SELFTEST)
    {
        return SelftTest();
    }
    else if(control == SENSOR_CTRL_GET_ODR)
    {
        *((uint32_t*)arg) = m_ODR;
    }
    else if(control == SENSOR_CTRL_SET_GAIN)
    {
        return SetGain(arg);
    }
    else if(control == ADS131E_CTRL_GET_SAMPLE_COUNT)
    {
        *((uint32_t*)arg) = m_intCount;
        m_intCount = 0;
        return 1;
    }
    else if(control == ADS131E_CTRL_SET_MUX)
    {
        
    }

    return 0;
}

int32_t ADS131E::ReadData(int32_t *data, uint32_t num)
{
    char buf[3+3*ADS131E_ADC_CHANNELS], txbuf[3+3*ADS131E_ADC_CHANNELS] = {0};
    int32_t adc[ADS131E_ADC_CHANNELS];
    int adc_byte_count;
    int data_bytes;
    int spi_rx_bytes = 0;
    int spi_tx_bytes = 0;

    if(m_ODR < 32000)
        data_bytes = 3;
    else
        data_bytes = 2;
    adc_byte_count = 3 + ADS131E_ADC_CHANNELS * data_bytes;

    m_SPIDev->select();
    //m_SPIDev->write(NULL, 0, buf, adc_byte_count);
    m_SPIDev->transfer(txbuf, adc_byte_count, buf, adc_byte_count, callback(this, &ADS131E::SPI_Done));
    m_SPI_done.wait_any(0x01);
    m_SPIDev->deselect();

    if(data_bytes == 2)
    {
        for(int i=0; i<ADS131E_ADC_CHANNELS; i++)
            adc[i] = (int16_t)((buf[3+i*2+0]<<8) | buf[3+i*2+1]);
    }
    else
    {
        for(int i=0; i<ADS131E_ADC_CHANNELS; i++)
        {
            adc[i] = (((int32_t)buf[3+i*3+0]<<24) | (buf[3+i*3+1]<<16) | (buf[3+i*3+2]<<8));
            //adc[i] = adc[i]>>8;
            adc[i] = adc[i]>>16;    // truncate to 16 bit
        }
    }

    memcpy(data, adc, sizeof(adc));
    m_intCount++;

    return 1;
}

int32_t ADS131E::ReadRegisters(uint8_t reg_addr, uint8_t num, uint8_t *data)
{
    int spi_rx_bytes = 0;

    m_SPIDev->select();
    m_SPIDev->write(CMD_RREG | (reg_addr & 0x1F));
    wait_us(2);
    m_SPIDev->write((num & 0x1F));
    wait_us(2);
    m_SPIDev->clear_transfer_buffer();

    while(spi_rx_bytes < num)
    {
        data[spi_rx_bytes++] = m_SPIDev->write(0x00);
    }

    m_SPIDev->deselect();

	return spi_rx_bytes;
}

uint8_t ADS131E::ReadRegisterByte(uint8_t reg_addr)
{
    uint8_t reg = 0;
    ReadRegisters(reg_addr, 1, &reg);
    return reg;
}

void ADS131E::WriteCommand(uint8_t cmd)
{
    m_SPIDev->select();
    m_SPIDev->write(cmd);
    wait_us(2);
    m_SPIDev->deselect();
    m_SPIDev->clear_transfer_buffer();
}

int32_t ADS131E::WriteRegisters(uint8_t reg_addr, uint8_t num, uint8_t *data)
{
    int spi_tx_bytes = 0;

    m_SPIDev->select();
    m_SPIDev->write(CMD_WREG | (reg_addr & 0x1F));
    //wait_us(2);
    m_SPIDev->write((num & 0x1F));
    //wait_us(2);

    while( spi_tx_bytes < num )
    {
        m_SPIDev->write(data[spi_tx_bytes++]);
        //wait_us(2);
    }

    m_SPIDev->deselect();
    m_SPIDev->clear_transfer_buffer();

	return spi_tx_bytes;
}


int32_t ADS131E::WriteRegisterByte(uint8_t reg_addr, uint8_t data)
{
    WriteRegisters(reg_addr, 1, &data);
    return 1;
}

int32_t ADS131E::SetODR(uint32_t arg)
{
    uint32_t odr = arg;
    uint8_t reg = 0;

    if(odr > ads131e_odr_list[ARRAY_SIZE(ads131e_odr_list)-1].config)
        odr = ads131e_odr_list[ARRAY_SIZE(ads131e_odr_list)-1].config;

    for(uint32_t k=0; k<ARRAY_SIZE(ads131e_odr_list); k++)
    {
        if( ads131e_odr_list[k].config >= odr )
        {
            reg = ads131e_odr_list[k].reg_value;
            odr = ads131e_odr_list[k].config;
            m_ODR = odr;
            break;
        }
    }

    WriteRegisterByte(REG_CONFIG1, 0x90 | (reg & 0x7) );
    return odr;
}

int32_t ADS131E::SetGain(uint32_t arg)
{
    int ch_mask = arg >> 16;
    uint32_t gain = arg && 0xFFFF;
    uint8_t reg = 0;

    if(gain > ads131e_gain_list[ARRAY_SIZE(ads131e_gain_list)-1].config)
        gain = ads131e_gain_list[ARRAY_SIZE(ads131e_gain_list)-1].config;

    for(uint32_t k=0; k<ARRAY_SIZE(ads131e_gain_list); k++)
    {
        if( ads131e_gain_list[k].config >= gain )
        {
            reg = ads131e_gain_list[k].reg_value;
            gain = ads131e_gain_list[k].config;
            m_gain = gain;
            break;
        }
    }

    for(int i=0; i<ADS131E_ADC_CHANNELS; i++)
    {
        if(GET_BITS(ch_mask, 0x01, i))
            WriteRegisterByte(REG_CH1SET+i, reg);
    }
    return gain;
}

int32_t ADS131E::SelftTest()
{
    START_PIN = 0;
    DRDY_PIN.disable_irq();

    WriteRegisterByte(REG_CONFIG1, 0x91);
    WriteRegisterByte(REG_CONFIG3, 0xE0);
    for(int i=0; i<ADS131E_ADC_CHANNELS; i++)
        WriteRegisterByte(REG_CH1SET+i, 0x11);

    START_PIN = 1;
    wait_us(10);
    START_PIN = 0;
    ThisThread::sleep_for(std::chrono::milliseconds(200));

    int32_t adc[6];
    ReadData(adc, 6*2);
    printf("%ld, %ld, %ld, %ld, %ld, %ld\n", adc[0], adc[1], adc[2], adc[3], adc[4], adc[5]);

    SetGain((0x3F<<16) | (m_gain & 0xFFFF));
    SetODR(m_ODR);

    DRDY_PIN.enable_irq();
    START_PIN = 1;

    return 1;
}

}; //namespace CMC


