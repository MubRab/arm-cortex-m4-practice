/*
 * bit-banding.c
 *
 *  Created on: 23 Feb 2022
 *      Author:
 */
#include <stdint.h>
#include <stdio.h>

#define BASE_ADDR       0x20000000U
#define ADDR            0x20000200U
#define BIT             7
#define ALIAS_BASE      0x22000000U
#define BIT_OFFSET()    (((ADDR - BASE_ADDR) * 32) + (BIT * 4))
#define ALIAS_ADDR()    (ALIAS_BASE + BIT_OFFSET())

extern void initialise_monitor_handles(void);

int main(void)
{
    initialise_monitor_handles();

    /**
     * E.g. At memory location 0x2000_0200, load the byte with 0xFF
     *      and then clear the 7th bit
     *
     * to calculate alias address:
     *  bit_offset = (byte_offset * 32) + (bit_number * 4)
     *  alias_address = alias_base + bit_offset
     */

    uint32_t *ptr = (uint32_t *) ADDR;
    *ptr = 0xFF;
    printf("Value at address (before change): %lX\n", *ptr);
    *ptr = 0xFF; /*I have to rewrite, since printf clears the contents of the address*/

    /*
     * Using bit-band aliasing
     *  - uses less assembly instructions as opposed to using *ptr &= ~(1 << 7)
     */
    uint32_t *alias_address = (uint32_t *) ALIAS_ADDR();
    *alias_address = 0;
    printf("Value at address (after change): %lX\n", *ptr);

    while(1);
}
