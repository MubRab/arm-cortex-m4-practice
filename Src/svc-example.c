/*
 * svc-example.c
 *
 * Program that extracts the SVC number
 *
 *  Created on: 28 Feb 2022
 *      Author:
 */
#include <stdint.h>
#include <stdio.h>

extern void initialise_monitor_handles(void);

int main (void)
{
    initialise_monitor_handles();
    __asm volatile("SVC #8");
    while (1);
}

__attribute__ ((naked)) void SVC_Handler(void)
{
    /**
     * this function will not have a prolog and epilog and will
     * not manipulate the MSP.
     * the value of R0 (MSP) will be passed to "arg" of SVC_Handler_C
     * see the Procedure Call Standard for the ARM Architecture
     */
    __asm volatile("MRS R0, MSP");
    __asm volatile("B SVC_Handler_C");
}

/**
 * Extracting the SVC number
 */
void SVC_Handler_C(uint32_t *pArg)
{
    /*the sixth element is the PC, see stacking information during exceptions
     * and subtract two (one for T-bit and another one because we want the previous address*/
    uint8_t *pSVC_number = (uint8_t *) (pArg[6] - 2);
    printf("SVC Number: %d\n", *pSVC_number);
}

