#ifndef _CONDITION_H_
#define _CONDITION_H_
#include "sys.h"


#define CMD_NUM     5

typedef enum{
    NO_ACT = 0,                         //未触发状态
    INIT,                               //初始状态，即触发后的第一个状态
    WAIT_STAGE_1,                       //初始状态到阶段1之间
    STAGE_1,                            //阶段1
    WAIT_STAGE_2,                       //阶段1到阶段2之间
    STAGE2,                             //阶段2
    END                                 //结束状态
}condition_status;

typedef struct
{
	uint8_t J_status[IO_NUM];								//场景模式继电器控制逻辑

    char cmd[CMD_NUM][16];                                  //场景模式的控制指令 每个阶段最多支持五条指令
    uint8_t cmd_num;                                        //记录指令数量
    void (*state_callback)(uint8_t condition_id , uint8_t state_id);                           //场景状态对应的回调函数
}condition_control_t;

typedef struct
{
    uint8_t act_status;                     //场景是否被触发
    uint16_t time_period1;                  //时间段1
    uint16_t time_period2;                  //时间段2
    condition_status status;                //运行状态
    condition_control_t state[3];			//状态1预设逻辑

}condition_t;

#define CUSTOM_NUM			32

typedef struct
{
	char name[16];
	uint8_t cmd[16];
	uint8_t cmd_len;				
	uint8_t trigger;			
	uint8_t type;									//指令类型 0：自定义指令 发送时不添加校验  1：modbus指令 发送时添加crc16校验	
}custom_cmd_t;

#define CONDITION_NUM       2

/*场景编号*/
#define ARRIVER_CONDITION     0
#define LEAVE_CONDITION       1

/*状态编号*/
#define INIT_STATE_ID        0
#define STATE1_ID           1
#define STATE2_ID           2
extern condition_t condition[CONDITION_NUM];
void conditionl_poll(void);
void condition_init(void);
void states_callback(uint8_t condition_id , uint8_t state_id);




#endif
