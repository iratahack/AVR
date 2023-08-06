#include <stdio.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <time.h>

extern void initUART(void);
extern void initTimer(void);

void outputPins(void)
{
    printf("%02x%2x%2x\n", (PINB & ~_BV(DDB5)), PINC, PIND);
}

ISR(PCINT0_vect)
{
    outputPins();
}

ISR(PCINT1_vect)
{
    outputPins();
}

ISR(PCINT2_vect)
{
    outputPins();
}

void initDigitalIO(void)
{
    // Enable Interrupts for PCINT23-8
    PCICR |= _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0);
    // Enable interrupts
    PCMSK2 = _BV(PCINT18) | _BV(PCINT19) | _BV(PCINT20) | _BV(PCINT21) | _BV(PCINT22) | _BV(PCINT23);
    // Enable interrupts
    PCMSK0 = _BV(PCINT0) | _BV(PCINT1) | _BV(PCINT2) | _BV(PCINT3) | _BV(PCINT4);
    // Enable interrupts
    PCMSK1 = _BV(PCINT0) | _BV(PCINT1) | _BV(PCINT2) | _BV(PCINT3) | _BV(PCINT4) | _BV(PCINT5) | _BV(PCINT6) | _BV(PCINT7);

    // Set port D to input
    DDRD = 0;
    // Enable pull-ups
    PORTD = 0xff;

    // Port B, all input except bit 5 (LED)
    DDRB = _BV(DDB5);
    // Enable pull-ups
    PORTB = ~_BV(DDB5);

    // Set port C to input
    DDRC = 0;
    // Enable pull-ups
    PORTC = 0xff;
}

int main(void)
{
    struct tm timePtr;
    unsigned char oldVal = 0;
    unsigned char newVal = 0;
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    initUART();
    initTimer();
    initDigitalIO();

    printf("Hello from ATmega328p\n");

    timePtr.tm_hour = 9;
    timePtr.tm_isdst = 1;
    timePtr.tm_mday = 6;
    timePtr.tm_min = 56;
    timePtr.tm_mon = 7;
    timePtr.tm_sec = 0;
    timePtr.tm_wday = 0;
    timePtr.tm_yday = 0;
    timePtr.tm_year = 123;

    set_system_time(mktime(&timePtr));

    while (1)
    {
        sleep_cpu();
    }
}
