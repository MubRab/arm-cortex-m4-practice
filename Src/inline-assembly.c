/*
 * inline-assembly.c
 *
 * Playing around with in-line assembly in C
 *
 *  Created on: Feb 22, 2022
 *      Author:
 */


#include <stdio.h>

extern void initialise_monitor_handles(void);

int main(void)
{
    initialise_monitor_handles();

    /**
     * This block of code copies contents of CONTROL register of the ARM Cortex M4 Core
     * to a C variable
     *
     * The C variable is then used to determine whether the program is in privileged mode
     * or unprivileged mode.
     *
     * The C variable is then changed to unprivileged mode and is copied back to the
     * CONTROL register
     *
     * In order to read/write to Special Registers, use MRS or MSR
     */
    uint32_t ctrl_reg;

    __asm volatile("MRS %0,CONTROL": "=r"(ctrl_reg) : : );

    if (ctrl_reg & 0x1)
    {
        printf("Thread Mode: Unprivileged mode\n");
    }
    else
    {
        printf("Thread Mode: Privileged mode\n");
    }

    printf("Changing to Unprivileged mode\n");

    ctrl_reg |= (1 << 0);
    __asm volatile("MSR CONTROL,%0": : "r"(ctrl_reg) : );
    __asm volatile("MRS %0,CONTROL": "=r"(ctrl_reg) : : );

    if (ctrl_reg & 0x1)
    {
        printf("Thread Mode: Unprivileged mode\n");
    }
    else
    {
        printf("Thread Mode: Privileged mode\n");
    }
    /**
     * End of Block
     */

    printf("\n");

    /**
     * This block copies contents of a C pointer to
     * another C variable
     */
    uint8_t c = 'A';
    uint32_t *ptrC = (uint32_t* )&c;
    uint8_t val = 0;

    printf("Value of val before change: %c\n", val);

    __asm volatile("LDR %0,[%1]" : "=r"(val) : "r"(ptrC) : );

    printf("Value of val after change: %c\n", val);
    /*
     * End of block
     */

    while (1);
}
