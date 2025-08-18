#include "sys.h"

int main(void)
{
	/*Hardware Init*/
	 
	/*User TASK Init*/
	
	//IWDG_Init(IWDG_Prescaler_64, 0x0FFF);

	while (1)
	{
		//IWDG_Feed();

		rt_thread_mdelay(5000);
	}
}

