/*
 * task-scheduler.c
 *
 * Round robin scheduler of various user tasks
 *
 *  Created on: 28 Feb 2022
 *      Author:
 */

#include <stdio.h>
#include "task-scheduler.h"

extern void initialise_monitor_handles(void);

static uint8_t task_current = 1;
static uint32_t global_tick_count = 0;

int main (void)
{
    initialise_monitor_handles();
    enable_sys_faults();
    init_stack_scheduler((uint32_t) SCHEDULER_STACK_BASE);
    init_tasks();
    init_systick(DESIRED_FREQ_HZ);
    init_sp();
    task1_handler();
    while(1);
}


void task1_handler(void)
{
    while(1)
    {
        printf("Task1 before block\n");
        task_delay(100);
        printf("Task1 after block\n");
    }
}

void task2_handler(void)
{
    while(1)
    {
        printf("Task2 before block\n");
        task_delay(150);
        printf("Task2 after block\n");
    }
}

void task3_handler(void)
{
    while(1)
    {
        printf("Task3 before block\n");
        task_delay(10);
        printf("Task3 after block\n");
    }
}

void task4_handler(void)
{
    while(1)
    {
        printf("Task4 before block\n");
        task_delay(50);
        printf("Task4 after block\n");
    }
}

void idle_task_handler(void)
{
    while(1)
    {
        printf("Idle Task\n");
    }
}

/**
 * Init functions
 */

void enable_sys_faults(void)
{
    /*enable mem-, bus- and usg fault*/
    uint32_t *shcrs = (uint32_t *) SHCRS_ADDR;
    *shcrs |= 0x70000;

    /*enable div by zero trap*/
    uint32_t *ccr = (uint32_t *)CCR_ADDR;
    *ccr |= (1 << 4);
}

void init_systick(uint32_t desired_freq_hz)
{
    uint32_t reload_val = (SYSTICK_FREQ_HZ / desired_freq_hz) - 1;

    uint32_t *pSYSTCSR = (uint32_t *)0xE000E010;
    uint32_t *pSYSTRVR = (uint32_t *)0xE000E014;

    *pSYSTRVR &= ~(0x00FFFFFF);
    *pSYSTRVR |= reload_val;

    *pSYSTCSR |= (1 << 1);
    *pSYSTCSR |= (1 << 2);
    *pSYSTCSR |= (1 << 0);
}

__attribute__ ((naked)) void init_stack_scheduler(uint32_t stack_address)
{
    __asm volatile("MSR MSP, %0": : "r" (stack_address) : );
    __asm volatile("BX LR");
}

void init_tasks(void)
{
    for (int i = 0; i < NUM_TASKS; ++i)
    {
        tasks[i].current_state = TASK_STATE_READY;

        switch(i)
        {
            case 0:
                tasks[i].psp_value = TASK_IDLE_STACK_BASE;
                tasks[i].task_handler = idle_task_handler;
                break;
            case 1:
                tasks[i].psp_value = TASK1_STACK_BASE;
                tasks[i].task_handler = task1_handler;
                break;
            case 2:
                tasks[i].psp_value = TASK2_STACK_BASE;
                tasks[i].task_handler = task2_handler;
                break;
            case 3:
                tasks[i].psp_value = TASK3_STACK_BASE;
                tasks[i].task_handler = task3_handler;
                break;
            case 4:
                tasks[i].psp_value = TASK4_STACK_BASE;
                tasks[i].task_handler = task4_handler;
                break;
        }
    }


    uint32_t *pPSP;

    for (int i = 0; i < NUM_TASKS; ++i)
    {
        pPSP = (uint32_t *) tasks[i].psp_value;

        --pPSP;
        *pPSP = XPSR_INIT;

        --pPSP;
        *pPSP = (uint32_t) tasks[i].task_handler;

        --pPSP;
        *pPSP = LR_INIT;

        /*for R0-R12*/
        for (int j = 0; j <= 12; ++j)
        {
            --pPSP;
            *pPSP = 0;
        }
        tasks[i].psp_value = (uint32_t) pPSP;
    }
}

