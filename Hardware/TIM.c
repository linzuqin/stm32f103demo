#include "TIM.h"
#include "sys.h"
extern struct rt_mailbox mb;
int TIM3_1s_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 9999; // 10,000 - 1
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; // 72,000,000 / (7199 + 1) = 10,000 Hz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3, ENABLE);
	
	return 1;
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		static uint8_t at_disconnect_time = 0;
		if(AT_Device.status < AT_IDLE)
		{
			if(at_disconnect_time ++ > 60)
			{
				memset(AT_Device.wifi_params.WiFi_Password , 0 , strlen(AT_Device.wifi_params.WiFi_Password));
				memcpy(AT_Device.wifi_params.WiFi_Password , DEFAULT_WIFI_PWD , strlen(DEFAULT_WIFI_PWD));
				
				memset(AT_Device.wifi_params.WiFi_SSID , 0 , strlen(AT_Device.wifi_params.WiFi_SSID));
				memcpy(AT_Device.wifi_params.WiFi_SSID , DEFAULT_WIFI_SSID , strlen(DEFAULT_WIFI_SSID));
				
				rt_mb_send(&mb , (rt_ubase_t)"wifi");
//				flash_set("esp_params", &AT_Device.wifi_params, sizeof(AT_Device.wifi_params));
			}
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}