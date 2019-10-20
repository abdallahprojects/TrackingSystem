/*
 * OS.h
 *
 * Created: 10/19/2019 8:16:54 PM
 *  Author: asere
 */ 


#ifndef OS_H_
#define OS_H_
// Congfig //
#define TASK_MAX 5


// Config END //
#define RUN  1
#define RUNNING 2
#define IDLE  0
#ifndef NULL
#define NULL (void*)0
#endif
typedef enum OS_State_e{
	OS_TASK_CREATED,
	OS_MAX_TASK_REACHED
}OS_State_t;
void Init_OS(void);
typedef void(*taskPtr_t)(void);

void OS_handlerCallBack(void);
OS_State_t OS_CreateNewTask(taskPtr_t task, uint16_t TASK_TIME);
void OS_ServeOS(void);

#endif /* OS_H_ */
