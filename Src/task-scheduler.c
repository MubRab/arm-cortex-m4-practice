/*
 * task-scheduler.c
 *
 * Round robin scheduler of various user tasks
 *
 *  Created on: 28 Feb 2022
 *      Author:
 */

#include <stdint.h>
#include <stdio.h>

extern void initialise_monitor_handles(void);

void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

/*
 * Macros for stack memory
 */
#define STACK_TASK_SIZE                 1024U   /*size of user stack is 1KB*/
#define STACK_SCHEDULER_SIZE            1024U   /*size of scheduler stack is 1KB*/
#define SRAM_BASE_ADDR                  0x200000000U
#define SRAM_SIZE                       (192 * 1024)
#define TASK1_STACK_BASE                (SRAM_BASE_ADDR + SRAM_SIZE)
#define TASK2_STACK_BASE                (TASK1_STACK_BASE - STACK_TASK_SIZE)
#define TASK3_STACK_BASE                (TASK2_STACK_BASE - STACK_TASK_SIZE)
#define TASK4_STACK_BASE                (TASK3_STACK_BASE - STACK_TASK_SIZE)
#define SCHEDULER_STACK_BASE            (TASK4_STACK_BASE - STACK_TASK_SIZE)

int main (void)
{
    initialise_monitor_handles();
    while(1);
}


void task1_handler(void)
{
    while(1)
    {

    }
}

void task2_handler(void)
{
    while(1)
    {

    }
}

void task3_handler(void)
{
    while(1)
    {

    }
}

void task4_handler(void)
{
    while(1)
    {

    }
}
