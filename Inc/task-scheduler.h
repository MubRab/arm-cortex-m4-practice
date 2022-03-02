/*
 * task-scheduler.h
 *
 *  Created on: 01 Mar 2022
 *      Author:
 */

#ifndef TASK_SCHEDULER_H_
#define TASK_SCHEDULER_H_

#include <stdint.h>

/*
 * Macros for stack memory
 */
#define NUM_TASKS                       4
#define STACK_TASK_SIZE                 1024U   /*size of user stack is 1KB*/
#define STACK_SCHEDULER_SIZE            1024U   /*size of scheduler stack is 1KB*/
#define SRAM_BASE_ADDR                  0x20000000U
#define SRAM_SIZE                       (192 * 1024)
#define TASK1_STACK_BASE                (SRAM_BASE_ADDR + SRAM_SIZE)
#define TASK2_STACK_BASE                (TASK1_STACK_BASE - STACK_TASK_SIZE)
#define TASK3_STACK_BASE                (TASK2_STACK_BASE - STACK_TASK_SIZE)
#define TASK4_STACK_BASE                (TASK3_STACK_BASE - STACK_TASK_SIZE)
#define SCHEDULER_STACK_BASE            (TASK4_STACK_BASE - STACK_TASK_SIZE)

#define CLK_FREQ_HZ                     16000000U
#define SYSTICK_FREQ_HZ                 CLK_FREQ_HZ
#define DESIRED_FREQ_HZ                 1000U

#define XPSR_INIT                       0x01000000U
#define LR_INIT                         0xFFFFFFFDU


/*faults*/
#define SHCRS_ADDR                      0xE000ED24U
#define CCR_ADDR                        0xE000ED14U
#define SRAM_ADDR                       0x20010000U

#define TASK_STATE_BLOCKED              0
#define TASK_STATE_RUNNING              1

typedef struct
{
    uint32_t psp_value;
    uint32_t block_count;
    uint8_t current_state;
    void (*task_handler) (void);
} task_control_block_t;

task_control_block_t tasks[NUM_TASKS];


/**
 * Function Prototypes
 */
void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

void init_systick(uint32_t desired_freq_hz);
__attribute__ ((naked)) void init_stack_scheduler(uint32_t stack_address);
void init_tasks(void);
__attribute__ ((naked)) void init_sp(void);
void enable_sys_faults(void);

void exception_type(void);
void update_task(void);
void save_psp(uint32_t psp_val);


#endif /* TASK_SCHEDULER_H_ */
