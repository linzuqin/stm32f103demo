/*本文件为场景模式i运行函数 默认支持两个场景 三种状态 每个状态支持自定义继电器输出状态 与 指令下发  默认不保存*/
/*2025 7.30 LZQ*/

#include "condition.h"
custom_cmd_t custom_cmd[CUSTOM_NUM];

void states_callback(uint8_t condition_id , uint8_t state_id)
{
    /*继电器控制*/
    for(uint8_t i = 0;i<IO_NUM;i++)
    {
			condition[condition_id].state[state_id].J_status[i] = my_io[i].output;			//1:输出触发电平
    }

    /*指令下发控制*/
    for(uint8_t i = 0;i< CMD_NUM;i++)
    {
        for(uint8_t j = 0;j< CUSTOM_NUM;j++)
        {
            if(strcmp(condition[condition_id].state[state_id].cmd[i] , custom_cmd[j].name) == 0)	//查找指令表是否有对应指令
            {
                custom_cmd[j].trigger = 1;
            }
        }
    }
}

condition_t condition[CONDITION_NUM] = 
{
    [ARRIVER_CONDITION] = {
        .act_status = 0,                                //初始状态为未触发
        .status = NO_ACT,                               //初始状态为未触发
        .time_period1 = 5,                              //初始默认间隔1为5s
        .time_period2 = 5,                              //初始默认间隔2为5s
        .state = 
        {
            [INIT_STATE_ID] = {
                .J_status = {0},  // 显式初始化数组
                .cmd = {{0}},     // 初始化所有命令字符串
                .cmd_num = 0,
                .state_callback = states_callback,  // 确保赋值
            },
            [STATE1_ID] = {
                .J_status = {0},  // 显式初始化数组
                .cmd = {{0}},     // 初始化所有命令字符串
                .cmd_num = 0,
                .state_callback = states_callback,  // 确保赋值
            },
            [STATE2_ID] = {
                .J_status = {0},  // 显式初始化数组
                .cmd = {{0}},     // 初始化所有命令字符串
                .cmd_num = 0,
                .state_callback = states_callback,  // 确保赋值
            },
        }
    },
    [LEAVE_CONDITION] = {
        .act_status = 0,                                //初始状态为未触发
        .status = NO_ACT,                               //初始状态为未触发
        .time_period1 = 5,                              //初始默认间隔1为5s
        .time_period2 = 5,                              //初始默认间隔2为5s
        .state = 
        {
            [INIT_STATE_ID] = {
                .J_status = {0},  // 显式初始化数组
                .cmd = {{0}},     // 初始化所有命令字符串
                .cmd_num = 0,
                .state_callback = states_callback,  // 确保赋值
            },
            [STATE1_ID] = {
                .J_status = {0},  // 显式初始化数组
                .cmd = {{0}},     // 初始化所有命令字符串
                .cmd_num = 0,
                .state_callback = states_callback,  // 确保赋值
            },
            [STATE2_ID] = {
                .J_status = {0},  // 显式初始化数组
                .cmd = {{0}},     // 初始化所有命令字符串
                .cmd_num = 0,
                .state_callback = states_callback,  // 确保赋值
            },
        }
    },
};


void conditionl_poll(void)
{
    for(uint8_t i = 0;i < CONDITION_NUM;i++)
    {
        switch(condition[i].status)
        {
            case NO_ACT:					//未触发
            if(condition[i].act_status == 1)
            {
                condition[i].status = INIT;
            }

            break;

            case INIT:						//初始状态
            if(condition[i].state[INIT_STATE_ID].state_callback != NULL)
            {
                condition[i].state[INIT_STATE_ID].state_callback(i , INIT_STATE_ID);
								condition[i].status = WAIT_STAGE_1;

            }
            break;

            case WAIT_STAGE_1:		//等待触发状态1

            break;

            case STAGE_1:					//状态1
            if(condition[i].state[STATE1_ID].state_callback != NULL)
            {
                condition[i].state[STATE1_ID].state_callback(i , STATE1_ID);
            }
            condition[i].status = WAIT_STAGE_2;

            break;

            case WAIT_STAGE_2:		//等待触发状态2

            break;

            case STAGE2:					//状态2
            if(condition[i].state[STATE2_ID].state_callback != NULL)
            {
                condition[i].state[STATE2_ID].state_callback(i , STATE2_ID);
            }
            condition[i].status = END;
            break;

            case END:							//结束
            condition[i].act_status = 0;
			condition[i].status = NO_ACT;
            break;
        }
    }
}

void condition_task(void *params)
{
	while(1)
	{
        conditionl_poll();
        rt_thread_mdelay(100);
	}
}

uint8_t condition_stack[1024];
struct rt_thread condition_pannel;

void condition_init(void)
{
    rt_err_t result = RT_EOK;
    result = rt_thread_init(&condition_pannel, "condition", condition_task, RT_NULL, 
                            condition_stack,sizeof(condition_stack) ,  22, 10);

    if(result == RT_EOK)
    {
        rt_thread_startup(&condition_pannel);
    }

}