__attribute__ ((naked)) void init_sp(void)
{
//    __asm volatile("PUSH {LR}");    /*Preserve LR value in stack before branching*/
//    __asm volatile("BL get_psp");
//    __asm volatile("MSR PSP, R0");
//    __asm volatile("POP {LR}");    /*Restore LR value from stack before branching*/
    __asm volatile("MSR PSP, %0": : "r" (tasks[task_current].psp_value) : );
    __asm volatile("MOV R0, #0x02");
    __asm volatile("MSR CONTROL, R0");
    __asm volatile("BX LR");
}

/**
 * Handler Functions
 */

__attribute__ ((naked)) void PendSV_Handler(void)
{
    /**
     * Saving current task's context
     */

    /*Obtain current PSP value*/
    __asm volatile("MRS R0, PSP");
    /**
     * use STMDB to store muliple registers
     * do not use PUSH operation, since it will store it in MSP,
     * but we want to store in PSP
     **/
    __asm volatile("STMDB R0!, {R4-R11}");
    /*Save the value of PSP*/
    __asm volatile("PUSH {LR}");    /*Preserve LR value in stack before branching*/
    __asm volatile("BL save_psp");
    __asm volatile("POP {LR}");    /*Restore LR value from stack before branching*/

    /**
     * Retrieving next task's context
     */
    __asm volatile("PUSH {LR}");    /*Preserve LR value in stack before branching*/
    __asm volatile("BL update_task");
    __asm volatile("POP {LR}");    /*Restore LR value from stack before branching*/
    __asm volatile("MOV R0, %0": : "r" (tasks[task_current].psp_value) : );
    __asm volatile("LDMIA R0!, {R4-R11}");
    __asm volatile("MSR PSP, R0");

    __asm volatile("BX LR");
}

void SysTick_Handler(void)
{
    ++global_tick_count;

    for (int i = 1; i < NUM_TASKS; ++i)
    {
        if (tasks[i].current_state == TASK_STATE_BLOCKED && tasks[i].block_count == global_tick_count)
        {
            tasks[i].current_state = TASK_STATE_READY;
        }
    }

    //pends the PendSV
    uint32_t *pICSR = (uint32_t *) 0xE000ED04;
    *pICSR |= (1 << 28);
}

void HardFault_Handler(void)
{
    exception_type();
    while (1);
}

void MemManage_Handler(void)
{
    exception_type();
    while (1);
}

void BusFault_Handler(void)
{
    exception_type();
    while (1);
}

void UsageFault_Handler(void)
{
    exception_type();
    while(1);
}

/**
 * Miscellaneous functions
 */

void update_task(void)
{
    uint32_t blocked_task_count = 0;

    do
    {
        ++task_current;
        task_current %= NUM_TASKS;

        if ( (task_current != 0) && (tasks[task_current].current_state == TASK_STATE_READY) )
        {
            break;
        }
        else if (tasks[task_current].current_state == TASK_STATE_BLOCKED)
        {
            ++blocked_task_count;
            continue;
        }
        else
        {
            continue;
        }
    } while (blocked_task_count < (NUM_TASKS - 1));

    if (blocked_task_count == (NUM_TASKS - 1))
    {
        task_current = 0;
    }
}

void save_psp(uint32_t psp_val)
{
    tasks[task_current].psp_value = psp_val;
}

void task_delay(uint32_t ticks)
{
    INTERRUPT_DISABLE();

    if (task_current > 0)
    {
        tasks[task_current].block_count = global_tick_count + ticks;
        tasks[task_current].current_state = TASK_STATE_BLOCKED;
        /* pends the PendSV, since task is blocked */
        uint32_t *pICSR = (uint32_t *) 0xE000ED04;
        *pICSR |= (1 << 28);
    }

    INTERRUPT_ENABLE();
}

void exception_type(void)
{
    uint32_t ipsr_reg = 0;
    __asm volatile("MRS %0,IPSR": "=r"(ipsr_reg) : : );
    ipsr_reg &= 0x1FF;

    switch (ipsr_reg)
    {
        case 0:
            printf("Thread Mode\n");
            break;
        case 3:
            printf("HardFault Exception\n");
            break;
        case 4:
            printf("MemManage Exception\n");
            break;
        case 5:
            printf("BusFault Exception\n");
            break;
        case 6:
            printf("UsageFault Exception\n");
            break;
        default:
            printf("Other Exception\n");
            break;
    }
}



