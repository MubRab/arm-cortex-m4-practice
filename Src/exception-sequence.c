#include <stdint.h>
#include <stdio.h>

extern void initialise_monitor_handles(void);

uint32_t lr_val = 0;
uint32_t ipsr_reg = 0;

void generate_exception()
{
    uint32_t *pSTIR = (uint32_t *) 0xE000EF00;
    uint32_t *pISER0 = (uint32_t *) 0xE000E100;

    *pISER0 |= (1 << 3);

    *pSTIR |= (3 & 0x1FF);
}

int main(void)
{
    initialise_monitor_handles();

    ipsr_reg = 0;
    __asm volatile("MRS %0,IPSR": "=r"(ipsr_reg) : : );
    ipsr_reg &= 0x1FF;

    if (ipsr_reg)
    {
        printf("Handler Mode\n");
    }
    else
    {
        printf("Thread Mode\n");
    }

    generate_exception();

    ipsr_reg = 0;

    __asm volatile("MRS %0,IPSR": "=r"(ipsr_reg) : : );
    ipsr_reg &= 0x1FF;

    if (ipsr_reg)
    {
        printf("Handler Mode\n");
    }
    else
    {
        printf("Thread Mode\n");
    }

    while(1);
}

void RTC_WKUP_IRQHandler(void)
{
    /*Link Register (LR) is loaded with EXC_RETURN at this point*/
    __asm volatile("MOV %0,R14": "=r"(lr_val) : : );

    printf("Exception Generated\n");

    ipsr_reg = 0;
    __asm volatile("MRS %0,IPSR": "=r"(ipsr_reg) : : );
    ipsr_reg &= 0x1FF;

    if (ipsr_reg)
    {
        printf("Handler Mode\n");
    }
    else
    {
        printf("Thread Mode\n");
    }

    printf("Link Register(LR) value: %lX\n", lr_val);

    printf("Exiting Exception Handler\n");
}
