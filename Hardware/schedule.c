#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "stdio.h"

#define MAX_SCHEDULE_ITEMS 10
#define MINUTES_PER_DAY 1440
#define INVALID_TIME 0xFFFF

static uint8_t select_best_schedule(const uint16_t* schedule, uint16_t current) {
    uint8_t best_idx = 0;
    uint16_t min_diff = INVALID_TIME;
    bool found_valid = false;

    for (uint8_t i = 0; i < MAX_SCHEDULE_ITEMS; i++) {
        if (schedule[i] == INVALID_TIME) continue;
        
        if (schedule[i] <= current) {
            uint16_t diff = current - schedule[i];
            if (diff < min_diff) {
                min_diff = diff;
                best_idx = i;
                found_valid = true;
            }
        }
    }

    if (!found_valid) {
        uint16_t max_time = 0;
        for (uint8_t i = 0; i < MAX_SCHEDULE_ITEMS; i++) {
            if (schedule[i] != INVALID_TIME && schedule[i] > max_time) {
                max_time = schedule[i];
                best_idx = i;
            }
        }
    }

    return best_idx;
}

/**
 * @brief  Determines the appropriate schedule index based on the current time and mode.
 * 
 * This function manages a time-based scheduler, supporting two modes:
 * - Mode 0: Finds the most suitable schedule index for the current time.
 * - Mode 1: Sequentially executes schedule items, skipping invalid time entries.
 * 
 * The function uses static variables to track the first run and the last selected index.
 * On the first run, it selects the best schedule index for the current time.
 * In mode 0, it updates the index if a better match is found.
 * In mode 1, it advances to the next valid schedule index in sequence.
 * 
 * @param schedule      Pointer to an array of schedule times.
 * @param current_time  The current time value to compare against the schedule.
 * @param mode          Scheduler mode (0: best match, 1: sequential execution).
 * 
 * @return The selected schedule index.
 */
uint8_t time_scheduler(const uint16_t* schedule,uint16_t current_time,uint8_t mode) 
{
    static bool first_run = true;
    static uint8_t last_idx = 0;

    if (first_run) 
		{
        first_run = false;
        last_idx = select_best_schedule(schedule, current_time);
        return last_idx;
    }

    if (mode == 0) 						//模式1 寻找最合适时间
	{ 
        uint8_t new_idx = select_best_schedule(schedule, current_time);
        if (new_idx != last_idx) 
		{
            last_idx = new_idx;
        }
    } 
	else 											//模式2 顺序执行
    { 
        uint8_t next_idx = (last_idx + 1) % MAX_SCHEDULE_ITEMS;
            
        if(schedule[next_idx] >= INVALID_TIME)						//若为非法时间参数 则跳过
        {
            next_idx ++;
        }
        
        if(next_idx >= MAX_SCHEDULE_ITEMS)									//若为最后一个时段 则重新检查第一个时段
        {
            next_idx = 0;
        }
        
        if (current_time == schedule[next_idx]) 
        {
                last_idx = next_idx;
        }
    }
    return last_idx;
}
