/*
 * OS.c
 *
 * Created: 10/19/2019 8:16:40 PM
 *  Author: asere
 */ 

// handler invoked every 100ms

#include <stdint.h>
#include <OStimer.h>
#include <OS.h>

typedef struct task_s{
	taskPtr_t taskPointer;
	uint8_t task_id;
	uint32_t task_time;
	uint32_t task_counter;
	uint8_t state;
}task_t;
task_t tasks[TASK_MAX]; // Max 5 tasks in this mini OS

// init OS timer
void Init_OS(void){
	Init_OStimer();
}
void OS_handlerCallBack(void){
	uint8_t i;
	for(i = 0;i<TASK_MAX;i++){
		if((tasks[i].taskPointer != NULL)&&(tasks[i].state == IDLE)) // check if task exist
		{
			if(tasks[i].task_counter == (tasks[i].task_time-1)){ // time expired
				tasks[i].state = RUN;
				tasks[i].task_counter = 0;
			}else{
				tasks[i].task_counter++;
			}
		}
	}

}
void OS_ServeOS(void)
{
	uint8_t i;
	for(i = 0;i<TASK_MAX;i++){
			if(tasks[i].taskPointer != NULL) // check if task exist
			{
				if(tasks[i].state == RUN){ // time expired
					tasks[i].state = RUNNING;
					tasks[i].taskPointer();
					tasks[i].state = IDLE;
				}
			}
	}
}
// define the TASK_TIME is multiples of 100us
OS_State_t OS_CreateNewTask(taskPtr_t task, uint32_t TASK_TIME)
{
	OS_State_t ret;
	static uint8_t task_indx;
	if(task_indx < TASK_MAX)
	{
		tasks[task_indx].taskPointer = task;
		tasks[task_indx].task_time = TASK_TIME;
		tasks[task_indx].task_id = task_indx+1;
		tasks[task_indx++].task_counter = 0;
		ret = OS_TASK_CREATED;
	}else{
		ret = OS_MAX_TASK_REACHED;
	}
	return ret;
}
