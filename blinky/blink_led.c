#include <stdio.h>
#include <avr/sleep.h>

extern void initUART(void);
extern void initTimer(void);

int main(void)
{
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    initUART();
    initTimer();

    printf("Hello from ATmega328p\n");

    while (1)
    {
        sleep_cpu();
    }
}
