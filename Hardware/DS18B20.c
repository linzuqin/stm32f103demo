#include "ds18b20.h"
#include "sys.h"  // ��Ҫʵ��΢�뼶��ʱ����
float temperature;
struct rt_thread ds18b20_pannel;
#define ds18b20_pannel_size 1024
uint8_t ds18b20_pannel_stack[ds18b20_pannel_size];

// ����������Ϊ���ģʽ
static void DS18B20_SetOutput(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

// ����������Ϊ����ģʽ
static void DS18B20_SetInput(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

// ��ʼ������
void DS18B20_Init(void)
{
    RCC_APB2PeriphClockCmd(DS18B20_RCC, ENABLE);
    DS18B20_SetOutput();
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN); // ��������
}

// ��λDS18B20
void DS18B20_Reset(void)
{
    DS18B20_SetOutput();
    
    // ��������480us
    GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
    rt_hw_us_delay(480);
    
    // �ͷ�����
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
    rt_hw_us_delay(60);
    
    // �ȴ�DS18B20��Ӧ
    DS18B20_SetInput();
    while(GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN));
    while(!GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN));
    
    // �ȴ���λ����
    rt_hw_us_delay(420);
}

// д��һ���ֽ�
void DS18B20_WriteByte(uint8_t data)
{
    uint8_t i;
    DS18B20_SetOutput();
    
    for(i = 0; i < 8; i++)
    {
        // �������߿�ʼдʱ��
        GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
        
        if(data & 0x01) // д1
        {
            rt_hw_us_delay(2);
            GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
            rt_hw_us_delay(60);
        }
        else // д0
        {
            rt_hw_us_delay(60);
            GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
            rt_hw_us_delay(2);
        }
        
        data >>= 1;
    }
}

// ��ȡһ���ֽ�
uint8_t DS18B20_ReadByte(void)
{
    uint8_t i, data = 0;
    
    for(i = 0; i < 8; i++)
    {
        DS18B20_SetOutput();
        GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
        rt_hw_us_delay(2);
        
        DS18B20_SetInput();
        rt_hw_us_delay(8);
        
        if(GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN))
        {
            data |= (1 << i);
        }
        
        rt_hw_us_delay(50);
        DS18B20_SetOutput();
        GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
    }
    
    return data;
}

// ��ʼ�¶�ת��
void DS18B20_StartConversion(void)
{
    DS18B20_Reset();
    DS18B20_WriteByte(0xCC); // ����ROM
    DS18B20_WriteByte(0x44); // �����¶�ת��
}

// ��ȡ�¶�ֵ(���϶�)
float DS18B20_ReadTemperature(void)
{
    uint8_t temp_l, temp_h;
    int16_t temp;
    float temperature;
    
    DS18B20_StartConversion();
    
    // �ȴ�ת�����(���750ms)
    DS18B20_SetInput();
    while(!GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN));
    
    DS18B20_Reset();
    DS18B20_WriteByte(0xCC); // ����ROM
    DS18B20_WriteByte(0xBE); // ��ȡ�ݴ���
    
    temp_l = DS18B20_ReadByte(); // LSB
    temp_h = DS18B20_ReadByte(); // MSB
    
    temp = (temp_h << 8) | temp_l;
    temperature = temp * 0.0625f; // 12λ�ֱ���
    
    return temperature;
}

void ds18b20_pannel_entry(void *params)
{
    while (1)
    {
        temperature = DS18B20_ReadTemperature();
        rt_thread_mdelay(1000); // ÿ���ȡһ��
    }
}

int ds18b20_start(void)
{
	DS18B20_Init();
	rt_thread_init(&ds18b20_pannel , "ds18b20_pannel", 
        ds18b20_pannel_entry, 
        RT_NULL, ds18b20_pannel_stack, ds18b20_pannel_size, 
        10, 100);
	
	rt_thread_startup(&ds18b20_pannel);
	return 1;
}

//INIT_APP_EXPORT(ds18b20_start);
