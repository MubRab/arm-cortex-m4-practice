/*
 * semi-hosting.c
 *
 * Unable to use the ITM component of the ARM Cortex M4 processor for printf(),
 * without the use of USART/UART, for the STM32F429 board
 * see:
 * https://web.archive.org/web/20220222101721/https://electronics.stackexchange.com/questions/421204/stm32f429i-discovery-board-unable-to-do-printf-redirection-using-swv-itm
 *
 * Setting up semi-hosting
 * - Use ST-LINK(OpenOCD) debug probe
 * - add the command "monitor arm semihosting enable"
 * - add the flags for the linker: "-specs=rdimon.specs -lc -lrdimon"
 * - add the initialise_monitor_handles(); in main
 * - exclude syscalls.c
 *
 *  Created on: Feb 22, 2022
 *      Author:
 */
#include <stdio.h>

extern void initialise_monitor_handles(void);

int main(void)
{
    initialise_monitor_handles();
    printf("Hello World\n");
    while (1);
}
