#include <stdio.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <time.h>

extern void initUART(void);
extern void initTimer(void);

ISR(PCINT0_vect)
{
    printf("%02x%2x%2x\n", (PINB & ~_BV(DDB5)), PINC, PIND);
}

ISR_ALIAS(PCINT1_vect, PCINT0_vect);
ISR_ALIAS(PCINT2_vect, PCINT0_vect);

void initDigitalIO(void)
{
    // Enable Interrupts for PCINT23-0
    PCICR = _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0);

    // Unmask pin interrupts, excluding TX, RX, XTAL1, XTAL2, LED, PCINT15
    PCMSK0 = _BV(PCINT0) | _BV(PCINT1) | _BV(PCINT2) | _BV(PCINT3) | _BV(PCINT4);
    PCMSK1 = _BV(PCINT8) | _BV(PCINT9) | _BV(PCINT10) | _BV(PCINT11) | _BV(PCINT12) | _BV(PCINT13) | _BV(PCINT14);
    PCMSK2 = _BV(PCINT18) | _BV(PCINT19) | _BV(PCINT20) | _BV(PCINT21) | _BV(PCINT22) | _BV(PCINT23);

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
    unsigned char oldVal = 0;
    unsigned char newVal = 0;
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    initUART();
    initTimer();
    initDigitalIO();

    printf("Hello from ATmega328p\n");

    while (1)
    {
        sleep_cpu();
    }
}
