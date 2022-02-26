/*
 * system-exceptions.c
 *
 *  Created on: 26 Feb 2022
 *      Author:
 */
#include <stdint.h>
#include <stdio.h>

/**
 * 1: Undefined Instruction = UsageFault
 * 2: Divide by zero = UsageFault
 * 3: Divide by zero = HardFault (disable UsageFault)
 */
#define EXCEPTION_TYPE          1

#define SHCRS_ADDR              0xE000ED24U
#define CCR_ADDR                0xE000ED14U
#define SRAM_ADDR               0x20010000U

extern void initialise_monitor_handles(void);

static void exception_type()
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

static void enable_sys_faults(void)
{
    /*enable mem-, bus- and usg fault*/
    uint32_t *shcrs = (uint32_t *) SHCRS_ADDR;
    *shcrs |= 0x70000;

    /*enable div by zero trap*/
    uint32_t *ccr = (uint32_t *)CCR_ADDR;
    *ccr |= (1 << 4);

    /*disable usage fault*/
#if (EXCEPTION_TYPE == 3)
    *shcrs &= ~(1 << 18);
#endif
}

int main(void)
{
    initialise_monitor_handles();
    enable_sys_faults();

    /*Undefined instruction*/
#if (EXCEPTION_TYPE == 1)
    uint32_t *pSRAM = (uint32_t *) SRAM_ADDR;
    *pSRAM = 0xFFFFFFFF;
    void (*func_addr) (void);
    func_addr = (void *)(SRAM_ADDR + 1);
    func_addr();
#endif

#if (EXCEPTION_TYPE == 2 || EXCEPTION_TYPE == 3)
    uint32_t val = 100/0;
    (void) val;
#endif

    while(1);
}

/**
 * Fault Handlers
 * see startup code
 */
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

