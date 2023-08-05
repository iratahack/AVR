#include <time.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define MS_DELAY 500

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
    // Toggle LED
    PORTB ^= _BV(PORTB5);
    system_tick();
    reti();
}

void setupTimer(void)
{
    // Enable timer COMPA interrupt
    TIMSK1 |= _BV(OCIE1A);
    // Set timer compare value
    OCR1A = (F_CPU/256);
    // Select timer clock
    TCCR1B = _BV(CS12); // clk/256
}

int main(void)
{
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();

    // Set bit 5 of DDRB to one - Set digital pin 13 to output mode
    DDRB |= _BV(DDB5);

    setupTimer();

    // Enable interrupts
    sei();

    while (1)
    {
        sleep_cpu();
    }
}